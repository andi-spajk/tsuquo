CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = obj
SRC = src

DEP = $(addprefix $(OBJ)/,main.o lexer.o)
HEADERS = $(addprefix $(SRC)/,common.h lexer.h)

.PHONY: all clean

all: main

$(OBJ):
	mkdir -p $@

main: $(DEP) $(HEADERS)
	$(CC) $(CFLAGS) $(DEP) -o $@

$(OBJ)/main.o: $(SRC)/main.c | $(OBJ)
	$(CC) $(CFLAGS) -c $<
	mv main.o $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $<
	mv *.o $(OBJ)

clean:
	rm $(DEP) main -rf
