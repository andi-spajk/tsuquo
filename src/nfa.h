/** nfa.h

Module definition for Thompson nondeterministic finite automata.

*/

#ifndef NFA_H
#define NFA_H

#include <stdbool.h>

#include "common.h"
#include "set.h"

// epsilon transition
#define EPSILON 0x00

typedef struct NFAState {
	struct NFAState *out1;
	struct NFAState *out2;
	U8 ch;
	int index;  // should be DISREGARDED until index_states() is called!!!
	bool seen;
} NFAState;

typedef struct NFA {
	NFAState *start;
	NFAState *accept;
	U64 alphabet0_63;  // store alphabet as a bitfield
	U64 alphabet64_127;
	int size;
	Set *mem_region;  // set of all of ptrs to NFAStates in the NFA
	                  // this (kinda) region-based system simplifies cleanup
	                  // of NFAStates
} NFA;

NFAState *init_nfastate(void);
void destroy_nfastate(NFAState *state);
int compare_nfastate_ptr(const void *n1, const void *n2);

NFA *init_nfa(void);
void destroy_nfa(NFA *nfa);
void destroy_nfa_and_states(NFA *nfa);
NFA *init_thompson_nfa(U8 ch);
NFA *nfa_union(NFA *lhs, NFA *rhs);
NFA *nfa_append(NFA *lhs, NFA *rhs);
NFA *transform(NFA *nfa, U8 quantifier);
NFA *init_range_nfa(U8 left, U8 right);

int index_states(NFA *nfa);
int gen_nfa_graphviz(NFA *nfa, const char *file_name);
int compare_nfastates(const void *n1, const void *n2);
Set *epsilon_closure(NFAState *state);

#endif