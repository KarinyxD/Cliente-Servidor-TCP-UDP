# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra 
BFLAGS = -lssl -lcrypto

# Regras de compilação
all: serverUDP clientUDP

serverUDP: serverUDP.c
	$(CC) $(CFLAGS) -o serverUDP serverUDP.c $(BFLAGS)

clientUDP: clientUDP.c
	$(CC) $(CFLAGS) -o clientUDP clientUDP.c $(BFLAGS)

# Limpeza dos arquivos compilados
clean:
	rm -f serverUDP clientUDP

# Limpeza completa, incluindo arquivos objeto
distclean: clean

.PHONY: all clean distclean