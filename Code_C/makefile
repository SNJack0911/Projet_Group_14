# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Source and target
SRC = main.c
TARGET = crypto_sym

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lncurses

# Clean up
clean:
	rm -f $(TARGET)
