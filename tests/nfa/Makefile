CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj/linux
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_nfa.o nfa.o set.o)
HEADERS = $(addprefix $(SRC)/,common.h nfa.h set.h)

.PHONY: all clean

all: test_nfa

$(OBJ):
	mkdir -p $@

test_nfa: $(DEP) $(UNITY_DEP) $(HEADERS)
	mkdir -p dots
	mkdir -p svgs
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_nfa.o: test_nfa.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DEP) $(UNITY_DEP) test_nfa -rf
	rm dots/* -rf
	rm svgs/* -rf
