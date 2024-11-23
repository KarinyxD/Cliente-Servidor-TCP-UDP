# Root Makefile

# Subdirectories
TCP_DIR = TCP
UDP_DIR = UDP

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Binary generator source
BIN_GENERATOR_SRC = gerador_de_arquivos_binarios.c
BIN_GENERATOR = gerador

# Targets
all: tcp udp $(BIN_GENERATOR)

# Compile TCP programs
tcp:
	$(MAKE) -C $(TCP_DIR)

# Compile UDP programs
udp:
	$(MAKE) -C $(UDP_DIR)

# Compile binary generator
$(BIN_GENERATOR): $(BIN_GENERATOR_SRC)
	$(CC) $(CFLAGS) -o $@ $<

# Clean all compiled files
clean:
	$(MAKE) -C $(TCP_DIR) clean
	$(MAKE) -C $(UDP_DIR) clean
	rm -f $(BIN_GENERATOR)

# Clean everything, including potential leftovers
distclean: clean
	$(MAKE) -C $(TCP_DIR) distclean
	$(MAKE) -C $(UDP_DIR) distclean

.PHONY: all tcp udp clean distclean