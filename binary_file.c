#include <stdio.h>
#include <stdlib.h>

#define FILE_SIZE 10 * 1024 * 1024 // 10MB

int main() {
    FILE *file = fopen("arquivo_binario_10mb.bin", "wb");
    if (file == NULL) {
        perror("Erro ao criar arquivo");
        return 1;
    }

    // Preenche o arquivo com dados aleatórios
    unsigned char byte = 0;
    for (int i = 0; i < FILE_SIZE; i++) {
        fwrite(&byte, sizeof(byte), 1, file);
        byte++;
    }

    fclose(file);
    printf("Arquivo binário de 10MB gerado com sucesso.\n");
    return 0;
}
