/** nfa.c

Construct Thompson nondeterministic finite automata by converting from regular
expressions.

*/

#include <stdlib.h>

#include "common.h"
#include "nfa.h"

/* init_nfastate()
	@return         ptr to dynamically allocated NFAState, NULL if fail

	Dynamically allocate an NFAState struct and initialize its members.
*/
NFAState *init_nfastate(void)
{
	NFAState *state = calloc(1, sizeof(NFAState));
	if (!state)
		return NULL;
	state->index = -1;
	return state;
}

// for symmetry and to prevent confusion, a destroy_nfastate() function is
// defined
// the user could always call free(state), though
inline void destroy_nfastate(NFAState *state)
{
	free(state);
}

/* init_nfa()
	@return         ptr to dynamically allocated NFA, NULL if fail

	Dynamically allocate an NFA struct and initialize its members.
*/
NFA *init_nfa(void)
{
	NFA *nfa = calloc(1, sizeof(NFA));
	if (!nfa)
		return NULL;
	// everything is zero-initialized
	return nfa;
}

/* destroy_nfa()
	@nfa            ptr to NFA struct

	Shallow destroy an NFA. Constituent states are not traversed nor
	destroyed.
*/
inline void destroy_nfa(NFA *nfa)
{
	free(nfa);
}

/* init_thompson_nfa()
	@ch             the character which triggers the transition

	@return         ptr to newly constructed NFA

	Construct a basic Thompson NFA unit: one start state and one accept
	state, with a single transition on @ch.
*/
NFA *init_thompson_nfa(U8 ch)
{
	NFAState *start = init_nfastate();
	NFAState *accept = init_nfastate();
	NFA *nfa = init_nfa();
	if (!(start || accept || nfa)) {
		destroy_nfastate(start);
		destroy_nfastate(accept);
		destroy_nfa(nfa);
		return NULL;
	}

	start->ch = ch;
	start->out1 = accept;
	nfa->start = start;
	nfa->accept = accept;
	if (ch <= 63)
		nfa->alphabet0_63 |= (1ULL << ch);
	else
		nfa->alphabet64_127 |= (1ULL << (ch-64));
	nfa->size = 2;
	return nfa;
}