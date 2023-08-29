/** dfa.c

Construct deterministic finite automata out of Thompson NFAs using the subset
construction.

*/

#include <stdlib.h>

#include "common.h"
#include "dfa.h"
#include "nfa.h"
#include "set.h"

/* init_dfastate()
	@alphabet_size  number of characters in an NFA's alphabet

	@return         ptr to dynamically allocated DFAState, or NULL if fail

	Dynamically allocate a DFAState and initialize its members.

*/
DFAState *init_dfastate(int alphabet_size)
{
	DFAState *state = calloc(1, sizeof(DFAState));
	if (!state)
		return NULL;

	state->outs = calloc(alphabet_size, sizeof(DFAState *));
	if (!state->outs) {
		free(state->outs);
		free(state);
		return NULL;
	}

	// constituent_nfastates is NOT allocated by the DFAState
	// We can't figure out the constituent NFA states UNTIL we are doing
	// the subset construction.
	// So subset should assigns the set to the DFA state's pointer.
	// If we initialize a set in here, we will leak memory when the subset
	// assigns the actual set of constituent NFAStates.

	state->index = -1;
	return state;
}

/* destroy_dfastate()
	@state          ptr to DFAState struct

	Free all the memory used by a DFAState.
*/
void destroy_dfastate(DFAState *state)
{
	if (!state)
		return;
	free(state->outs);
	// DFAState is doubly-linked to a set of NFAStates
	// the dfa->mem_region stores a set of these sets of NFAStates
	// destroy_dfa will handle them and the DFAStates
	free(state);
}

/* compare_dfastates()
	@d1             ptr to DFAState struct
	@d2             ptr to another DFAState struct

	@return         any value indicating the following:
	                >0: d1 index goes after d2
	                =0: d1 index is d2
	                <0: d1 index goes before d2

	Check whether two DFAStates have the same index.
*/
int compare_dfastates(const void *d1, const void *d2)
{
	return ((DFAState *)d1)->index - ((DFAState *)d2)->index;
}

/* count_bits
	@x              64-bit unsigned integer

	@return         number of 1-bits in the integer

	Counts the number of 1 bits.

	Taken from https://en.wikipedia.org/wiki/Hamming_weight
	The NFA alphabet bitfield is likely to have large contiguous chunks of
	1 bits. There will be large contiguous chunks of 0s, so I could use the
	 Wegner algorithm, but there are very few chunks of 0s compared to the
	amount of 1s. I would spend more time iterating over the chunk of 1s
	even though there are lots of 0s.
	So I opted for the popcount64c() implementation.
*/
static int count_bits(U64 x)
{
	x -= (x >> 1) & 0x5555555555555555;
	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
	return (x * 0x0101010101010101) >> 56;
}

/* init_dfa()
	@nfa            ptr to NFA which is used to construct the DFA

	@return         ptr to dynamically allocated DFA, or NULL if fail

	Dynamically allocate a DFA struct and initialize its members, using a
	base NFA as a guide.
*/
DFA *init_dfa(NFA *nfa)
{
	DFA *dfa = calloc(1, sizeof(DFA));
	if (!dfa)
		return NULL;

	dfa->alphabet_size = count_bits(nfa->alphabet0_63);
	dfa->alphabet_size += count_bits(nfa->alphabet64_127);

	dfa->alphabet = malloc(dfa->alphabet_size);
	dfa->accepts = init_set(compare_dfastates);
	dfa->mem_region = init_set(compare_sets);
	if (!(dfa->accepts || dfa->mem_region || dfa->alphabet)) {
		destroy_set(dfa->accepts);
		destroy_set(dfa->mem_region);
		free(dfa->alphabet);
		free(dfa);
		return NULL;
	}

	// build alphabet and its mapping to an index into state->outs[]
	U8 ascii = 0;
	int i = 0;
	U64 bitfield = nfa->alphabet0_63;
	while (ascii < NUM_ASCII_CHARS) {
		if (bitfield & 1) {
			dfa->alphabet[i] = ascii;
			dfa->mappings[ascii] = i;
			i++;
		}
		ascii++;
		bitfield >>= 1;
		if (ascii == NUM_ASCII_CHARS / 2)
			bitfield = nfa->alphabet64_127;
	}
	return dfa;
}

