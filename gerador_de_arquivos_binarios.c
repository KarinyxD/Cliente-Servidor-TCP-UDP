#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int file_size_mb;
    char file_name[256];
    char file_path[512];

    printf("Enter the desired file size in MB: ");
    scanf("%d", &file_size_mb);

    if (file_size_mb <= 0)
    {
        printf("Invalid file size.\n");
        return 1;
    }

    printf("Enter the file name: ");
    scanf("%s", file_name);

    // Construct the full file path
    snprintf(file_path, sizeof(file_path), "arquivos_servidor/%s.bin", file_name);

    size_t file_size = file_size_mb * 1024 * 1024; // Convert MB to bytes

    FILE *file = fopen(file_path, "wb");
    if (file == NULL)
    {
        perror("Error creating file");
        return 1;
    }

    // Fill the file with random data
    // Unsigned char has 4 bits, so it can store values from 0 to 255
    unsigned char byte = 0;
    for (size_t i = 0; i < file_size; i++)
    {
        fwrite(&byte, sizeof(byte), 1, file);
        byte++;
    }

    fclose(file);
    printf("Binary file of %dMB generated successfully.\n", file_size_mb);
    printf("File path: %s\n", file_path);
    return 0;
}