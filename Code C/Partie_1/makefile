CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS =

SRC_DIR = src
BIN_DIR = bin

EXEC_MAIN_PART_1 = sym_crypt

SRC_MAIN_PART_1 = $(SRC_DIR)/sym_crypt.c $(SRC_DIR)/xor_chiff_dechiff.c $(SRC_DIR)/masque_jetable.c $(SRC_DIR)/cbc_crypt_decrypt.c

OBJ_MAIN_PART_1 = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_MAIN_PART_1))

ECHO = @

ifeq ($(VERBOSE),1)
	ECHO =
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
else
	CFLAGS += -O3 -DNDEBUG
endif

all: $(EXEC_MAIN_PART_1)

$(EXEC_MAIN_PART_1): $(OBJ_MAIN_PART_1)
	$(ECHO)$(CC) -o $(EXEC_MAIN_PART_1) $(OBJ_MAIN_PART_1) $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

$(BIN_DIR):
	$(ECHO)mkdir -p $(BIN_DIR)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -f $(BIN_DIR)/*.o

mrproper: clean
	$(ECHO)rm -f $(EXEC_MAIN_PART_1) 

# Specific file dependencies
$(BIN_DIR)/sym_crypt.o: $(SRC_DIR)/fonctions_annexes.h $(SRC_DIR)/sym_crypt.c
