# Cliente-Servidor TCP com Verificação de Integridade
Este projeto implementa um sistema de cliente-servidor TCP em C, onde o servidor envia um arquivo para o cliente e o cliente verifica a integridade dos dados recebidos usando o hash MD5.

## Estrutura do Projeto
- `serverTCP.c`: Implementação do servidor TCP.
- `clientTCP.c`: Implementação do cliente TCP.
- `Makefile`: Arquivo para compilar o servidor e o cliente.

## Funcionalidades
- **Servidor:**
  - Aceita conexões de clientes.
  - Envia um arquivo especificado para o cliente.
  - Calcula o hash MD5 do arquivo e envia para o cliente.

- **Cliente:**
  - Conecta-se ao servidor.
  - Recebe o arquivo e o hash MD5 do servidor.
  - Calcula o hash MD5 do arquivo recebido.
  - Verifica a integridade dos dados comparando os hashes MD5.

## Compilação
Para compilar o servidor e o cliente, use o `Makefile` fornecido. Execute o seguinte comando no diretório do projeto:
```sh
make
```

## Execução
- **Servidor:**
  - Para executar o servidor, use o seguinte comando:
```sh
./serverTCP <porta>
```

- **Cliente:**
  - Para executar o cliente, use o seguinte comando:
```sh
./clientTCP <IP do servidor> <porta>
```
No nosso caso, o ip do servidor local pode ser: '127.0.0.1' ou 'localhost'