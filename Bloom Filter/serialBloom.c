#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include "main.h"

#define MAX_WORD_LENGTH 100
#define NUMBER_OF_FILES 3

int wordsInserted = 0;

int hash1(const char *s, int arrSize);

int hash2(const char *s, int arrSize);

int hash3(const char *s, int arrSize);

int hash4(const char *s, int arrSize);

// Lookup
bool lookup(bool *bitarray, int arrSize, char s[]);

// Insert
void insert(bool *bitarray, int arrSize, char **words, int numWords);

// Function to check if a word is already in the list
int checkDuplicateWords(char *word, char **resultList, int resultListLength);

// Function to read from file to array
int readFromFileToArray(const char *fileName, int fileLength, char ***pppArray);

// Check Bloom Filter using Query List
void processQueryFile(bool *bitarray, int arrSize, const char *queryFileName);

// Function for counting the number of rows in a file
int countRowsInFile(const char *filename);


// Driver Code
int main()
{
    printf("************************************************************************\n");
    printf("\n*                         Serial Bloom Filter                          *\n");
    printf("\n************************************************************************\n");
    // Array of file names to read words from
    // You can comment/uncommented these at your own accord
    char *fileNames[NUMBER_OF_FILES] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
    int fileLengths[NUMBER_OF_FILES] = {215724,195467,965465};

    // char *fileNames[NUMBER_OF_FILES] = {"LITTLE_WOMEN.txt"};
    // int fileLengths[NUMBER_OF_FILES] = {195467};

    // char *fileNames[NUMBER_OF_FILES] = {"MOBY_DICK.txt"};
    // int fileLengths[NUMBER_OF_FILES] = {215724};

    // char *fileNames[NUMBER_OF_FILES] = {"SHAKESPEARE.txt"};
    // int fileLengths[NUMBER_OF_FILES] = {965465};

    // Query files
    char *queryFileName = "query.txt";

    // char *queryFileName = "Little_Women_Query.txt";

    // char *queryFileName = "Moby_Dick_Query.txt";

    // char *queryFileName = "Shakespeare_Query.txt";

    char** ppWordListArray[NUMBER_OF_FILES] = {0};
	int wordListLengthArray[NUMBER_OF_FILES] = {0};
	
	struct timespec start, end, startComp, endComp, startQ, endQ, startC, endC; 
	double totalProcessTime; 
	int n = 0;
    int i;

	// Get the overall current clock time
	clock_gettime(CLOCK_MONOTONIC, &start); 

    // Get the counting words current clock time
	clock_gettime(CLOCK_MONOTONIC, &startC); 
	
	for (i = 0; i < NUMBER_OF_FILES; i++){
		wordListLengthArray[i] = readFromFileToArray(fileNames[i], fileLengths[i], &ppWordListArray[i]);
		n += wordListLengthArray[i];
	}

    // Get the counting words current clock time
	clock_gettime(CLOCK_MONOTONIC, &endC); 

    // Subtract end from start to get the CPU time used for counting unique words
	totalProcessTime = (endC.tv_sec - startC.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (endC.tv_nsec - startC.tv_nsec)) * 1e-9; 
	printf("Total estimated unique words from the files: %d. Counting time only(s): %lf seconds\n", n, totalProcessTime);

    // Bit-array size = (-n*ln[Desired P])/(ln2)^2
    int m = -( (n*log(0.05)) / pow(log(2), 2));

    // Number of hashes = (m/n)*ln2
    int k = (m/n)*log(2);

    // // Probability of FP = (1 - (1 - (1/m)^kn))^k
    // float p = pow((1 - (1 - pow((1/m),k*n ) )), k);

    printf("Number of unique words: %d\nRecommended array size: %d\nRecommended number of hashes: %d\n", n, m, k);

    // Initialize the Bloom filter
    bool *bitarray = (bool *)malloc(m * sizeof(bool));

    // Initialize the dynamic array with false
    for (i = 0; i < m; i++) {
        bitarray[i] = false;
    }

    int arrSize = m;

    // Get insertion current clock time
	clock_gettime(CLOCK_MONOTONIC, &startComp); 

    // Insert the words
    for (i = 0; i < NUMBER_OF_FILES; i++) {
        insert(bitarray, arrSize, ppWordListArray[i], wordListLengthArray[i]);
    }
    // Get the insertion current clock time
    clock_gettime(CLOCK_MONOTONIC, &endComp); 

	// Subtract end from start to get the CPU time used for inserting words
	totalProcessTime = (endComp.tv_sec - startComp.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9; 
	printf("Number of words inserted: %d. Computational time only(s): %lf seconds\n", wordsInserted, totalProcessTime);

    // Get query current clock time
	clock_gettime(CLOCK_MONOTONIC, &startQ); 

    // Process the query file
    processQueryFile(bitarray, arrSize, queryFileName);

    // Get the query current clock time
    clock_gettime(CLOCK_MONOTONIC, &endQ); 

	// Subtract end from start to get the CPU time used for inserting words
	totalProcessTime = (endQ.tv_sec - startQ.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (endQ.tv_nsec - startQ.tv_nsec)) * 1e-9; 
	printf("Query time only(s): %lf seconds\n", totalProcessTime);
  
 	// Get the ending clock time
	clock_gettime(CLOCK_MONOTONIC, &end); 
	
	// Subtract end from start to get the CPU time used for overall process
	totalProcessTime = (end.tv_sec - start.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Overall process time(s): %lf seconds\n\n", totalProcessTime);

    // Free the memory allocated to the duplicates to avoid memory leaks
    free(bitarray);
   
	for (i = 0; i < NUMBER_OF_FILES; i++){
		for (int j = 0; j < wordListLengthArray[i]; j++){
			free(ppWordListArray[i][j]);
		}
		free(ppWordListArray[i]);	
	}

    return 0;
}










/*****************************************
  
	         Helper Functions

*****************************************/

int hash1(const char *s, int arrSize){
    long long int hash = 0;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash + ((int)s[i]));
    }
    return abs(hash % arrSize);
}

