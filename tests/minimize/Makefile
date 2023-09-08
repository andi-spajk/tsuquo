CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj/linux
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_minimize.o minimize.o dfa.o nfa.o set.o parser.o \
                          lexer.o control.o)
HEADERS = $(addprefix $(SRC)/,common.h minimize.h dfa.h nfa.h set.h parser.h \
                              lexer.h control.h)

.PHONY: all clean

all: test_minimize

$(OBJ):
	mkdir -p $@

test_minimize: $(DEP) $(UNITY_DEP) $(HEADERS)
	mkdir -p dots
	mkdir -p svgs
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_minimize.o: test_minimize.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DEP) $(UNITY_DEP) test_minimize -rf
	rm dots/* -rf
	rm svgs/* -rf
