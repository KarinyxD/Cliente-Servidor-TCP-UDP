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

void calculate_md5(const char *filepath, unsigned char *result) {
    int filefd = open(filepath, O_RDONLY);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo para calcular MD5");
        exit(1);
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erro ao criar contexto MD5");
        close(filefd);
        exit(1);
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erro ao inicializar MD5");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(filefd, buffer, BUFFER_SIZE)) > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            perror("Erro ao atualizar MD5");
            EVP_MD_CTX_free(mdctx);
            close(filefd);
            exit(1);
        }
    }

    if (bytes_read < 0) {
        perror("Erro ao ler o arquivo para calcular MD5");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        exit(1);
    }

    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, result, &md_len) != 1) {
        perror("Erro ao finalizar MD5");
        EVP_MD_CTX_free(mdctx);
        close(filefd);
        exit(1);
    }

    EVP_MD_CTX_free(mdctx);
    close(filefd);
}

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
    int filefd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo para escrita");
        close(client_sock);
        exit(1);
    }

    // Recebe os dados do servidor em blocos e escreve no arquivo
    ssize_t bytes_received;
    while ((bytes_received = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        // Medindo o tempo de recepção dos bytes
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        total_bytes_received += bytes_received;

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        time_taken += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        if (write(filefd, buffer, bytes_received) != bytes_received) {
            perror("Erro ao escrever no arquivo");
            close(filefd);
            close(client_sock);
            exit(1);
        }
    }

    if (bytes_received < 0) {
        perror("Erro ao ler dados do servidor");
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
    calculate_md5(filepath, client_md5);

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