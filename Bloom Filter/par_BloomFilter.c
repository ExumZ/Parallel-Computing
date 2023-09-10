#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <omp.h>
#include <pthread.h>
#include "main.h"

#define MAX_WORD_LENGTH 100
#define MAX_FPR 0.05
#define NUMBER_OF_FILES 3
#define NUM_THREADS 8

// Global parameters

// You can comment/uncommented these at your own accord
char *fileNames[NUMBER_OF_FILES] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
int fileLengths[NUMBER_OF_FILES] = {215724,195467,965465};

// char *fileNames[NUMBER_OF_FILES] = {"LITTLE_WOMEN.txt"};
// int fileLengths[NUMBER_OF_FILES] = {195467};

// char *fileNames[NUMBER_OF_FILES] = {"MOBY_DICK.txt"};
// int fileLengths[NUMBER_OF_FILES] = {215724};

// char *fileNames[NUMBER_OF_FILES] = {"SHAKESPEARE.txt"};
// int fileLengths[NUMBER_OF_FILES] = {965465};

//Query files
char *queryFileName = "query.txt";

// char *queryFileName = "Little_Women_Query.txt";

// char *queryFileName = "Moby_Dick_Query.txt";

// char *queryFileName = "Shakespeare_Query.txt";

char** ppWordListArray[NUMBER_OF_FILES] = {0};
int wordListLengthArray[NUMBER_OF_FILES] = {0};

int par_wordsInserted = 0;

// Calculation told me to use 4 hash functions
int par_hash1(const char *s, int arrSize);

int par_hash2(const char *s, int arrSize);

int par_hash3(const char *s, int arrSize);

int par_hash4(const char *s, int arrSize);

// Lookup
bool par_lookup(bool *bitarray, int arrSize, char s[]);

// Insert
void par_insert(bool *bitarray, int arrSize, char **words, int numWords);

// Function to check if a word is already in the list
int par_checkDuplicateWords(char *word, char **resultList, int resultListLength);

// Function to read from file to array
int par_readFromFileToArray(const char *fileName, int fileLength, char ***pppArray);

// Check Bloom Filter using Query List
void par_processQueryFile(bool *bitarray, int arrSize, const char *queryFileName);

// Count the number of rows in a file
int par_countRowsInFile(const char *filename);

// Parallelized version of the function above
//  - Shows which thread is responsible for each file
void *readThread(void *pArg);

// Driver Code
int parBF()
{
    printf("************************************************************************\n");
    printf("\n*                        Parallel Bloom Filter                         *\n");
    printf("\n************************************************************************\n");
	struct timespec start, end, startComp, endComp, startQ, endQ, startC, endC; 
	double totalProcessTime; 

    pthread_t threadId[NUMBER_OF_FILES];
    int threadNum[NUMBER_OF_FILES];
	int n = 0;
    int i;

	// Get the overall current clock time
	clock_gettime(CLOCK_MONOTONIC, &start); 

    // Get the counting words current clock time
	clock_gettime(CLOCK_MONOTONIC, &startC); 
	
	// Fork the different files into separate threads for faster reading
    for (i = 0; i < NUMBER_OF_FILES; i++){
        threadNum[i] = i;
        pthread_create(&threadId[i], 0, readThread, &threadNum[i]);
    }

    // Join back the results from each thread
    for (i = 0; i < NUMBER_OF_FILES; i++){
        pthread_join(threadId[i], NULL);
    }

    for (i = 0; i < NUMBER_OF_FILES; i++){
        n += wordListLengthArray[i];
    }

    // Get the counting words current clock time
    clock_gettime(CLOCK_MONOTONIC, &endC); 

	// Subtract end from start to get the CPU time used for counting unique words
	totalProcessTime = (endC.tv_sec - startC.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (endC.tv_nsec - startC.tv_nsec)) * 1e-9; 
	printf("Total estimated unique words from the files: %d. Counting time only(s): %lf seconds\n", n, totalProcessTime);

    // Bit-array size = (-n*ln[Desired P])/(ln2)^2
    int m = -( (n*log(MAX_FPR)) / pow(log(2), 2));

    // Number of hashes = (m/n)*ln2
    int k = (m/n)*log(2);

    // Probability of FP = (1 - (1 - (1/m)^kn))^k
    // float p = (float)(pow((1 - (1 - pow((1/(float)m),(float)k*(float)n ) )), (float)k));

    printf("Number of unique words: %d\nRecommended array size: %d\nRecommended number of hashes: %d\n", n, m, k);

    // Initialize the Bit-array
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
        par_insert(bitarray, arrSize, ppWordListArray[i], wordListLengthArray[i]);
    }

    // Get the insertion current clock time
    clock_gettime(CLOCK_MONOTONIC, &endComp); 

	// Subtract end from start to get the CPU time used for querying words
	totalProcessTime = (endComp.tv_sec - startComp.tv_sec) * 1e9; 
    totalProcessTime = (totalProcessTime + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9; 
	printf("Number of words inserted: %d. Insertion time only(s): %lf seconds\n", par_wordsInserted, totalProcessTime);

    // Get query current clock time
	clock_gettime(CLOCK_MONOTONIC, &startQ); 

    // Process the query file
    par_processQueryFile(bitarray, arrSize, queryFileName);

    // Get and query current clock time
    clock_gettime(CLOCK_MONOTONIC, &endQ); 

	// Subtract end from start to get the CPU time used for querying words
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

int par_hash1(const char *s, int arrSize){
    long long int hash = 0;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash + ((int)s[i]));
    }
    return abs(hash % arrSize);
}

