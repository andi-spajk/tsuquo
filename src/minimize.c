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
*/
int compare_minimal_dfastates(const void *m1, const void *m2)
{
	return ((MinimalDFAState *)m1)->index - ((MinimalDFAState *)m2)->index;
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

	min_dfa->mem_region = init_set(compare_sets);
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
			if (distinguishable(outi, outj, min_dfa, dfa))
				return true;
		}
		return false;
	}
}