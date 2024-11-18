# Nome dos executáveis
SERVER_EXEC = serverTCP
CLIENT_EXEC = clientTCP

# Nome dos arquivos fonte
SERVER_SRC = serverTCP.c
CLIENT_SRC = clientTCP.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -lssl -lcrypto

# Regras de compilação
all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SERVER_SRC)
    $(CC) $(CFLAGS) -o $@ $^

$(CLIENT_EXEC): $(CLIENT_SRC)
    $(CC) $(CFLAGS) -o $@ $^

# Limpeza dos arquivos compilados
clean:
    rm -f $(SERVER_EXEC) $(CLIENT_EXEC)

# Limpeza completa, incluindo arquivos objeto
distclean: clean

.PHONY: all clean distclean