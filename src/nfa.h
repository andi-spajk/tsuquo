/** nfa.h

Module definition for nondeterministic finite automata.

*/

#ifndef NFA_H
#define NFA_H

#include "common.h"

typedef struct NFAState {
	struct NFAState *out1;
	struct NFAState *out2;
	U8 ch;
	int index;
} NFAState;

typedef struct NFA {
	NFAState *start;
	NFAState *accept;
	U64 alphabet0_63;  // store alphabet as a bitfield
	U64 alphabet64_127;
	int size;
} NFA;

/*
NFA->size and NFAState->index should be DISREGARDED until index_states() is
called!!!
*/

NFAState *init_nfastate(void);
void destroy_nfastate(NFAState *state);
NFA *init_nfa(void);
void destroy_nfa(NFA *nfa);
NFA *init_thompson_nfa(U8 ch);

#endif