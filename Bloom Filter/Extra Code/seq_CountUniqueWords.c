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
int readFromFileToArray(const char *fileName, int fileLength, char ***pppArray);

int main() {

	char* fileNames[3] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
	int fileLengths[3] = {215724,195467,965465};

	char** ppWordListArray[3] = {0};
	int wordListLengthArray[3] = {0};

	int i;
	char writeFile[32] = {0};
	
	struct timespec start, end; 
	double totalProcessTime; 
	int n = 0;

	// Get the starting clock time
	clock_gettime(CLOCK_MONOTONIC, &start); 
	
	for (i = 0; i < 3; i++){
		wordListLengthArray[i] = readFromFileToArray(fileNames[i], fileLengths[i], &ppWordListArray[i]);
		n += wordListLengthArray[i];
	}
  
 	// Get the ending clock time
	clock_gettime(CLOCK_MONOTONIC, &end); 
	
	// Subtract end from start to get the CPU time used.
	totalProcessTime = (end.tv_sec - start.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Total unique words from the files: %d. Process time(s): %lf\n", n, totalProcessTime);
   
	// Free the memory allocated to the duplicates to avoid memory leaks
	for (i = 0; i < 3; i++){
		for (int j = 0; j < wordListLengthArray[i]; j++){
			free(ppWordListArray[i][j]);
		}
		free(ppWordListArray[i]);	
	}

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

int readFromFileToArray(const char *fileName, int fileLength, char ***pppArray){
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
	wordList = (char**)malloc(fileLength * sizeof(char *));

	for(int i = 0; i < fileLength; i++){
		fscanf(file, "%s", word);
		// Allocates memory for the duplicated string, including the space for the string characters,
		//  ... and then copies the characters from the input string to the newly allocated memory
		//  ... before returning a pointer to the duplicated string.
		wordList[i] = strdup(word);
	}

	// Pointer to an array of char pointers that point to dynamically allocated unique strings
	uniqueWordList = (char**)malloc(fileLength * sizeof(char *));
	
	for(int i = 0; i < fileLength; i++){
		// Converts each word to lower case (Case-sensitive comparisons)
		for (int j = 0; wordList[i][j]; j++){
		    wordList[i][j] = tolower(wordList[i][j]);
		}
		// Checks for duplicate words
		if (!checkDuplicateWords(wordList[i], uniqueWordList, uniqueWordListLength)){
			uniqueWordList[uniqueWordListLength] = strdup(wordList[i]);
			uniqueWordListLength++;
		}
	}

	// Resizes the previous allocated block of memory to the length of the uniqueWordList to save memory
	uniqueWordList = realloc(uniqueWordList, uniqueWordListLength * sizeof(char *));

	*pppArray = uniqueWordList;
	fclose(file);
	
	// Free the memory allocated to the duplicates to avoid memory leaks
	for (int i = 0; i < fileLength; i++){
		free(wordList[i]);
	}
	free(wordList);	
	return uniqueWordListLength;
}