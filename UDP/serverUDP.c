#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/evp.h>

char *ROOT="../arquivos_servidor"; // diretorio dos arquivos

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){   
    int sockfd, portno; // socket servidor e porta
    socklen_t clilen; // tam da estrutura do cliente
    struct sockaddr_in serv_addr, cli_addr; // endereço IP e porta do servidor e do cliente
    char buffer[1023];
    char *filename = "arquivo_binario_10mb.bin"; // Nome do arquivo a ser enviado
    int filefd; 
    ssize_t bytes_read;
    int n;
    size_t packets_sent = 0; // quantidade de pacotes enviados

    if (argc < 2) { // verifica se a porta foi passada como argumento
        fprintf(stderr,"ERROR, no port provided\n");
        fprintf(stderr, "Use: %s <PORT>\n", argv[0]);
        exit(0);
    }

    // Criando o socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr)); // Zerando a estrutura serv_addr
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET; //IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); // converte o nº da porta

    // Associa o socket à porta e endereço especificados
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    clilen = sizeof(cli_addr);

    // Espera receber a solicitação de um cliente (nn precisa aceitar, apenas receber)
    n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cli_addr, &clilen);
    if (n < 0) 
        error("ERROR on recvfrom");    
    printf("Solicitação recebida.\n");

    // Abre o arquivo que será enviado
    char path[1023];
    sprintf(path, "%s/%s", ROOT, filename);
    filefd = open(path, O_RDONLY);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo");
        close(sockfd);
        exit(0);
    }

    long file_size = lseek(filefd, 0, SEEK_END); // Tamanho do arquivo
    lseek(filefd, 0, SEEK_SET); // Volta o ponteiro para o início do arquivo

    // numero de pacotes que serao enviados
    size_t packet_size = sizeof(buffer);
    size_t num_packets = (file_size + packet_size - 1) / packet_size; //arredonda para cima
    
    // Envia o número de pacotes para o cliente
    sprintf(buffer, "%zu", num_packets);
    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cli_addr, clilen);
    if (n < 0) {
        error("Erro ao enviar número de pacotes ao cliente");
    }
    printf("Número de pacotes esperados (%zu) enviado ao cliente.\n", num_packets);

    // Envia o arquivo em blocos de 1023 bytes
    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0) {
        n = sendto(sockfd, buffer, bytes_read, 0, (struct sockaddr *) &cli_addr, clilen);
        if (n < 0) {
            perror("ERROR writing to socket");
            close(filefd);
            close(sockfd);
            exit(1);
        }
        packets_sent++; // incrementa a quantidade de pacotes enviados
    }

    // Envia a mensagem "EOF" para indicar o término
    const char *eof_msg = "EOF";
    n = sendto(sockfd, eof_msg, strlen(eof_msg), 0, (struct sockaddr *) &cli_addr, clilen);
    if (n < 0) {
        perror("ERROR sending EOF");
    }

    if (bytes_read < 0) {
        perror("ERROR reading from file");
    }

    printf("Arquivo enviado com sucesso.\n");
    printf("Quantidade de pacotes enviados: %zu\n", packets_sent);

    close(filefd);
    close(sockfd);
    return 0;
}