int par_hash2(const char *s, int arrSize){
    long long int hash = 1;
    int p = 23;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash + (pow(p, i) * s[i]));
    }
    return abs(hash % arrSize);
}

int par_hash3(const char *s, int arrSize){
    long long int hash = 7;
    int p = 37;
    for (int i = 0; i < strlen(s); i++){
        hash = (hash * p + s[i]);
    }
    return abs(hash % arrSize);
}

int par_hash4(const char *s, int arrSize){
    long long int hash = 3;
    int p = 13;
    for (int i = 0; i < strlen(s); i++){
        hash += hash * p + s[0] * pow(p, i);
    }
    return abs(hash % arrSize);
}

bool par_lookup(bool *bitarray, int arrSize, char s[]){
    int a = par_hash1(s, arrSize);
    int b = par_hash2(s, arrSize);
    int c = par_hash3(s, arrSize);
    int d = par_hash4(s, arrSize);
  
    return (bitarray[a] && bitarray[b] && bitarray[c] && bitarray[d]);
}

void par_insert(bool *bitarray, int arrSize, char **words, int numWords) {
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < numWords; i++) {
        // Get the pointer to the current word in the array of words for processing
        char *word = words[i];
        
        // Check if the element is already present or not
        if (!par_lookup(bitarray, arrSize, word)){
            int a = par_hash1(word, arrSize);
            int b = par_hash2(word, arrSize);
            int c = par_hash3(word, arrSize);
            int d = par_hash4(word, arrSize);

            #pragma omp critical
            {
                bitarray[a] = true;
                bitarray[b] = true;
                bitarray[c] = true;
                bitarray[d] = true;

                par_wordsInserted += 1;
            }
        }
    }
}

int par_checkDuplicateWords(char *word, char **resultList, int resultListLength){
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

int par_readFromFileToArray(const char *fileName, int fileLength, char ***pppArray){
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("Error opening file");
		return -1;
	}

	char word[MAX_WORD_LENGTH];
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
	
    // Actual unique words: 71846
	// I opted for guided scheduling since it gave me the most accurate result on average with the lowest average process time
	// Result: 71870 <= x <= 71900
	// Processing time: 3.9s <= t <= 4.5s
    #pragma omp parallel for schedule(guided) num_threads(NUM_THREADS)
	for(int i = 0; i < fileLength; i++){
		// Converts each word to lower case (Case-sensitive comparisons)
		for (int j = 0; wordList[i][j]; j++){
		    wordList[i][j] = tolower(wordList[i][j]);
		}
		// Checks for duplicate words
		if (!par_checkDuplicateWords(wordList[i], uniqueWordList, uniqueWordListLength)){
			#pragma omp critical
            {
                uniqueWordList[uniqueWordListLength] = strdup(wordList[i]);
                uniqueWordListLength++;
            }
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

void par_processQueryFile(bool *bitarray, int arrSize, const char *queryFileName) {
    // Open the file
    FILE *queryFile = fopen(queryFileName, "r");
    if (queryFile == NULL) {
        perror("Error opening query file");
        return;
    }

    // Count the number of rows in the query
    int queryLength = par_countRowsInFile("query.txt");

    // int queryLength = par_countRowsInFile("Little_Women_Query.txt");

    // int queryLength = par_countRowsInFile("Moby_Dick_Query.txt");

    // int queryLength = par_countRowsInFile("Shakespeare_Query.txt");

    char word[MAX_WORD_LENGTH];
    int truePositive = 0;
    int falseNegative = 0;
    int falsePositive = 0;
    int trueNegative = 0;

    bool result;
    int label;
    
    // For the word in each row...
    for (int i = 0; i < queryLength; i++) {
        // End the loop when it detects the end of the file
        if (fscanf(queryFile, "%s %d", word, &label) != EOF) {
            // Lookup
            result = par_lookup(bitarray, arrSize, word);

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

int par_countRowsInFile(const char *filename) {
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

void *readThread(void *pArg){
    int my_rank = *((int*)pArg);
    wordListLengthArray[my_rank] = par_readFromFileToArray(fileNames[my_rank], fileLengths[my_rank], &ppWordListArray[my_rank]);
    return NULL;
}