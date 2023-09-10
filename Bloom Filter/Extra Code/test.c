#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_WORD_LENGTH 100

// hash 1
int h1(const char *s, int arrSize) {
    long long int hash = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        hash = (hash + ((int)s[i])) % arrSize;
    }
    return (int)hash;
}

//hash 2
int h2(const char *s, int arrSize) {
    long long int hash = 1;
    for (int i = 0; s[i] != '\0'; i++) {
        hash = (hash + (long long int)(pow(19, i) * s[i])) % arrSize;
    }
    return (int)(hash % arrSize);
}

// hash 3
int h3(const char *s, int arrSize) {
    long long int hash = 7;
    for (int i = 0; s[i] != '\0'; i++) {
        hash = (hash * 31 + s[i]) % arrSize;
    }
    return (int)(hash % arrSize);
}

// hash 4
int h4(const char *s, int arrSize) {
    long long int hash = 3;
    int p = 7;
    for (int i = 0; s[i] != '\0'; i++) {
        hash += hash * 7 + s[0] * pow(p, i);
        hash = (int)(hash % arrSize);
    }
    return (int)hash;
}

// lookup operation
bool lookup(bool *bitarray, int arrSize, const char *s) {
    int a = h1(s, arrSize);
    int b = h2(s, arrSize);
    int c = h3(s, arrSize);
    int d = h4(s, arrSize);

    if (bitarray[a] && bitarray[b] && bitarray[c] && bitarray[d])
        return true;
    else
        return false;
}

// insert operation
void insert(bool *bitarray, int arrSize, const char *s) {
    // check if the element is already present or not
    if (lookup(bitarray, arrSize, s))
        printf("%s is Probably already present\n", s);
    else {
        int a = h1(s, arrSize);
        int b = h2(s, arrSize);
        int c = h3(s, arrSize);
        int d = h4(s, arrSize);

        bitarray[a] = true;
        bitarray[b] = true;
        bitarray[c] = true;
        bitarray[d] = true;

        printf("%s inserted\n", s);
    }
}

// Driver Code
int main() {
    bool bitarray[100] = { false }; // Initialize the Bloom filter
    int arrSize = 100;
    const char *sarray[33] = { "abound", "abounds", "abundance",
                                "abundant", "accessible", "bloom",
                                "blossom", "bolster", "bonny",
                                "bonus", "bonuses", "coherent",
                                "cohesive", "colorful", "comely",
                                "comfort", "gems", "generosity",
                                "generous", "generously", "genial",
                                "bluff", "cheater", "hate",
                                "war", "humanity", "racism",
                                "hurt", "nuke", "gloomy",
                                "facebook", "geeksforgeeks", "twitter" };

    for (int i = 0; i < 33; i++) {
        insert(bitarray, arrSize, sarray[i]);
    }

    return 0;
}
