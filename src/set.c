/** set.c

Sorted set. Internally represented as a linked list.

*/

#include <stdbool.h>
#include <stdlib.h>

#include "set.h"

/* init_set()
	@compare        function ptr to a compare function

	@return         ptr to dynamically allocated Set, NULL if fail

	Dynamically allocate and initialize a Set struct and its members.

	The @compare function should compare 2 elements that go in the set.
	The return value should indicate the following:
		>0      element1 goes after element2
		=0      element1 equals element2
		<0      element1 goes before element2
*/
Set *init_set(int (*compare)(const void *, const void *))
{
	Set *set = calloc(1, sizeof(Set));
	if (!set)
		return NULL;
	set->compare = compare;
	return set;
}

/* destroy_set()
	@set            ptr to Set struct

	Free a Set and its nodes from memory.
*/
void destroy_set(Set *set)
{
	if (!set)
		return;
	Node *curr = set->head;
	Node *tmp;
	while (curr) {
		tmp = curr;
		curr = curr->next;
		destroy_node(tmp);
	}
	free(set);
}

/* init_node()
	@data           element to insert into the set

	@return         ptr to dynamically allocated Node struct, NULL if fail

	Dynamically allocate and initialize a Node struct and the user's data.
	This data can be dynamically allocated. It is always the USER'S
	responsibility to manage its memory and lifetime.
*/
Node *init_node(void *data)
{
	Node *node = calloc(1, sizeof(Node));
	if (!node)
		return NULL;
	node->data = data;
	return node;
}

// for symmetry and to prevent confusion, a destroy_node() function is defined
// the user need only call free(node), though
inline void destroy_node(Node *node)
{
	free(node);
}

/* set_is_empty()
	@set            ptr to Set struct

	@return         true if set is empty, otherwise false

	Check if a set is empty.
*/
bool set_is_empty(Set *set)
{
	return set->size == 0;
}

/* set_insert()
	@set            ptr to Set struct
	@element        ptr to element that will be inserted

	@return         0 if insertion is successful, otherwise -1

	Insert an element into a set.
*/
int set_insert(Set *set, void *element)
{
	Node *new = init_node(element);
	if (!new)
		return -1;
	if (!set->head) {
		set->head = new;
		set->tail = new;
		set->size++;
		return 0;
	}

	Node *curr = set->head;
	Node *prev = NULL;
	int cmp;
	while (curr) {
		cmp = (*set->compare)(curr->data, element);
		if (cmp > 0) {
			// insert element before curr
			if (!prev) {
				// insert element before head
				new->next = set->head;
				set->head = new;
			} else {
				prev->next = new;
				new->next = curr;
			}
			set->size++;
			return 0;
		} else if (cmp == 0) {
			// duplicate item so don't insert
			destroy_node(new);
			return 0;
		} else {
			// keep advancing
			prev = curr;
			curr = curr->next;
		}
	}
	// curr went past the tail, so append the element to the set
	set->tail->next = new;
	set->tail = new;
	set->size++;
	return 0;
}

/* set_find()
	@set            ptr to Set struct
	@element        ptr to the element to find

	@return         ptr to element if located, otherwise NULL

	Find an element in a set.
*/
void *set_find(Set *set, void *element)
{
	Node *curr = set->head;
	while (curr) {
		if ((*set->compare)(curr->data, element) == 0)
			return curr->data;
		curr = curr->next;
	}
	return NULL;
}
