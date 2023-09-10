#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <omp.h>

#define MAX_WORD_LENGTH 1000

// Helper function to check if a word is already in the list
int isWordInList(char *word, char **wordList, int numWords) {
    for (int i = 0; i < numWords; i++) {
        if (strcmp(word, wordList[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int ReadFromFileToArray(const char *filename, int fileLength, char ***pppArray)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file");
		return -1;
	}

	char word[MAX_WORD_LENGTH]; // stack array
	char **FileWordList = NULL;
	char **UniqueWordList = NULL;
	int UniqueWordListLength = 0;
	
	FileWordList = (char**)malloc(fileLength * sizeof(char *));
	for(int i = 0; i < fileLength; i++){
		fscanf(file, "%s", word);
		FileWordList[i] = strdup(word);
	}
	UniqueWordList = (char**)malloc(fileLength * sizeof(char *));
	fclose(file);
	
	#pragma omp parallel for schedule(dynamic)
	for(int i = 0; i < fileLength; i++){
		// Convert word to lowercase for case-insensitive comparison
		for (int j = 0; FileWordList[i][j]; j++) {
		    FileWordList[i][j] = tolower(FileWordList[i][j]);
		}
		// Check if word is unique
		if (!isWordInList(FileWordList[i], UniqueWordList, UniqueWordListLength)) {
		    #pragma omp critical
            {
                UniqueWordList[UniqueWordListLength] = strdup(FileWordList[i]);
                UniqueWordListLength++;
            }
		}
	}
	UniqueWordList = realloc(UniqueWordList, UniqueWordListLength * sizeof(char *));

	*pppArray = UniqueWordList;
	

	for (int i = 0; i < fileLength; i++) {
		free(FileWordList[i]);
	}
	free(FileWordList);	
	return UniqueWordListLength;
}

int main() {

	char* filenames[3] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
	int fileLengths[3] = {215724,195467,965465};

	char** ppWordListArray[3] = {0};
	int wordListLengthArray[3] = {0};

	int i;
	char writeFile[32] = {0};
	
	struct timespec start, end, startComp, endComp; 
	double time_taken; 
	int n = 0;

	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); 
	
	for (i = 0; i < 3; i++){
		wordListLengthArray[i] = ReadFromFileToArray(filenames[i], fileLengths[i], &ppWordListArray[i]);
		n += wordListLengthArray[i];
	}
  
 	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Total unique words from read files: %d. Process time(s): %lf\n", n, time_taken);
   
	// Clean up
	for (i=0; i<3; i++){
		for (int j = 0; j < wordListLengthArray[i]; j++) {
			free(ppWordListArray[i][j]);
		}
		free(ppWordListArray[i]);	
	}

    return 0;
}