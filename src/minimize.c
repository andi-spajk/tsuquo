/** minimize.c

Minimize a deterministic finite automata.

*/

#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "dfa.h"
#include "minimize.h"
#include "set.h"

/* compare_minimal_dfastates()
	@m1             ptr to MinimalDFAState
	@m2             ptr to another MinimalDFAState

	@return         any value indicating the following:
	                >0: m1 index goes after m2
	                =0: m1 index is m2
	                <0: m1 index goes before m2

	Check whether two MinimalDFAStates have the same index.
*/
int compare_minimal_dfastates(const void *m1, const void *m2)
{
	return ((MinimalDFAState *)m1)->index - ((MinimalDFAState *)m2)->index;
}

/* compare_minimal_sets()
	@s1             ptr to Set of DFAState indices
	@s2             ptr to another Set of DFAState indices

	@return         any value indicating the following:
	                >0: s1 goes after s2
	                =0: s1 is s2
	                <0: s1 goes before s2

	Check whether two sets of DFAState indices are the same.

	A set of DFAState indices represents one state in the MinimalDFA since
	all the indices in the set form ONE equivalence class. A DFA state can
	only be collapsed into one equivalence class, otherwise the equivalence
	classes would overlap, meaning we haven't fully refined the partitions.

	Therefore one index can never appear in another set. Therefore we can
	just check the first element.
*/
int compare_minimal_sets(const void *s1, const void *s2)
{
	return *(int *)( ((Set *)s1)->head->element ) -
	       *(int *)( ((Set *)s2)->head->element );
}

/* compare_ints()
	@num1           ptr to an integer
	@num2           ptr to another integer

	@return         any value indicating the following:
	                >0: num1 is greater than num2
	                =0: num1 equals num2
	                <0: num1 is less than num2
*/
int compare_ints(const void *num1, const void *num2)
{
	return *(int *)num1 - *(int *)num2;
}

/* init_minimal_dfastate()
	@return         ptr to dynamically allocated MinimalDFAState, or NULL if
	                fail

	Dynamically allocate a MinimalDFAState and initialize its members.
*/
MinimalDFAState *init_minimal_dfastate(void)
{
	MinimalDFAState *min_state = calloc(1, sizeof(MinimalDFAState));
	return min_state;
}

// for symmetry and to prevent confusion, a destroy_minimal_dfastate() function
// is defined
// the user could always call free(state), though
inline void destroy_minimal_dfastate(MinimalDFAState *min_state)
{
	free(min_state);
}

/* init_minimal_dfa()
	@dfa            ptr to DFA struct

	@return         ptr to dynamically allocated MinimalDFA, or NULL if fail

	Dynamically allocate a MinimalDFA and initialize all possible members.
*/
MinimalDFA *init_minimal_dfa(DFA *dfa)
{
	MinimalDFA *min_dfa = calloc(1, sizeof(MinimalDFA));
	if (!min_dfa)
		return NULL;

	min_dfa->accepts = init_set(compare_minimal_dfastates);

	min_dfa->rows = dfa->size - 1;
	min_dfa->cols = dfa->size;
	int **I = malloc(min_dfa->rows * sizeof(int *));
	if (!I) {
		destroy_minimal_dfa(min_dfa);
		return NULL;
	}
	for (int i = 0; i < min_dfa->rows; i++) {
		I[i] = malloc(min_dfa->cols * sizeof(int));
		if (!I[i]) {
			destroy_minimal_dfa(min_dfa);
			return NULL;
		}
	}
	min_dfa->merge = I;

	// construct the table of indistinguishable states
	DFAState *accepti, *acceptj;
	for (int i = 0; i < min_dfa->rows; i++) {
		for (int j = i+1; j < min_dfa->cols; j++ ) {
			accepti = set_find(dfa->accepts, dfa->states[i]);
			acceptj = set_find(dfa->accepts, dfa->states[j]);
			if (!accepti ^ !acceptj)
				min_dfa->merge[i][j] = 0;
			else
				min_dfa->merge[i][j] = 1;
		}
	}

	// delta is allocated and constructed later

	min_dfa->numbers = malloc(dfa->size * sizeof(int));
	for (int i = 0; i < dfa->size; i++)
		min_dfa->numbers[i] = i;

	min_dfa->mem_region = init_set(compare_minimal_sets);
	if (!min_dfa->mem_region) {
		destroy_minimal_dfa(min_dfa);
		return NULL;
	}
	return min_dfa;
}

