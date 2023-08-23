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

	@return         INSERT_SUCCESS if insertion is successful
	                INSERT_DUPLICATE if item was a duplicate
	                INSERT_ERROR otherwise

	Insert an element into a set. If the element is a duplicate, the set is
	unmodified.
*/
int set_insert(Set *set, void *element)
{
	Node *new = init_node(element);
	if (!new)
		return INSERT_ERROR;
	if (!set->head) {
		set->head = new;
		set->tail = new;
		set->size++;
		return INSERT_SUCCESS;
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
			return INSERT_SUCCESS;
		} else if (cmp == 0) {
			// it's a duplicate item, so don't insert
			destroy_node(new);
			return INSERT_DUPLICATE;
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
	return INSERT_SUCCESS;
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

/* set_decapitate()
	@set            ptr to Set struct

	@return         ptr to first element

	Remove the first element of the set.

	Naming history: remove_tail --> chop_tail --> chop_head --> decapitate
*/
void *set_decapitate(Set *set)
{
	if (!set->head)
		return NULL;

	Node *decapitation = set->head;
	Node *new_head = set->head->next;
	if (!new_head) {
		// 1-element set
		set->head = NULL;
		set->tail = NULL;
	} else {
		set->head = new_head;
	}

	void *innards = decapitation->data;
	destroy_node(decapitation);
	set->size--;
	return innards;
}

/* set_equals()
	@s1             ptr to Set struct
	@s2             ptr to another Set struct

	@return         true if both sets have identical elements, otherwise
	                false

	Check 2 sets for equality: both sets have all the same elements with the
	same data, no more and no less.
*/
bool set_equals(Set *s1, Set *s2)
{
	if (s1->size != s2->size)
		return false;
	else if (s1->compare != s2->compare)
		return false;

	Node *cmp1 = s1->head;
	Node *cmp2 = s2->head;
	while (cmp1 && cmp2) {
		if ((*s1->compare)(cmp1->data, cmp2->data) != 0)
			return false;
		cmp1 = cmp1->next;
		cmp2 = cmp2->next;
	}
	// exhausted both lists
	return !cmp1 && !cmp2;
}

/* set_union()
	@lhs            ptr to Set that will receive new elements
	@rhs            ptr to Set that will merge into @lhs

	@return         ptr to modified @lhs, NULL if error

	Merge two sets together, ie `lhs |= rhs`. @rhs will be unmodified.
*/
Set *set_union(Set *lhs, Set *rhs)
{
	// some_set |= {}
	if (!rhs->head)
		return lhs;

	// working solution but not a smart solution
	Node *curr = rhs->head;
	while (curr) {
		if (set_insert(lhs, curr->data) == INSERT_ERROR)
			return NULL;
		curr = curr->next;
	}
	return lhs;
}