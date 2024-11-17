/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

char *ROOT="arquivos"; // diretorio dos arquivos

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{
		char line[2048]; // Buffer de dados recebidos do cliente
		int sockfd, newsockfd, portno; // socket servidor, socket cliente(especifico), porta
		socklen_t clilen; // tamanho da estrutura do endereço do cliente
		struct sockaddr_in serv_addr, cli_addr; // endereços IP do servidor e do cliente
		int n;

		if (argc < 2) { // verifica se a porta foi passada como argumento
				fprintf(stderr,"ERROR, no port provided\n");
				exit(1);
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
		if (bind(sockfd, (struct sockaddr *) &serv_addr,
								sizeof(serv_addr)) < 0) 
				error("ERROR on binding");

        // Coloca o servidor em modo de escuta
		listen(sockfd,5);
		clilen = sizeof(cli_addr);

		int count = 0;
        // O servidor aceita a conexão de um cliente(bloqueia até que um cliente se conecte)
        // Quando a conexão é estabelecida, um novo socket (newsockfd) é criado para se comunicar com o cliente
	    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) error("ERROR on accept");
		

        // Lê os dados enviados pelo cliente e responde com um HTML simples
		while(1) {
            n = read(newsockfd, &line[count], 1);

            if(count > 0) {

			    if(line[count-1] == '\r' && line[count] == '\n') {
				    line[count+1] = '\0';
					if(strlen(line) == 2 && line[0] == '\r' && line[1] == '\n') {
					char *response = "HTTP/1.1 200 OK\r\n"
					                 "Content-Length: 88\r\n"
                                     "Content-Type: text/html\r\n"
                                     "Connection: Closed\r\n"
                                     "\r\n"
                                     "<html>"
                                     "<body>"
                                     "<h1>Hello, World!</h1>"
                                     "</body>"
                                      "</html>";
                                                
					
					write(newsockfd, response, strlen(response));
					close(newsockfd);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		    if (newsockfd < 0) error("ERROR on accept");
	}

				count = 0;
				continue;
							
			}
		}
				count++;
				if (n < 0) error("ERROR reading from socket");
			}
		
		close(sockfd);
		return 0; 
}