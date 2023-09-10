#include "main.h"

// Main code for the MAKEFILE that calls both the serial and parallel versions of the Bloom Filter

int main() {
    seqBF();
    parBF();

    return 0;
}