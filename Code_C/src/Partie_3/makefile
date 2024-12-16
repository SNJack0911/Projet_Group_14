CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS = -lm

SRC_DIR = src
BIN_DIR = bin

EXEC= break_code

SRC = $(SRC_DIR)/break_code_c1.c $(SRC_DIR)/break_code_c2.c $(SRC_DIR)/break_code_c3.c $(SRC_DIR)/break_code_main.c $(SRC_DIR)/ordonered_list.c $(SRC_DIR)/break_head.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC))

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
$(BIN_DIR)/break_code_main.o: $(SRC_DIR)/break_head.h $(SRC_DIR)/break_code_main.c
$(BIN_DIR)/break_code_c3.o: $(SRC_DIR)/ordonered_list.h $(SRC_DIR)/ordonered_list.c $(SRC_DIR)/break_head.h $(SRC_DIR)/break_code_main.c
$(BIN_DIR)/break_code_c1.o: $(SRC_DIR)/break_head.h $(SRC_DIR)/break_head.c
