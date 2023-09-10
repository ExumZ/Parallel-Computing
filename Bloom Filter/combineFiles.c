#include <stdio.h>
#include <stdlib.h>

int main() {
    // File pointers for input files and output file
    FILE *file1, *file2, *file3, *outputFile;
    
    // Open the input files for reading
    file1 = fopen("MOBY_DICK.txt", "r");
    file2 = fopen("LITTLE_WOMEN.txt", "r");
    file3 = fopen("SHAKESPEARE.txt", "r");
    
    // Check if any file failed to open
    if (file1 == NULL || file2 == NULL || file3 == NULL) {
        perror("Error opening input files");
        return 1;
    }
    
    // Open the output file for writing
    outputFile = fopen("combined.txt", "w");
    
    // Check if the output file failed to open
    if (outputFile == NULL) {
        perror("Error opening output file");
        return 1;
    }
    
    int c;

    // Copy content from the first file to the output file
    while ((c = fgetc(file1)) != EOF) {
        fputc(c, outputFile);
    }

    // Copy content from the second file to the output file
    while ((c = fgetc(file2)) != EOF) {
        fputc(c, outputFile);
    }

    // Copy content from the third file to the output file
    while ((c = fgetc(file3)) != EOF) {
        fputc(c, outputFile);
    }

    // Close all open files
    fclose(file1);
    fclose(file2);
    fclose(file3);
    fclose(outputFile);

    printf("Files combined successfully.\n");

    return 0;
}