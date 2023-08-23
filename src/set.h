/** set.h

Module definition for generic sets.

*/

#ifndef SET_H
#define SET_H

#include <stdbool.h>

// set_insert() return codes
#define INSERT_SUCCESS    0
#define INSERT_DUPLICATE  1
#define INSERT_ERROR     -1

// a set is internally represented as a linked list of ptrs to the user's data
typedef struct Node {
	// ptr to an element in a set
	void *data;
/*
	The memory and lifespan of this data will always be the USER's
	responsibility, regardless of whether it's dynamically allocated or not.
	The Set will not take control of your allocations.
*/
	struct Node *next;
} Node;

typedef struct Set {
	// optional unique identifier for a set
	void *id;
	// element sorting function
	int (*compare)(const void *, const void *);
/*
	The compare function dicates how the elements will be sorted and it
	should compare 2 elements that go in the set. Inside the compare
	function, you will probably have to cast (const void *) to your desired
	data type.
	The return value must indicate the following:
		>0      element1 goes after element2
		=0      element1 equals element2
		<0      element1 goes before element2
*/

	// internal linked list
	Node *head;
	Node *tail;
	int size;
} Set;

Set *init_set(int (*compare)(const void *, const void *));
void destroy_set(Set *set);
Node *init_node(void *data);
void destroy_node(Node *node);

bool set_is_empty(Set *set);
int set_insert(Set *set, void *element);
void *set_find(Set *set, void *element);
void *set_decapitate(Set *set);
bool set_equals(Set *s1, Set *s2);
Set *set_union(Set *lhs, Set *rhs);

#endif