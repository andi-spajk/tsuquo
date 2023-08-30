CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

REL = release
OBJ = obj
SRC = src

DBG_DEP = $(addprefix $(OBJ)/,debug.o control.o dfa.o lexer.o nfa.o parser.o set.o)
REL_DEP = $(addprefix $(REL)/,main.o control.o dfa.o lexer.o nfa.o parser.o set.o)
HEADERS = $(addprefix $(SRC)/,common.h control.h dfa.h lexer.h nfa.h parser.h set.h)

.PHONY: all clean deepclean

all: main

$(REL):
	mkdir -p $@

$(OBJ):
	mkdir -p $@

main: $(REL_DEP) $(HEADERS) | $(REL)
	$(CC) $(CFLAGS) -O3 $(REL_DEP) -o $@

$(REL)/main.o: $(SRC)/main.c | $(REL)
	$(CC) $(CFLAGS) -O3 -c $<
	mv main.o $(REL)

$(REL)/%.o: $(SRC)/%.c $(SRC)/%.h | $(REL)
	$(CC) $(CFLAGS) -O3 -c $<
	mv *.o $(REL)

debug: $(DBG_DEP) $(HEADERS) | $(OBJ)
	$(CC) $(CFLAGS) -g3 -fsanitize=address,undefined $(DBG_DEP) -o $@

$(OBJ)/debug.o: $(SRC)/main.c | $(OBJ)
	$(CC) $(CFLAGS) -g3 -fsanitize=address,undefined -c $< -o debug.o
	mv debug.o $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -g3 -fsanitize=address,undefined -c $<
	mv *.o $(OBJ)

clean:
	rm $(REL_DEP) main -rf

deepclean:
	rm $(REL_DEP) $(DBG_DEP) main -rf
