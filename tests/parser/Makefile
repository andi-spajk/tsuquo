CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj/linux
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_parser.o parser.o control.o nfa.o set.o lexer.o)
HEADERS = $(addprefix $(SRC)/,common.h parser.h control.h nfa.h set.h lexer.h)

.PHONY: all clean

all: test_parser

$(OBJ):
	mkdir -p $@

test_parser: $(DEP) $(UNITY_DEP) $(HEADERS)
	mkdir -p dots
	mkdir -p svgs
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_parser.o: test_parser.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DEP) $(UNITY_DEP) test_parser -rf
	rm dots/* -rf
	rm svgs/* -rf