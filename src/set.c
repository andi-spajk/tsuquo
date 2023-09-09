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
	@element        element to insert into the set

	@return         ptr to dynamically allocated Node struct, NULL if fail

	Dynamically allocate and initialize a Node struct and the user's
	element. This element can be dynamically allocated. It is always the
	USER'S responsibility to manage its memory and lifetime.
*/
Node *init_node(void *element)
{
	Node *node = calloc(1, sizeof(Node));
	if (!node)
		return NULL;
	node->element = element;
	return node;
}

// for symmetry and to prevent confusion, a destroy_node() function is defined
// the user could always call free(node), though
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
	                INSERT_DUPLICATE if element was a duplicate
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
		cmp = (*set->compare)(curr->element, element);
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

	Find and return an element in a set.
*/
void *set_find(Set *set, void *element)
{
	Node *curr = set->head;
	while (curr) {
		if ((*set->compare)(curr->element, element) == 0)
			return curr->element;
		curr = curr->next;
	}
	return NULL;
}

/* set_decapitate()
	@set            ptr to Set struct

	@return         ptr to first element

	Remove and return the first element from the set.

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

	void *innards = decapitation->element;
	destroy_node(decapitation);
	set->size--;
	return innards;
}

/* set_equals()
	@lhs            ptr to Set struct
	@rhs            ptr to another Set struct

	@return         true if both sets have identical elements, otherwise
	                false

	Check 2 sets for equality: both sets have all the same elements with the
	same element, no more and no less.
*/
bool set_equals(Set *lhs, Set *rhs)
{
	if (lhs->size != rhs->size)
		return false;
	else if (lhs->compare != rhs->compare)
		return false;

	Node *lcmp = lhs->head;
	Node *rcmp = rhs->head;
	while (lcmp && rcmp) {
		if ((*lhs->compare)(lcmp->element, rcmp->element) != 0)
			return false;
		lcmp = lcmp->next;
		rcmp = rcmp->next;
	}
	// exhausted both lists
	return !lcmp && !rcmp;
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

	// TODO: this is a working solution but not a smart solution
	Node *curr = rhs->head;
	while (curr) {
		// set_insert will make new nodes in lhs and copy the elements
		// this prevents double frees in case user destroys both lhs and
		// rhs
		if (set_insert(lhs, curr->element) == INSERT_ERROR)
			return NULL;
		curr = curr->next;
	}
	return lhs;
}

/* set_begin()
	@set            ptr to Set struct

	@return         Set iterator

	Return an iterator of the Set. To access the element, the user should
	dereference like so: (DataType *)(iterator->element)
*/
Iterator *set_begin(Set *set)
{
	if (set)
		return set->head;
	return NULL;
}

/* advance_iter()
	@it             ptr to Set iterator (double ptr)

	@return         the iterator advanced by one element, NULL if no more

	Advance an iterator to the next element. Modifies the user's original
	iterator.
*/
Iterator *advance_iter(Iterator **it)
{
	if (it) {
		if (*it) {
			*it = (*it)->next;
			return *it;
		}
	}
	return NULL;
}

/* compare_sets()
	@s1             ptr to Set of sets
	@s2             ptr to another Set of sets

	@return         value indicating the following:
	                =1: s1 and s2 contain different sets
	                =0: s1 and s2 contain identical sets

	Compare whether two sets of sets contain all the same sets.
*/
int compare_sets(const void *s1, const void *s2)
{
	if (set_equals((Set *)s1, (Set *)s2))
		return 0;
	return 1;
	// positive result means we will insert as soon as possible since >0 is
	// the first condition that set_insert() checks
	// we don't care about sorting the set of sets
}