#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <time.h>

//#define PORT 12348 // Porta do servidor
#define BUFFER_SIZE 1024  // Tamanho do buffer para receber os dados

int main(int argc, char **argv) {
    // Verificando se o IP do servidor foi passado como argumento
    if (argc != 3) {
        fprintf(stderr, "Use: %s <IP do servidor> <Porta> \n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serveraddr; // Estrutura para armazenar o endereço do servidor
    int client_sock; // Identificador do socket do cliente
    char buffer[BUFFER_SIZE]; // Buffer dos dados recebidos
    size_t total_bytes_received = 0; // Total de bytes recebidos
    clock_t start_time, end_time; // medir os tempos de inicio e fim da transferencia(taxa de download)
    double time_taken; // Tempo total de download em segundos

    // Criando o socket TCP
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr)); // Zerando a estrutura serveraddr
    serveraddr.sin_family = AF_INET; // IPV4
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr); // converte o IP do servidor para binario
    serveraddr.sin_port = htons(atoi(argv[2])); // porta do servidor na ordem de bytes correta

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



    // Recebe os dados do servidor em blocos e imprime no terminal
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erro ao criar contexto MD5");
        close(client_sock);
        exit(1);
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erro ao inicializar MD5");
        close(client_sock);
        exit(1);
    }

    // Iniciando a medição do tempo de download
    start_time = clock();

    ssize_t bytes_received;
    while ((bytes_received = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        total_bytes_received += bytes_received;
        //fwrite(buffer, 1, bytes_received, stdout);
        if (EVP_DigestUpdate(mdctx, buffer, bytes_received) != 1) {
            perror("Erro ao atualizar MD5");
            close(client_sock);
            exit(1);
        }
    }

    // Finalizando a medição do tempo de download
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Calculando a taxa de download (bytes por segundo)
    double download_rate = total_bytes_received / time_taken;
    printf("\nArquivo recebido com sucesso!\n");
    printf("Taxa de download: %.2f bytes por segundo\n", download_rate);

    // Calculando o hash MD5 do arquivo recebido
    unsigned char client_md5[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, client_md5, &md_len) != 1) {
        perror("Erro ao finalizar MD5");
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

    // Fechando o socket
    close(client_sock);

    return 0;
}