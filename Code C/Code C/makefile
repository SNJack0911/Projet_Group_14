CC=gcc
CFLAGS=	-std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS=	

EXEC_MAIN_PART_1=sym_crypt 
EXEC_METHOD_1=function_xor

SRC_MAIN_PART_1=cbc_crypt_decrypt.c fonctions_annexes.c xor_chiff_dechiff.c sym_crypt.c
SRC_METHOD_1=xor_chiff_dechiff.c

OBJ_MAIN_PART_1= $(SRC_MAIN_PART_1:.c=.o)
OBJ_METHOD_1= $(SRC_METHOD_1:.c=.o)

ECHO = @

ifeq ($(VERBOSE),1)
	ECHO=
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
	LDFLAGS =
else
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS =
endif

all:
ifeq ($(DEBUG),yes)
	@echo "Debug mode"
else
	@echo "Release mode"
=======
	@echo "Generating in debug mode"
else
	@echo "Generating in release mode"

	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)  

$(EXEC_MAIN_PART_1):$(OBJ_MAIN_PART_1)
	$(ECHO)$(CC) -o $(EXEC_MAIN_PART_1) $(OBJ_MAIN_PART_1) $(LDFLAGS)

$(EXEC_METHOD_1):$(OBJ_METHOD_1)
	$(ECHO)$(CC) -o	$(EXEC_METHOD_1) $(OBJ_METHOD_1) $(LDFLAGS) 
 
.PHONY: clean mrproper

clean:
	$(ECHO)rm -f *.o

mrproper: clean
	$(ECHO)rm -f $(EXEC_MAIN_PART_1) $(EXEC_METHOD_1)

# Specific file dependencies
sym_crypt.o: fonctions_annexes.h sym_crypt.c
cbc_crypt_decrypt.o: fonctions_annexes.h cbc_crypt_decrypt.c
xor_chiff_dechiff.o: fonctions_annexes.h xor_chiff_dechiff.c
