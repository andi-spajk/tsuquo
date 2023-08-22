#ifndef SET_H
#define SET_H

#include <stdbool.h>

typedef struct Node {
	// ptr to an element in a set
	void *data;
	// the memory and lifespan of this data is always the USER's
	// responsibility

	struct Node *next;
} Node;

typedef struct Set {
	/* public */
	// optional unique identifier for a set
	void *id;
	int (*compare)(const void *, const void *);
	// The @compare function should compare 2 elements that go in the set.
	// The return value should indicate the following:
	//      >0      element1 goes after element2
	//      =0      element1 equals element2
	//      <0      element1 goes before element2

	/* private */
	Node *head;
	Node *tail;
	int size;
} Set;

Set *init_set(int (*compare)(const void *, const void *));
void destroy_set(Set *set);
Node *init_node(void *data);
void destroy_node(Node *node);
bool set_is_empty(Set *set);

#endif