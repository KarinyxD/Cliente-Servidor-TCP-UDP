#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int client_sock, portno;
    struct sockaddr_in serv_addr;
    socklen_t addr_len;
    char buffer[1023];
    ssize_t n;

    if (argc < 3) { // Verifica se o IP e a porta foram passados como argumentos
        fprintf(stderr, "Uso: %s <IP do servidor> <PORTA>\n", argv[0]);
        exit(1);
    }

    // Obtém o número da porta
    portno = atoi(argv[2]);

    // Cria o socket UDP
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0)
        error("Erro ao abrir o socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    // Converte o endereço IP do servidor
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
        error("Erro no endereço IP do servidor");

    addr_len = sizeof(serv_addr);

    // Envia uma solicitação ao servidor
    const char *request = "Solicitação de arquivo";
    n = sendto(client_sock, request, strlen(request), 0, (struct sockaddr *)&serv_addr, addr_len);
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

    // Recebe os dados do servidor em blocos
    printf("Recebendo arquivo...\n");
    while (1) {
        n = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&serv_addr, &addr_len);
        if (n < 0) {
            perror("Erro ao receber dados do servidor");
            close(filefd);
            close(client_sock);
            exit(1);
        }

        // Verifica se a mensagem é "EOF"
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
    }


    printf("Arquivo recebido e salvo como 'arquivo_recebido.bin'.\n");

    close(filefd);
    close(client_sock);
    return 0;
}
