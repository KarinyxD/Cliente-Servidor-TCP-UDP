# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra 
BFLAGS = -lssl -lcrypto

# Regras de compilação
all: serverTCP clientTCP

serverTCP: serverTCP.c
	$(CC) $(CFLAGS) -o serverTCP serverTCP.c $(BFLAGS)

clientTCP: clientTCP.c
	$(CC) $(CFLAGS) -o clientTCP clientTCP.c $(BFLAGS)

# Limpeza dos arquivos compilados
clean:
	rm -f serverTCP clientTCP

# Limpeza completa, incluindo arquivos objeto
distclean: clean

.PHONY: all clean distclean