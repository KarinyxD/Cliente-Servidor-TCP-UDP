#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#define TIMEOUT_SEC 5 // Definindo o tempo limite de 5 segundos
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Verifica se o IP e a porta foram passados como argumentos
    if (argc != 3) { 
        fprintf(stderr, "Uso: %s <IP do servidor> <PORTA>\n", argv[0]);
        exit(1);
    }
    
    int client_sock;
    struct sockaddr_in serveraddr;
    socklen_t addr_len;
    char buffer[1023];
    ssize_t n;
    double time_taken = 0;
    size_t expected_packets = 0;
    size_t packets_received = 0;
    struct timespec start_time, end_time;

    // Cria o socket UDP
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0)
        error("Erro ao abrir o socket");

    memset(&serveraddr, 0, sizeof(serveraddr)); // Zerando a estrutura serveraddr
    serveraddr.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr); // converte o IP do servidor para binario
    serveraddr.sin_port = htons(atoi(argv[2])); // porta do servidor na ordem de bytes correta

    addr_len = sizeof(serveraddr);

    // Envia uma solicitação ao servidor(pois no UDP nao se estabelece conexao)
    const char *request = "Solicitação de arquivo";
    n = sendto(client_sock, request, strlen(request), 0, (struct sockaddr *)&serveraddr, addr_len);
    if (n < 0)
        error("Erro ao enviar solicitação");
    printf("Solicitação enviada ao servidor.\n");

    // Abrindo o arquivo para escrita no diretório arquivos_cliente
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "../arquivos_cliente/arquivo_recebidoUDP.bin");
    int filefd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo para escrita");
        close(client_sock);
        exit(1);
    }

    // Recebe o número de pacotes esperados do servidor
    n = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&serveraddr, &addr_len);
    if (n < 0) {
        error("Erro ao receber número de pacotes");
    }
    buffer[n] = '\0';  // Garantir que a string de pacotes tenha um terminador
    expected_packets = strtoul(buffer, NULL, 10);  // Converte a string recebida para número de pacotes
    printf("Número de pacotes esperados: %zu\n", expected_packets);

    // Inicializa o fd_set e timeout para o select
    fd_set readfds;
    struct timeval timeout;

    // Recebe os dados do servidor em blocos
    printf("Recebendo arquivo...\n");
    size_t total_bytes_received = 0;
    while (1) {

        // Preenche o fd_set
        FD_ZERO(&readfds);
        FD_SET(client_sock, &readfds);

        // Define o tempo limite de 5 segundos
        timeout.tv_sec = TIMEOUT_SEC;
        timeout.tv_usec = 0;

        // Espera até que o socket esteja pronto para leitura ou o timeout seja alcançado
        int ret = select(client_sock + 1, &readfds, NULL, NULL, &timeout);

        if (ret == -1) {
            perror("Erro ao usar select");
            close(filefd);
            close(client_sock);
            exit(1);
        } else if (ret == 0) {
            // Timeout ocorreu
            printf("Timeout: Nenhum dado recebido em %d segundos.\n", TIMEOUT_SEC);
            break;
        } else {
            // Dados disponíveis para leitura
            if (FD_ISSET(client_sock, &readfds)) {
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                n = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&serveraddr, &addr_len);
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                time_taken += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

                if (n < 0) {
                    perror("Erro ao receber dados do servidor");
                    close(filefd);
                    close(client_sock);
                    exit(1);
                }

                total_bytes_received += n;

                // Verifica se a mensagem é "EOF"(se chegou ao final)
                if (strncmp(buffer, "EOF", n) == 0) {
                    printf("Fim da transmissão.\n");
                    break;
                }

                // Grava os dados no arquivo
                if (write(filefd, buffer, n) < 0) {
                    perror("Erro ao gravar no arquivo");
                    close(filefd);
                    close(client_sock);
                    exit(1);
                }
                packets_received++; // Incrementa a quantidade de pacotes recebidos
            }
        }
    }

    printf("Numero de pacotes recebidos: %zu\n", packets_received);
    printf("Porcetagem de perda de pacotes: %.2f%%\n", (1 - (double)packets_received / expected_packets) * 100);
    // printf("time: %f\n", time_taken);
    // printf("total_bytes_received: %ld\n", total_bytes_received);
    // Calculando a taxa de download (megabytes por segundo)
    double download_rate = (total_bytes_received / (1024*1024)) / time_taken;
    printf("Taxa de download: %.2f MB/s\n", download_rate);
    printf("Arquivo recebido e salvo como 'arquivos_cliente/arquivo_recebidoUDP.bin'.\n");

    close(filefd);
    close(client_sock);
    return 0;
}
