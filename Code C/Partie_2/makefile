CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS =

EXEC_MAIN_GEN = dh_gen_group

SRC_MAIN_GEN = $(wildcard *.c)

OBJ_MAIN_GEN = $(SRC_MAIN_GEN:.c=.o)

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

%.o: %.c
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -f *.o

mrproper: clean
	$(ECHO)rm -f $(EXEC_MAIN_GEN)

# Specific file dependencies
dh_gen_group.o: dh_gen_group.c dh_prime.c
