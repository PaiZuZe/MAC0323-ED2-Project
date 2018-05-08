CC := gcc
RM := rm -f

CFLAGS := -Wall -std=c99 -pedantic -O2 -Wextra

BIN := freq
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

OUT := ./

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
        $(RM) $(OUT)$(BIN)
        $(RM) $(OBJ)

$(BIN): $(OBJ)
        $(CC) $(CFLAGS) $^ -o $(OUT)$@

$(OBJ): $(SRC)
        $(CC) $(CFLAGS) -c $^

$(OUT):
        mkdir $(OUT)
