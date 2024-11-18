#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/evp.h>

char *ROOT="arquivos_servidor"; // diretorio dos arquivos

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//funcao para calcular o hash MD5 de um arquivo
void calculate_md5(const char *filename, unsigned char *result) {
    int filefd = open(filename, O_RDONLY);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo para calcular MD5");
        exit(1);
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Erro ao criar contexto MD5");
        exit(1);
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        perror("Erro ao inicializar MD5");
        exit(1);
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            perror("Erro ao atualizar MD5");
            exit(1);
        }
    }

    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, result, &md_len) != 1) {
        perror("Erro ao finalizar MD5");
        exit(1);
    }

    EVP_MD_CTX_free(mdctx);
    close(filefd);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno; // socket servidor, socket cliente(especifico), porta
    socklen_t clilen; // tamanho da estrutura do endereço do cliente
    struct sockaddr_in serv_addr, cli_addr; // endereços IP do servidor e do cliente
    char buffer[1023]; // Buffer de dados para enviar e receber
    char *filename = "arquivo_binario_10mb.bin"; // Nome do arquivo a ser enviado
    int filefd; 
    ssize_t bytes_read;
    int n;

    if (argc < 2) { // verifica se a porta foi passada como argumento
        fprintf(stderr,"ERROR, no port provided\n");
        fprintf(stderr, "Use: %s <PORT>\n", argv[0]);
        exit(0);
    }

    // Criando o socket TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr)); // Zerando a estrutura serv_addr
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Associa o socket à porta e endereço especificados
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    // Coloca o servidor em modo de escuta
    listen(sockfd, 4);
    clilen = sizeof(cli_addr);

    // Aceita a conexão de um cliente
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    // Abre o arquivo que será enviado
    char path[1023];
    sprintf(path, "%s/%s", ROOT, filename);
    filefd = open(path, O_RDONLY);
    if (filefd < 0) {
        perror("Erro ao abrir o arquivo");
        close(newsockfd);
        close(sockfd);
        exit(0);
    }

    // Calcula o hash MD5 do arquivo
    unsigned char md5_result[EVP_MAX_MD_SIZE];
    calculate_md5(path, md5_result);

    // Envia o hash MD5 para o cliente
    n = write(newsockfd, md5_result, EVP_MD_size(EVP_md5()));
    if (n < 0) {
        perror("ERROR writing MD5 to socket");
        close(filefd);
        close(newsockfd);
        close(sockfd);
        exit(1);
    }

    // Envia o arquivo em blocos de 1023 bytes
    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0) {
        n = write(newsockfd, buffer, bytes_read);
        if (n < 0) {
            perror("ERROR writing to socket");
            close(filefd);
            close(newsockfd);
            close(sockfd);
            exit(1);
        }
    }

    if (bytes_read < 0) {
        perror("ERROR reading from file");
    }

    printf("Arquivo enviado com sucesso.\n");

    close(filefd);
    close(newsockfd);
    close(sockfd);
    return 0;
}