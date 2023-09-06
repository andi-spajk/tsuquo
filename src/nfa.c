/** nfa.c

Construct Thompson nondeterministic finite automata by converting from regular
expressions.

*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "nfa.h"
#include "set.h"

// to recursively enumerate every NFA state
static int state_index = 0;

/* init_nfastate()
	@return         ptr to dynamically allocated NFAState, NULL if fail

	Dynamically allocate an NFAState struct and initialize its members.
*/
NFAState *init_nfastate(void)
{
	NFAState *state = calloc(1, sizeof(NFAState));
	if (!state)
		return NULL;
	// -1 is a sentinel
	state->index = -1;
	// not sure if calloc automatically fulfills this
	state->seen = false;
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
	if (!rhs)
		return lhs;
	if (!lhs)
		return rhs;

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

	Perform the Thompson construction for the concatenation of 2 NFAs. @rhs
	is connected to the end of @lhs. Original @lhs is modified and @rhs is
	destroyed.
*/
NFA *nfa_append(NFA *lhs, NFA *rhs)
{
	if (!lhs)
		return rhs;
	if (!rhs)
		return lhs;

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
	@quantifier     regex quantifier character

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

/* init_range_nfa()
	@left           lower bound of the regex range
	@right          upper bound of the regex range

	@return         ptr to NFA struct representing the entire range, NULL if
	                fail

	Recursively construct an NFA that represents a regular expression range.

	TODO: The result NFA almost resembles a binary tree but it's not
	perfect. Improve it later.
*/
NFA *init_range_nfa(U8 left, U8 right)
{
	NFA *t1, *t2;
	if (left == right) {
		return init_thompson_nfa(left);
	} else {
		t1 = init_range_nfa(left, left+((right-left)/2));
		t2 = init_range_nfa(left+((right-left)/2)+1, right);
		return nfa_union(t1, t2);
	}
}

/* reset_states()
	@nfa            ptr to NFA struct

	Tag all of the NFA's states to have index -1 and flag the state as
	unseen.
*/
static void reset_states(NFA *nfa)
{
	state_index = 0;
	for (Iterator *it = set_begin(nfa->mem_region); it; advance_iter(&it)) {
		((NFAState *)(it->element))->index = -1;
		((NFAState *)(it->element))->seen = false;
	}
}

/* index_helper()
	@state          ptr to NFA state

	Recursively enumerate all NFA states.
*/
static void index_helper(NFAState *state)
{
	if (state->out1) {
		// only recurse if state has not already been tagged with a
		// meaningful index
		if (state->out1->index == -1) {
			state_index++;
			state->out1->index = state_index;
			index_helper(state->out1);
		}
	}
	if (state->out2) {
		if (state->out2->index == -1) {
			state_index++;
			state->out2->index = state_index;
			index_helper(state->out2);
		}
	}
	return;
}

/* index_states()
	@nfa            ptr to NFA struct

	@return         index of the last NFA state that was tagged

	Enumerate every state in an NFA.
*/
int index_states(NFA *nfa)
{
	reset_states(nfa);
	nfa->start->index = state_index;
	index_helper(nfa->start);
	return state_index;
}

/* graphviz_helper()
	@state          ptr to NFA state
	@f              output file

	Recursively print the Graphviz DOT representation for every transition
	in an NFA.
*/
static void graphviz_helper(NFAState *state, FILE *f)
{
	state->seen = true;
	if (state->out1) {
		fprintf(f, "\tn%d", state->index);
		fprintf(f, " ->");
		fprintf(f, " n%d", state->out1->index);
		switch (state->ch) {
		case EPSILON:
			fprintf(f, " [label=\"&epsilon;\"]\n");
			break;
		case '"':
			fprintf(f, " [label=\"\\\"\"]\n");
			break;
		case '\\':
			// double backslash because we are representing a string
			// in a string
			fprintf(f, " [label=\"\\\\\"]\n");
			break;
		case '\t':
			fprintf(f, " [label=\"\\\\t\"]\n");
			break;
		case '\n':
			fprintf(f, " [label=\"\\\\n\"]\n");
			break;
		default:
			fprintf(f, " [label=\"%c\"]\n", state->ch);
			break;
		}
		// we printed transition, but only perform the transition if the
		// out state hasn't been tagged as seen
		if (!state->out1->seen)
			graphviz_helper(state->out1, f);
	}
	if (state->out2) {
		fprintf(f, "\tn%d", state->index);
		fprintf(f, " ->");
		fprintf(f, " n%d", state->out2->index);
		// out2 is always an epsilon transition because any non-epsilon
		// transition goes to out1 by default, and Thompson NFA states
		// can't have transitions on two different symbols
		fprintf(f, " [label=\"&epsilon;\"]\n");
		if (!state->out2->seen)
			graphviz_helper(state->out2, f);
	}
	return;
}

/* gen_nfa_graphviz()
	@nfa            ptr to NFA struct
	@file_name      name of output file

	@return         0 if success, otherwise -1

	Print the Graphviz DOT representation of an entire NFA to a file.
*/
int gen_nfa_graphviz(NFA *nfa, const char *file_name)
{
	index_states(nfa);
	FILE *f = fopen(file_name, "w");
	if (!f)
		return -1;

	fprintf(f, "digraph NFA {\n");
	fprintf(f, "\tfontname = \"Helvetica,Arial,sans-serif\";\n");
	fprintf(f, "\tnode [fontname=\"Helvetica,Arial,sans-serif\"];\n");
	fprintf(f, "\tedge [fontname=\"Helvetica,Arial,sans-serif\"];\n");
	fprintf(f, "\trankdir = LR;\n");

	fprintf(f, "\tnode [shape=doublecircle]");
	fprintf(f, " n%d;\n", nfa->accept->index);

	fprintf(f, "\tnode [shape=circle];\n");
	graphviz_helper(nfa->start, f);
	fprintf(f, "}\n");

	fclose(f);
	return 0;
}

/* compare_nfastates()
	@n1             ptr to NFAState struct
	@n2             ptr to another NFAState struct

	@return         any value indicating the following:
	                >0: n1 index goes after n2
	                =0: n1 and n2 have equal indices
	                <0: n1 index goes before n2

	Numerically compare two NFA states' indices. Use this to sort them in a
	Set of NFAStates.
*/
int compare_nfastates(const void *n1, const void *n2)
{
	return ((NFAState *)n1)->index - ((NFAState *)n2)->index;
}

/* epsilon_helper()
	@state          ptr to NFAState struct
	@set            ptr to set of NFAStates

	@return         set of states in the epsilon closure of @state

	Recursively find the epsilon closure of an NFA state.
*/
static Set *epsilon_helper(NFAState *state, Set *set)
{
	set_insert(set, state);
	if (state->ch != EPSILON)
		return set;
	if (state->out1)
		epsilon_helper(state->out1, set);
	if (state->out2)
		epsilon_helper(state->out2, set);
	return set;
}

/* epsilon_helper()
	@state          ptr to NFAState struct

	@return         set of states in the epsilon closure of @state

	Find the epsilon closure of an NFA state. The NFA state itself is also
	included.
*/
Set *epsilon_closure(NFAState *state)
{
	Set *set = init_set(compare_nfastates);
	if (!set)
		return NULL;
	return epsilon_helper(state, set);
}