int hash2(const char *s, int arrSize){
    long long int hash = 1;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash + (long long int)(pow(23, i) * s[i]));
    }
    return abs(hash % arrSize);
}

int hash3(const char *s, int arrSize){
    int hash = 7;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash * 37 + s[i]);
    }
    return abs(hash % arrSize);
}

int hash4(const char *s, int arrSize){
    int hash = 3;
    int p = 13;
    for (int i = 0; i < strlen(s); i++){
        hash += hash * 13 + s[0] * pow(p, i);
    }
    return abs(hash % arrSize);
}

bool lookup(bool *bitarray, int arrSize, char s[]){
    int a = hash1(s, arrSize);
    int b = hash2(s, arrSize);
    int c = hash3(s, arrSize);
    int d = hash4(s, arrSize);
  
    if (bitarray[a] && bitarray[b] && bitarray[c] && bitarray[d])
        return true;
    else
        return false;
}

void insert(bool *bitarray, int arrSize, char **words, int numWords) {
    for (int i = 0; i < numWords; i++) {
        char *word = words[i];
        // Check if the element is already present or not
        if (!lookup(bitarray, arrSize, word)){
            int a = hash1(word, arrSize);
            int b = hash2(word, arrSize);
            int c = hash3(word, arrSize);
            int d = hash4(word, arrSize);
            
            bitarray[a] = true;
            bitarray[b] = true;
            bitarray[c] = true;
            bitarray[d] = true;

            wordsInserted += 1;
        }
    }
}

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

void processQueryFile(bool *bitarray, int arrSize, const char *queryFileName) {
    // Open the file
    FILE *queryFile = fopen(queryFileName, "r");
    if (queryFile == NULL) {
        perror("Error opening query file");
        return;
    }

    // Count the number of rows in the query
    int queryLength = countRowsInFile("query.txt");

    // int queryLength = countRowsInFile("Little_Women_Query.txt");

    // int queryLength = countRowsInFile("Moby_Dick_Query.txt");

    // int queryLength = countRowsInFile("Shakespeare_Query.txt");

    char word[MAX_WORD_LENGTH];
    int truePositive = 0;
    int falseNegative = 0;
    int falsePositive = 0;
    int trueNegative = 0;

    bool result;
    int label;

    // #pragma omp parallel for private(result) reduction(+: truePositive, trueNegative, falseNegative, falsePositive) shared(bitarray, label, word)
    for (int i = 0; i < queryLength; i++) {
        // End the loops when it detects end of file
        if (fscanf(queryFile, "%s %d", word, &label) != EOF) {
            // Lookup
            result = lookup(bitarray, arrSize, word);

            if (result && label == 1) {
                truePositive++;
            } 
            else if (!result && label == 0) {
                trueNegative++;
            } 
            else if (!result && label == 1) {
                falseNegative++;
            } 
            else if (result && label == 0) {
                falsePositive++;
            }
        }
    }

    // Print final results
    printf("Total True Positives: %d\n", truePositive);
    printf("Total True Negatives: %d\n", trueNegative);
    printf("Total False Positives: %d\n", falsePositive);
    printf("Total False Negatives: %d\n", falseNegative);

    float fpr = (float)falsePositive / (float)(trueNegative + falsePositive);

    printf("False Positive Rate: %f\n", fpr);

    fclose(queryFile);
}

int countRowsInFile(const char *filename) {
    FILE *file;
    char ch;
    int lines = 0;

    // Attempt to open the file
    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 to indicate an error
    }

    // Count the lines in the file
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }

    // Close the file
    fclose(file);

    return lines;
}