/* destroy_dfa()
	@dfa            ptr to DFA struct

	Free all the memory used by a DFA.
*/
void destroy_dfa(DFA *dfa)
{
	if (!dfa)
		return;
	free(dfa->alphabet);
	destroy_set(dfa->accepts);

	// for each set in the set of sets of nfastates
	Set *qset;
	for (Iterator *q = set_begin(dfa->mem_region); q; advance_iter(&q)) {
		qset = (Set *)(q->element);
		// destroy the dfastate corresponding to the set of nfastates
		destroy_dfastate((DFAState *)(qset->id));
		// destroy the set of nfastates
		destroy_set(qset);
	}
	destroy_set(dfa->mem_region);

	free(dfa);
}

/* epsilon_closure_delta
	@nfastates      set of NFAStates
	@ch             transition character

	@return         set of NFA states reachable from @nfastates via @ch

	For each state in @nfastates, transition on @ch and find the epsilon
	closure of that transition (if any). Aggregate all the epsilon closures
	into one set.
*/
Set *epsilon_closure_delta(Set *nfastates, U8 ch)
{
	Set *result = init_set(compare_nfa_states);
	Set *epsilon;
	NFAState *nfastate;
	for (Iterator *it = set_begin(nfastates); it; advance_iter(&it)) {
		nfastate = (NFAState *)(it->element);
		if (nfastate->ch == ch) {
			epsilon = epsilon_closure(nfastate->out1);
			result = set_union(result, epsilon);
			destroy_set(epsilon);
		}
	}
	return result;
}

/* subset()
	@nfa            ptr to NFA struct

	@return         the NFA's equivalent DFA

	Convert an NFA to a DFA.
*/
DFA *subset(NFA *nfa)
{
	DFA *dfa = init_dfa(nfa);
	int state_index = 0;

	DFAState *start = init_dfastate(dfa->alphabet_size);
	start->index = state_index;
	state_index++;
	dfa->start = start;

	Set *q0 = epsilon_closure(nfa->start);
	// doubly link the DFA state and the set of NFA states
	q0->id = start;
	start->constituent_nfastates = q0;
	if (set_find(q0, nfa->accept))
		set_insert(dfa->accepts, q0->id);
	set_insert(dfa->mem_region, q0);

	Set *worklist = init_set(compare_sets);
	set_insert(worklist, q0);

	Set *q, *t, *found;
	DFAState *new, *qstate;
	U8 ch;
	while (!set_is_empty(worklist)) {
		q = set_decapitate(worklist);
		qstate = (DFAState *)(q->id);
		for (int i = 0; i < dfa->alphabet_size; i++) {
			ch = dfa->alphabet[i];
			t = epsilon_closure_delta(q, ch);
			if (set_is_empty(t)) {
				destroy_set(t);
				continue;
			}
			found = set_find(dfa->mem_region, t);
			if (!found) {
				// t represents a new DFA state
				new = init_dfastate(dfa->alphabet_size);
				new->index = state_index;
				state_index++;
				// doubly link the DFA state with the set of NFA
				// states
				t->id = new;
				new->constituent_nfastates = t;

				// q transitions to t via ch
				qstate->outs[i] = (DFAState *)(t->id);

				if (set_find(t, nfa->accept))
					set_insert(dfa->accepts, t->id);
				set_insert(dfa->mem_region, t);
				set_insert(worklist, t);
			} else {
				// t represents an already-existing DFA state
/*
Use found, not q.
We used q to build every t in this for loop, but q and t do not have the same
corresponding DFAState.
We used the same q for each t, BUT A DIFFERENT ch FOR EACH t.
So q may be transitioning to itself but it also may not.
Oh my god this mistake is so obvious in hindsight. Wtf was I thinking???
*/
				qstate->outs[i] = (DFAState *)(found->id);
				if (set_find(found, nfa->accept))
					set_insert(dfa->accepts, found->id);
				destroy_set(t);
			}
		}
	}
	destroy_set(worklist);
	return dfa;
}