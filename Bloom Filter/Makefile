# Compiler and compiler flags
CC = gcc
CFLAGS = -lm -pthread -fopenmp

# Executable name
TARGET = run

# Source files
SRC = main.c seq_BloomFilter.c par_BloomFilter.c 

# Object files
OBJ = $(SRC:.c=.o)

# Header files
HEADERS = 

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Linking step
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

# Compilation step for source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up intermediate object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)