/* destroy_minimal_dfa()
	@min_dfa        ptr to MinimalDFA struct

	Free all the memory used by a MinimalDFA and its states.
*/
void destroy_minimal_dfa(MinimalDFA *min_dfa)
{
	if (!min_dfa)
		return;

	destroy_set(min_dfa->accepts);

	if (min_dfa->merge) {
		for (int i = 0; i < min_dfa->rows; i++) {
			free(min_dfa->merge[i]);
		}
	}
	free(min_dfa->merge);

	// free delta

	free(min_dfa->numbers);

	Iterator *q = set_begin(min_dfa->mem_region);
	Set *qset;
	for (; q; advance_iter(&q)) {
		qset = (Set *)(q->element);
		// destroy corresponding minimal dfastate
		destroy_minimal_dfastate((MinimalDFAState *)(qset->id));
		// then destroy the set
		destroy_set(qset);
	}
	destroy_set(min_dfa->mem_region);

	free(min_dfa);
}


/* distinguishable()
	@i              a DFA state index
	@j              another DFA state index
	@min_dfa        ptr to MinimalDFA struct
	@dfa            ptr to DFA struct

	@return         true if states i,j are distinguishable, otherwise false

	Check whether two states are distinguishable according to the quotient
	construction.

	If only one of i,j is an accepting state, then they are distinguishable.
	Otherwise, if some suffix causes only one of i,j to transition to an
	accepting state, then they are distinguishable.
	Otherwise, no such suffix exists. Then i,j are in the same equivalence
	class, so they are indistinguishable.
*/
bool distinguishable(int i, int j, MinimalDFA *min_dfa, DFA *dfa)
{
	// a state is always indistinguishable with itself
	// no suffix can ever distinguish a state from itself, because every
	// suffix will generate the same transitions on i,j because i,j are
	// the same state
	if (i == j)
		return false;

	// dead/error state doesn't have a spot in the merge[][] table
	if (i != DEAD_STATE && j != DEAD_STATE) {
		if (i == min_dfa->rows)
			return false;
		// is the pair already marked as distinguishable?
		if (!min_dfa->merge[i][j])
			return true;
	}

	DFAState *accepti;
	DFAState *acceptj;
	if (i == DEAD_STATE)
		accepti = NULL;
	else
		accepti = set_find(dfa->accepts, dfa->states[i]);
	if (j == DEAD_STATE)
		acceptj = NULL;
	else
		acceptj = set_find(dfa->accepts, dfa->states[j]);
	if (!accepti ^ !acceptj) {
		return true;
	} else {
		// recursively generate every suffix that could come out of
		// states i,j
		int outi, outj;
		for (int c = 0; c < dfa->alphabet_size; c++) {
			outi = i;
			outj = j;
			// don't index delta with -1
			if (i != DEAD_STATE)
				outi = dfa->delta[i][c];
			if (j != DEAD_STATE)
				outj = dfa->delta[j][c];
			// check for infinite loop
			// TODO: this check might be unnecessary
			// uncomment it at your own risk
			// if (outi == i && outj == j)
			// 	continue;
			// else if (outi == j && outj == i)
			// 	continue;
			if (distinguishable(outi, outj, min_dfa, dfa))
				return true;
		}
		return false;
	}
}

