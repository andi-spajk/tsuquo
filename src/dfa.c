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
	state->constituent_nfas = init_set(compare_nfa_states);
	if (!(state->outs || state->constituent_nfas)) {
		free(state->outs);
		destroy_set(state->constituent_nfas);
		free(state);
		return NULL;
	}

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
	// freeing the actual nfastates is done by the NFA module
	destroy_set(state->constituent_nfas);
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

	// for each set of sets of nfastates
	for (Iterator *q = set_begin(dfa->mem_region); q; advance_iter(&q)) {
		// destroy the corresponding dfastate of the set
		destroy_dfastate(((Set *)q)->id);
		destroy_set((Set *)q);
	}
	destroy_set(dfa->mem_region);

	free(dfa);
}
