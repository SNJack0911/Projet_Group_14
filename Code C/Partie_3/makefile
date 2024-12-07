CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS =

SRC_DIR = src
BIN_DIR = bin

EXEC= break_code

SRC = $(SRC_DIR)/break_code_c1.c $(SRC_DIR)/break_code_c2.c $(SRC_DIR)/break_code_c3.c $(SRC_DIR)/break_code.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_MAIN_PART_1))

ECHO = @

ifeq ($(VERBOSE),1)
	ECHO =
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
else
	CFLAGS += -O3 -DNDEBUG
endif

all: $(EXEC)

$(EXEC): $(OBJ)
	$(ECHO)$(CC) -o $(EXEC) $(OBJ) $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

$(BIN_DIR):
	$(ECHO)mkdir -p $(BIN_DIR)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -f $(BIN_DIR)/*.o

mrproper: clean
	$(ECHO)rm -f $(EXEC) 

# Specific file dependencies
$(BIN_DIR)/break_code.o: $(SRC_DIR)/fonctions_annexes.h $(SRC_DIR)/break_code.c
