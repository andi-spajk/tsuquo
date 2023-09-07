CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
REL_FLAGS = -O3
DBG_FLAGS = -g3 -fsanitize=address,undefined

REL = release
OBJ = obj
SRC = src

DBG_DEP = $(addprefix $(OBJ)/,debug.o control.o dfa.o lexer.o minimize.o nfa.o \
                              parser.o set.o)
REL_DEP = $(addprefix $(REL)/,main.o control.o dfa.o lexer.o minimize.o nfa.o  \
                              parser.o set.o)
HEADERS = $(addprefix $(SRC)/,common.h control.h dfa.h lexer.h minimize.h nfa.h\
                              parser.h set.h)

.PHONY: all clean deepclean

all: main

$(REL):
	mkdir -p $@

$(OBJ):
	mkdir -p $@

main: $(REL_DEP) $(HEADERS) | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) $(REL_DEP) -o $@

$(REL)/main.o: $(SRC)/main.c | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) -c $<
	mv main.o $(REL)

$(REL)/%.o: $(SRC)/%.c $(SRC)/%.h | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) -c $<
	mv *.o $(REL)

debug: $(DBG_DEP) $(HEADERS) | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(DBG_DEP) -o $@

$(OBJ)/debug.o: $(SRC)/main.c | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) -c $< -o debug.o
	mv debug.o $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) -c $<
	mv *.o $(OBJ)

clean:
	rm $(REL_DEP) main -rf

deepclean:
	rm $(REL_DEP) $(DBG_DEP) main -rf
