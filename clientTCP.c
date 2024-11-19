#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <time.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024  // Tamanho do buffer para receber os dados

int main(int argc, char **argv) {
    // Verificando se o IP do servidor e a porta foram passados como argumentos
    if (argc != 3) {
        fprintf(stderr, "Use: %s <IP do servidor> <porta>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serveraddr; // Estrutura para armazenar o endereço do servidor
    int client_sock; 
    char buffer[BUFFER_SIZE]; 
    size_t total_bytes_received = 0; 
    struct timespec start_time, end_time; 
    double time_taken; // Tempo total de download em segundos

    // Criando o socket TCP
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr)); // Zerando a estrutura serveraddr
    serveraddr.sin_family = AF_INET; // IPV4
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr); // converte o IP do servidor para binario
    serveraddr.sin_port = htons(atoi(argv[2]));

    // Conectando ao servidor
    if (connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("connect() error");
        close(client_sock);
        exit(1);
    }

    // Recebendo o hash MD5 do servidor
    unsigned char server_md5[EVP_MAX_MD_SIZE];
    if (read(client_sock, server_md5, EVP_MD_size(EVP_md5())) != EVP_MD_size(EVP_md5())) {
        perror("Erro ao receber o hash MD5 do servidor");
        close(client_sock);
        exit(1);
    }

    // Abrindo o arquivo para escrita no diretório arquivos_cliente
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "arquivos_cliente/arquivo_recebido");
    int filefd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo para escrita");
        close(client_sock);
        exit(1);
    }

    // Início da função para calcular o hash MD5 do arquivo recebido
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erro ao criar contexto MD5");
        close(filefd);
        close(client_sock);
        exit(1);
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erro ao inicializar MD5");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        close(client_sock);
        exit(1);
    }

    // Iniciando a medição do tempo de download
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    ssize_t bytes_received;
    while ((bytes_received = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        // Medindo o tempo de recepção dos bytes
        struct timespec temp_start, temp_end;
        clock_gettime(CLOCK_MONOTONIC, &temp_start);

        total_bytes_received += bytes_received;

        clock_gettime(CLOCK_MONOTONIC, &temp_end);
        time_taken += (temp_end.tv_sec - temp_start.tv_sec) + (temp_end.tv_nsec - temp_start.tv_nsec) / 1e9;

        if (write(filefd, buffer, bytes_received) != bytes_received) {
            perror("Erro ao escrever no arquivo");
            EVP_MD_CTX_free(mdctx);
            close(filefd);
            close(client_sock);
            exit(1);
        }
        if (EVP_DigestUpdate(mdctx, buffer, bytes_received) != 1) {
            perror("Erro ao atualizar MD5");
            EVP_MD_CTX_free(mdctx);
            close(filefd);
            close(client_sock);
            exit(1);
        }
    }

    // Finalizando a medição do tempo de download
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    if (bytes_received < 0) {
        perror("Erro ao ler dados do servidor");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        close(client_sock);
        exit(1);
    }

    // Calculando a taxa de download (bytes por segundo)
    double download_rate = total_bytes_received / time_taken;
    printf("\nArquivo recebido com sucesso!\n");
    printf("Taxa de download: %.2f bytes por segundo\n", download_rate);

    // Calculando o hash MD5 do arquivo recebido
    unsigned char client_md5[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, client_md5, &md_len) != 1) {
        perror("Erro ao finalizar MD5");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        close(client_sock);
        exit(1);
    }

    EVP_MD_CTX_free(mdctx);

    // Verificando a integridade dos dados
    if (memcmp(server_md5, client_md5, EVP_MD_size(EVP_md5())) == 0) {
        printf("Integridade dos dados verificada com sucesso!\n");
    } else {
        printf("Falha na verificação da integridade dos dados!\n");
    }

    // Fechando o arquivo e o socket
    close(filefd);
    close(client_sock);

    return 0;
}