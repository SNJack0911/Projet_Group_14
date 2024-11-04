CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS =

EXEC_MAIN_PART_1 = sym_crypt
EXEC_METHOD_1 = function_xor
EXEC_METHOD_1_EXT = mask_jetable
EXEC_METHOD_2 = function_cbc

SRC_MAIN_PART_1 = cbc_crypt_decrypt.c xor_chiff_dechiff.c sym_crypt.c masque_jetable.c
SRC_METHOD_1 = xor_chiff_dechiff.c
SRC_METHOD_1_EXT = masque_jetable.c
SRC_METHOD_2 = cbc_crypt_decrypt.c

OBJ_MAIN_PART_1 = $(SRC_MAIN_PART_1:.c=.o)
OBJ_METHOD_1 = $(SRC_METHOD_1:.c=.o)
OBJ_METHOD_1_EXT = $(SRC_METHOD_1_EXT:.c=.o)
OBJ_METHOD_2 = $(SRC_METHOD_2:.c=.o)

ECHO = @

ifeq ($(VERBOSE),1)
	ECHO =
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
else
	CFLAGS += -O3 -DNDEBUG
endif

all: $(EXEC_MAIN_PART_1) $(EXEC_METHOD_1) $(EXEC_METHOD_2) $(EXEC_METHOD_1_EXT)
partie_1: $(EXEC_MAIN_PART_1)
method_1: $(EXEC_METHOD_1)
method_1_ext: $(EXEC_METHOD_1_EXT)
method_2: $(EXEC_METHOD_2)

$(EXEC_MAIN_PART_1): $(OBJ_MAIN_PART_1)
	$(ECHO)$(CC) -o $(EXEC_MAIN_PART_1) $(OBJ_MAIN_PART_1) $(LDFLAGS)

$(EXEC_METHOD_1): $(OBJ_METHOD_1)
	$(ECHO)$(CC) -o $(EXEC_METHOD_1) $(OBJ_METHOD_1) $(LDFLAGS)

$(EXEC_METHOD_1_EXT): $(OBJ_METHOD_1_EXT) xor_chiff_dechiff.o
	$(ECHO)$(CC) -o $(EXEC_METHOD_1_EXT) $(OBJ_METHOD_1_EXT) xor_chiff_dechiff.o $(LDFLAGS)

$(EXEC_METHOD_2): $(OBJ_METHOD_2)
	$(ECHO)$(CC) -o $(EXEC_METHOD_2) $(OBJ_METHOD_2) $(LDFLAGS)

%.o: %.c
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -f *.o

mrproper: clean
	$(ECHO)rm -f $(EXEC_MAIN_PART_1) $(EXEC_METHOD_1) $(EXEC_METHOD_1_EXT) $(EXEC_METHOD_2)

# Specific file dependencies
sym_crypt.o: fonctions_annexes.h sym_crypt.c
cbc_crypt_decrypt.o: fonctions_annexes.h cbc_crypt_decrypt.c
xor_chiff_dechiff.o: fonctions_annexes.h xor_chiff_dechiff.c
masque_jetable.o: fonctions_annexes.h masque_jetable.c
