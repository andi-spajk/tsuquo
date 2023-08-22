/* set.c

Sorted set. Internally represented as a linked list.

*/

#include <stdbool.h>
#include "stdlib.h"

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

	@return         ptr to dynamically allocated Node struct

	Dynamically allocate and initialize a Node struct and the user's data.
	This data can be dynamically allocated. It is always the USER'S
	responsibility to manage its memory and lifetime.
*/
Node *init_node(void *data)
{
	Node *node = calloc(1, sizeof(Node));
	node->data = data;
	return node;
}

// for symmetry and to prevent confusion, a destroy_node() function is defined
// the user need only call free(node), though
inline void destroy_node(Node *node)
{
	free(node);
}

bool set_is_empty(Set *set)
{
	return set->size == 0;
}