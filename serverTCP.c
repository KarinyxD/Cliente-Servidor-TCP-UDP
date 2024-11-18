#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

char *ROOT="arquivos"; // diretorio dos arquivos

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
		char line[2047]; // Buffer de dados recebidos do cliente
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
				exit(0);
		}

        // Criando o socket TCP
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
				error("ERROR opening socket");
    
		bzero((char *) &serv_addr, sizeof(serv_addr)); // Zerando a estrutura serv_addr
		portno = atoi(argv[1]);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);

        // Associa o socket à porta e endereço especificados
		if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
				error("ERROR on binding");

		
        // Coloca o servidor em modo de escuta
		listen(sockfd,4);
		clilen = sizeof(cli_addr);

        // O servidor aceita a conexão de um cliente(bloqueia até que um cliente se conecte)
        // Quando a conexão é estabelecida, um novo socket (newsockfd) é criado para se comunicar com o cliente
	    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) error("ERROR on accept");

		printf("Conexão estabelecida com o cliente. Enviando o arquivo...\n");	


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
    return -1;    
}