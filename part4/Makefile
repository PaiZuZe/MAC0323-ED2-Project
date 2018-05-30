# Guilherme Costa Vieira               9790930
# Gabriel Kazuyuki Isomura             9793673
# Victor Chiaradia Gramuglia Araujo    9793756

CC := gcc
RM := rm -f

CFLAGS := -Wall -std=c99 -pedantic -O2 -Wextra

BIN := parse_test
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

OUT := ./

.PHONY: all
all: ${BIN}

.PHONY: clean
clean:
	${RM} ${OUT}${BIN}
	${RM} ${OBJ}

$(BIN): ${OBJ}
	${CC} ${CFLAGS} $^ -o ${OUT}$@

$(OBJ): ${SRC}
	${CC} ${CFLAGS} -c $^

$(OUT):
	mkdir ${OUT}
