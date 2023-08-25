/** nfa.c

Construct Thompson nondeterministic finite automata by converting from regular
expressions.

*/

#include <stdlib.h>

#include "common.h"
#include "nfa.h"
#include "set.h"

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

/* compare_nfastate_ptr()
	@n1             ptr to an NFAState struct
	@n2             ptr to an NFAState struct

	@return         any value indicating the following:
	                >0      n1's address goes after n2
	                =0      same NFAState (the only practical result)
	                <0      n1's address goes before n2

	Check whether two NFAStates are the same state.

	Equality is the only practical result since we don't care whether the
	states in the NFA memory region are sorted. When we free them, we must
	iterate through all of them. Who cares about the order, as long as we
	free all of them and perform no double frees.
*/
int compare_nfastate_ptr(const void *n1, const void *n2)
{
	return (NFAState *)n1 - (NFAState *)n2;
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
	nfa->mem_region = init_set(compare_nfastate_ptr);
	if (!nfa->mem_region) {
		free(nfa);
		return NULL;
	}
	return nfa;
}

/* destroy_nfa
	@nfa            ptr to NFA struct

	Free all the memory holding the NFA struct and the saved memory region.
	The actual contents of that region-based memory (ie the NFAStates
	themselves) are unfreed.
*/
void destroy_nfa(NFA *nfa)
{
	if (!nfa)
		return;
	destroy_set(nfa->mem_region);
	free(nfa);
}

/* destroy_nfa_and_states()
	@nfa            ptr to NFA struct

	Free all the memory used by NFA. Constituent states are also freed.
*/
void destroy_nfa_and_states(NFA *nfa)
{
	if (!nfa)
		return;

	Iterator *it = set_begin(nfa->mem_region);
	for (; it; advance_iter(&it))
		destroy_nfastate(it->element);
	destroy_nfa(nfa);
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
	set_insert(nfa->mem_region, start);
	set_insert(nfa->mem_region, accept);

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

/* nfa_union()
	@lhs            ptr to NFA struct
	@rhs            ptr to another NFA struct

	@return         ptr to modified @lhs, NULL if fail

	Perform the Thompson construction for the union of 2 NFAs. Original @lhs
	is modified and @rhs is destroyed.
*/
NFA *nfa_union(NFA *lhs, NFA *rhs)
{
	NFAState *new_accept = init_nfastate();
	NFAState *new_start = init_nfastate();
	if (!(new_accept || new_start)) {
		destroy_nfastate(new_accept);
		destroy_nfastate(new_start);
		return NULL;
	}

	new_start->out1 = lhs->start;
	new_start->out2 = rhs->start;
	lhs->accept->out1 = new_accept;
	rhs->accept->out1 = new_accept;
	lhs->start = new_start;
	lhs->accept = new_accept;

	// rhs brings new symbols to the alphabet
	lhs->alphabet0_63 |= rhs->alphabet0_63;
	lhs->alphabet64_127 |= rhs->alphabet64_127;

	lhs->size += 2;
	lhs->size += rhs->size;

	if (!set_union(lhs->mem_region, rhs->mem_region)) {
		destroy_nfastate(new_accept);
		destroy_nfastate(new_start);
		return NULL;
	}
	set_insert(lhs->mem_region, new_accept);
	set_insert(lhs->mem_region, new_start);
	destroy_nfa(rhs);
	return lhs;
}

/* nfa_append()
	@lhs            ptr to NFA struct
	@rhs            ptr to another NFA struct

	@return         ptr to modified @lhs, NULL if fail

	Perform the Thompson construction for the concatenation of 2 NFAs.
	Original @lhs is modified and @rhs is destroyed.
*/
NFA *nfa_append(NFA *lhs, NFA *rhs)
{
	lhs->accept->out1 = rhs->start;
	// `lhs->accept`'s epsilon transition is still there
	// now reassign accept
	lhs->accept = rhs->accept;

	lhs->size += rhs->size;

	lhs->alphabet0_63 |= rhs->alphabet0_63;
	lhs->alphabet64_127 |= rhs->alphabet64_127;

	if (!set_union(lhs->mem_region, rhs->mem_region))
		return NULL;
	destroy_nfa(rhs);
	return lhs;
}

/* transform()
	@nfa            ptr to NFA struct
	@quantifier     regex quantifier

	@return         ptr to modified @nfa, NULL if fail

	Apply a Thompson transformation for a regex quantifier: *, ?, +
	Original @nfa is modified.
*/
NFA *transform(NFA *nfa, U8 quantifier)
{
	NFAState *new_accept = init_nfastate();
	NFAState *new_start = init_nfastate();
	if (!(new_accept || new_start)) {
		destroy_nfastate(new_accept);
		destroy_nfastate(new_start);
		return NULL;
	}
	set_insert(nfa->mem_region, new_accept);
	set_insert(nfa->mem_region, new_start);

	if (quantifier == '*') {
		nfa->accept->out2 = nfa->start; // the pattern cycles back to
		                                // itself

		nfa->accept->out1 = new_accept; // new accept state
		nfa->accept = new_accept;

		new_start->out1 = nfa->start;   // new start state
		nfa->start = new_start;

		nfa->start->out2 = nfa->accept; // maybe skip pattern
	} else if (quantifier == '?') {
		nfa->accept->out1 = new_accept; // new accept state
		nfa->accept = new_accept;

		new_start->out1 = nfa->start;   // new start state
		nfa->start = new_start;

		nfa->start->out2 = nfa->accept; // maybe skip pattern
	} else if (quantifier == '+') {
		nfa->accept->out2 = nfa->start; // the pattern cycles back to
		                                // itself

		nfa->accept->out1 = new_accept; // new accept state
		nfa->accept = new_accept;

		new_start->out1 = nfa->start;   // new start state
		nfa->start = new_start;
	} else {
		return NULL;
	}
	nfa->size += 2;
	return nfa;
}