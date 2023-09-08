CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
REL_FLAGS = -O3
DBG_FLAGS = -g3 -fsanitize=address,undefined

REL = release/linux
OBJ = obj/linux
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
	mkdir -p release
	mkdir -p $@

$(OBJ):
	mkdir -p obj
	mkdir -p $@

main: $(REL_DEP) $(HEADERS) | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) $(REL_DEP) -o $@
	mkdir -p dots
	mkdir -p svgs

$(REL)/main.o: $(SRC)/main.c | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) -c $< -o $@

$(REL)/%.o: $(SRC)/%.c $(SRC)/%.h | $(REL)
	$(CC) $(CFLAGS) $(REL_FLAGS) -c $< -o $@

debug: $(DBG_DEP) $(HEADERS) | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(DBG_DEP) -o $@
	mkdir -p dots
	mkdir -p svgs

$(OBJ)/debug.o: $(SRC)/main.c | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) -c $< -o $@

clean:
	rm $(REL_DEP) main -rf

deepclean:
	rm $(REL_DEP) $(DBG_DEP) main debug -rf
