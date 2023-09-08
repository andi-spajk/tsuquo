CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -std=c11 -fsanitize=address,undefined

OBJ = ../../obj/linux
SRC = ../../src
CFLAGS += -I$(SRC)

UNITY_SRC = ../../unity
UNITY_DEP = $(OBJ)/unity.o
DEP = $(addprefix $(OBJ)/,test_set.o set.o)
HEADERS = $(addprefix $(SRC)/,set.h)

.PHONY: all clean

all: test_set

$(OBJ):
	mkdir -p ../../obj
	mkdir -p $@

test_set: $(DEP) $(UNITY_DEP) $(HEADERS)
	$(CC) $(CFLAGS) $(DEP) $(UNITY_DEP) -o $@

$(OBJ)/test_set.o: test_set.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DEP): $(UNITY_SRC)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(DEP) $(UNITY_DEP) test_set -rf
