#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORD_LENGTH 100

// Function to check if a word is already in the list
int checkDuplicateWords(char *word, char **resultList, int resultListLength);

// Function to read from file to array
int readFromFileToArray(const char *fileName, char ***pppArray);

// Function to write unique words to a file
void writeUniqueWordsToFile(const char *fileName, char **uniqueWordList, int uniqueWordListLength);

int main() {

    char* fileName = "SHAKESPEARE.txt"; // Use only one file

    char** ppWordListArray = NULL;
    int wordListLength = 0;

    int i;
    char writeFile[32] = {0};

    struct timespec start, end;
    double totalProcessTime;
    int n = 0;

    // Get the starting clock time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Read from the single file
    wordListLength = readFromFileToArray(fileName, &ppWordListArray);
    n += wordListLength;

    // Get the ending clock time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Subtract end from start to get the CPU time used.
    totalProcessTime = (end.tv_sec - start.tv_sec) * 1e9;
    totalProcessTime = (totalProcessTime + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Total unique words from the file: %d. Process time(s): %lf\n", n, totalProcessTime);

    // Write unique words to a new file
    writeUniqueWordsToFile("Shakespeare_Query.txt", ppWordListArray, wordListLength);

    // Free the memory allocated to the duplicates to avoid memory leaks
    for (i = 0; i < wordListLength; i++){
        free(ppWordListArray[i]);
    }
    free(ppWordListArray);

    return n;
}

/*****************************************
  
	         Helper Functions

*****************************************/

int checkDuplicateWords(char *word, char **resultList, int resultListLength){
    for (int i = 0; i < resultListLength; i++) {
        // Lexicographical comparison
        if (strcmp(word, resultList[i]) == 0) {
            // Is Duplicate
            return 1;
        }
    }
    // Not Duplicate
    return 0;
}

int readFromFileToArray(const char *fileName, char ***pppArray){
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char word[MAX_WORD_LENGTH]; // stack array
    char **wordList = NULL;
    char **uniqueWordList = NULL;
    int uniqueWordListLength = 0;

    // Pointer to an array of char pointers that point to dynamically allocated strings
    //  ... malloc() allocates memory for that array of char pointers and assigns the address of the 
    //  ... first element to this pointer.
    // This creates an array that can hold 'fileLength' pointers
    wordList = (char**)malloc(1 * sizeof(char *)); // Start with a small size

    while (fscanf(file, "%s", word) != EOF) {
        // Allocates memory for the duplicated string, including the space for the string characters,
        //  ... and then copies the characters from the input string to the newly allocated memory
        //  ... before returning a pointer to the duplicated string.
        wordList = (char **)realloc(wordList, (uniqueWordListLength + 1) * sizeof(char *));
        wordList[uniqueWordListLength] = strdup(word);

        // Converts each word to lower case (Case-sensitive comparisons)
        for (int j = 0; wordList[uniqueWordListLength][j]; j++){
            wordList[uniqueWordListLength][j] = tolower(wordList[uniqueWordListLength][j]);
        }

        // Checks for duplicate words
        if (!checkDuplicateWords(wordList[uniqueWordListLength], uniqueWordList, uniqueWordListLength)){
            uniqueWordList = (char **)realloc(uniqueWordList, (uniqueWordListLength + 1) * sizeof(char *));
            uniqueWordList[uniqueWordListLength] = strdup(wordList[uniqueWordListLength]);
            uniqueWordListLength++;
        }
    }

    fclose(file);

    // Free the memory allocated to the duplicates to avoid memory leaks
    for (int i = 0; i < uniqueWordListLength; i++){
        free(wordList[i]);
    }
    free(wordList);

    *pppArray = uniqueWordList;
    return uniqueWordListLength;
}

void writeUniqueWordsToFile(const char *fileName, char **uniqueWordList, int uniqueWordListLength) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    for (int i = 0; i < uniqueWordListLength; i++) {
        fprintf(file, "%s 1\n", uniqueWordList[i]);
    }

    fclose(file);
}

