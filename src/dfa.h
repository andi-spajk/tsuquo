/** dfa.h

Module definition for deterministic finite automata.

*/

#ifndef DFA_H
#define DFA_H

#include <stdbool.h>

#include "common.h"
#include "nfa.h"
#include "set.h"

#define NUM_ASCII_CHARS 128

typedef struct DFAState {
	struct DFAState **outs;  // array of outward transitions
	int index;
	Set *constituent_nfastates;
	/*
	Subset construction builds DFAStates using sets of sets of NFA states
	Those sets and their corresponding DFAState shall be doubly-linked, eg:
		some_dfastate->constituent_nfastates = some_set
		some_set->id = some_dfastate
	*/
	bool seen;
} DFAState;

typedef struct DFA {
	DFAState *start;
	Set *accepts;
	U8 *alphabet;  // array of chars instead of bitfield
	               // lends itself more easily to a for loop
	int alphabet_size;

	Set *mem_region;
	/*
	Set of sets of NFAStates, usually denoted Q in formal contexts.
	Each set corresponds to 1 DFAState (via set->id).
	Since each set is doubly-linked with the DFAStates, the entire
	collection can function as a region-based memory manager.
	*/

	U8 mappings[NUM_ASCII_CHARS];  // maps alphabet char to an index into
	                               // the DFAState->outs array
} DFA;

DFAState *init_dfastate(int alphabet_size);
void destroy_dfastate(DFAState *state);
DFA *init_dfa(NFA *nfa);
void destroy_dfa(DFA *dfa);

Set *epsilon_closure_delta(Set *nfastates, U8 ch);
DFA *subset(NFA *nfa);

#endif