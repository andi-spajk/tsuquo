CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj/linux
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_lexer.o lexer.o control.o)
HEADERS = $(addprefix $(SRC)/,common.h lexer.h control.h)

.PHONY: all clean

all: test_lexer

$(OBJ):
	mkdir -p $@

test_lexer: $(DEP) $(UNITY_DEP) $(HEADERS)
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_lexer.o: test_lexer.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DEP) $(UNITY_DEP) test_lexer -rf
