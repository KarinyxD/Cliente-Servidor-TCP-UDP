# Cliente-Servidor UDP com Verificação de Integridade
Este projeto implementa um sistema de cliente-servidor UDP em C, onde o servidor envia um arquivo para o cliente e o cliente verifica a integridade dos dados recebidos usando o hash MD5.

## Estrutura do Projeto
- `serverUDP.c`: Implementação do servidor TCP.
- `clientUDP.c`: Implementação do cliente TCP.
- `Makefile`: Arquivo para compilar o servidor e o cliente.

## Funcionalidades
- **Servidor:**
  - Recebe mensagens de clientes e envia respostas.
  - Calcula a quantidade de pacotes do arquivo e a envia pro cliente.
  - Envia o arquivo especificado para o cliente.

- **Cliente:**
  - Envia uma solicitacao ao servidor.
  - Recebe a quantidade de pacotes que o servidor irá enviar e o arquivo.
  - Calcula a taxa de download.
  - Com base na quantidade de pacotes que o servidor enviou e os que foram recebidos,
    exibe a porcentagem de perca de pacotes.

## Compilação
Para compilar o servidor e o cliente, use o `Makefile` fornecido. Execute o seguinte comando no diretório raiz do projeto:
```sh
make
```

## Execução
- **Servidor:**
  - Para executar o servidor, use o seguinte comando:
```sh
./serverUDP <porta>
```

- **Cliente:**
  - Para executar o cliente, use o seguinte comando:
```sh
./clientUDP <IP do servidor> <porta>
```
No nosso caso, o ip do servidor local pode ser: '127.0.0.1' ou 'localhost'
