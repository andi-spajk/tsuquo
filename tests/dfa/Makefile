CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_dfa.o dfa.o nfa.o set.o)
HEADERS = $(addprefix $(SRC)/,common.h dfa.h nfa.h set.h)

.PHONY: all clean

all: test_dfa

$(OBJ):
	mkdir -p $@

test_dfa: $(DEP) $(UNITY_DEP) $(HEADERS)
	#mkdir -p dots
	#mkdir -p svgs
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_dfa.o: test_dfa.c | $(OBJ)
	$(CC) $(CFLAGS) -c $<
	mv test_dfa.o $(OBJ)

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $<
	mv unity.o $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $<
	mv *.o $(OBJ)

clean:
	rm $(DEP) $(UNITY_DEP) test_dfa -rf
