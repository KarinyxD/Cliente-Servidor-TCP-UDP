#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <openssl/sha.h>

#define PORT 12345 // Porta do servidor
#define BUFFER_SIZE 1024  // Tamanho do buffer para receber os dados

int main(int argc, char **argv) {
    // Verificando se o IP do servidor foi passado como argumento
    if (argc != 2) {
        fprintf(stderr, "Use: %s <IP do servidor>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serveraddr; // Estrutura para armazenar o endereço do servidor
    int client_sock; // Identificador do socket do cliente
    char buffer[BUFFER_SIZE]; // Buffer dos dados recebidos
    size_t total_bytes_received = 0; // Total de bytes recebidos
    clock_t start_time, end_time; // medir os tempos de inicio e fim da transferencia(taxa de download)
    double time_taken; // Tempo total de download em segundos
    unsigned char expected_hash[SHA256_DIGEST_LENGTH];
    unsigned char received_hash[SHA256_DIGEST_LENGTH];

    // Criando o socket TCP
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr)); // Zerando a estrutura serveraddr
    serveraddr.sin_family = AF_INET; // IPV4
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr); // converte o IP do servidor para binario
    serveraddr.sin_port = htons(PORT); // porta do servidor na ordem de bytes correta

    // Conectando ao servidor
    if (connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("connect() error");
        exit(1);
    }

    // Enviando a solicitação do arquivo para o servidor
    char request[] = "SEND_FILE";
    if (write(client_sock, request, strlen(request)) == -1) {
        perror("write() error");
        exit(1);
    }
    
    // Receber o hash do servidor
    int n = read(client_sock, received_hash, SHA256_DIGEST_LENGTH);
    if (n < 0) {
        perror("ERROR reading hash from server");
        exit(1);
    }

    // Iniciando a medição do tempo de download
    start_time = clock();

    // Recebe os dados do servidor em blocos e calcula o hash do conteúdo recebido
    size_t bytes_received;
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);

    while ((bytes_received = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        total_bytes_received += bytes_received;
        SHA256_Update(&sha256_ctx, buffer, bytes_received); // Atualiza o hash com os dados recebidos
    }

    SHA256_Final(expected_hash, &sha256_ctx);  // Finaliza o cálculo do hash    // Recebe os dados do servidor em blocos
    size_t bytes_received;
    while ((bytes_received = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        total_bytes_received += bytes_received;
    }

    // Finalizando a medição do tempo de download
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Calculando a taxa de download (bytes por segundo)
    double download_rate = total_bytes_received / time_taken;
    printf("Arquivo recebido com sucesso!\n");
    printf("Taxa de download: %.2f bytes por segundo\n", download_rate);

    // Fechando o socket
    close(client_sock);

    return 0;
}
