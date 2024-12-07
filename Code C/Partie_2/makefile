CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS =

SRC_DIR = src
BIN_DIR = bin

EXEC_MAIN_GEN = dh_gen_group

SRC_MAIN_GEN = $(wildcard $(SRC_DIR)/*.c)

OBJ_MAIN_GEN = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_MAIN_GEN))

ECHO = @

ifeq ($(VERBOSE),1)
	ECHO =
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
else
	CFLAGS += -O3 -DNDEBUG
endif

all: $(EXEC_MAIN_GEN)

$(EXEC_MAIN_GEN): $(OBJ_MAIN_GEN)
	$(ECHO)$(CC) -o $(EXEC_MAIN_GEN) $(OBJ_MAIN_GEN) $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

$(BIN_DIR):
	$(ECHO)mkdir -p $(BIN_DIR)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -f $(BIN_DIR)/*.o

mrproper: clean
	$(ECHO)rm -f $(EXEC_MAIN_GEN)

# Specific file dependencies
$(BIN_DIR)/dh_gen_group.o: $(SRC_DIR)/dh_gen_group.c $(SRC_DIR)/dh_prime.c