/* quotient()
	@min_dfa        ptr to MinimalDFA struct
	@dfa            ptr to DFA struct

	@return         0 if success (there should be no other return codes)

	Perform the quotient construction on a DFA in order to find equivalent
	states.
*/
int quotient(MinimalDFA *min_dfa, DFA *dfa)
{
	bool made_mark = true;
	while (made_mark) {
		made_mark = false;
		for (int i = 0; i < min_dfa->rows; i++) {
			for (int j = i+1; j < min_dfa->cols; j++) {
				// pair is already marked as distinguishable
				// distinguishable = don't merge
				if (!min_dfa->merge[i][j])
					continue;
				if (distinguishable(i, j, min_dfa, dfa)) {
					min_dfa->merge[i][j] = false;
					made_mark = true;
				}
			}
		}
	}
	return 0;
}

/* collect_equivalents()
	@i              row number
	@equ_class      ptr to Set representing an equivalence class
	@min_dfa        ptr to MinimalDFA struct

	@return         ptr to updated @equ_class

	Recursively collect all the equivalent states of a particular row,
	thereby creating an equivalence class.
*/
static Set *collect_equivalents(int i, Set *equ_class, MinimalDFA *min_dfa)
{
	if (i == min_dfa->rows) {
		min_dfa->collected_last_state = true;
		return equ_class;
	}
	if (min_dfa->merge[i][i+1] == -1)
		return equ_class;
	for (int j = i+1; j < min_dfa->cols; j++) {
		if (min_dfa->merge[i][j]) {
			set_insert(equ_class, &(min_dfa->numbers[j]));
			collect_equivalents(j, equ_class, min_dfa);
		}
	}
	// mark the row as already visited
	min_dfa->merge[i][i+1] = -1;
	return equ_class;
}

/* construct_minimal_states()
	@min_dfa        ptr to MinimalDFA struct
	@dfa            ptr to DFA struct

	@return         ptr to updated minimal DFA, or NULL if fail

	Construct all the minimal states in the minimal DFA.
*/
MinimalDFA *construct_minimal_states(MinimalDFA *min_dfa, DFA *dfa)
{
	Set *min_set;
	MinimalDFAState *min_state;
	int state_index = 0;
	Iterator *it;
	DFAState *qstate;
	for (int i = 0; i < min_dfa->rows; i++) {
		if (min_dfa->merge[i][i+1] == -1)
			continue;

		// build equivalence class
		min_set = init_set(compare_ints);
		if (!min_set)
			return NULL;
		set_insert(min_set, &(min_dfa->numbers[i]));
		collect_equivalents(i, min_set, min_dfa);

		min_state = init_minimal_dfastate();
		if (!min_state)
			return NULL;
		min_state->index = state_index;
		state_index++;
		min_set->id = min_state;
		min_state->constituent_dfa_indices = min_set;

		// is the minimal state an accepting state?
		it = set_begin(dfa->accepts);
		for (; it; advance_iter(&it)) {
			qstate = (DFAState *)(it->element);
			if (set_find(min_set, &(qstate->index))) {
				min_state->is_accept = true;
				set_insert(min_dfa->accepts, min_state);
				break;
			}
		}

		if (i == 0)
			min_dfa->start = min_state;
		set_insert(min_dfa->mem_region, min_set);
	}

	// TODO: make a solution that's not copy-and-paste
	// if DFA has states 0..N, then min_dfa->rows stops at state N-1
	// we must add state N into an equivalence class if it wasn't already
	// picked up during equivalent state collection
	if (!min_dfa->collected_last_state) {
		min_set = init_set(compare_ints);
		if (!min_set)
			return NULL;
		set_insert(min_set, &(min_dfa->numbers[min_dfa->rows]));

		min_state = init_minimal_dfastate();
		if (!min_state)
			return NULL;
		min_state->index = state_index;
		state_index++;
		min_set->id = min_state;
		min_state->constituent_dfa_indices = min_set;

		it = set_begin(dfa->accepts);
		for (; it; advance_iter(&it)) {
			qstate = (DFAState *)(it->element);
			if (set_find(min_set, &(qstate->index))) {
				min_state->is_accept = true;
				set_insert(min_dfa->accepts, min_state);
				break;
			}
		}
		set_insert(min_dfa->mem_region, min_set);
	}
	min_dfa->size = min_dfa->mem_region->size;
	return min_dfa;
}