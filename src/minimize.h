/** minimize.h

Module definition for the DFA minimizer.

*/

#ifndef MINIMIZE_H
#define MINIMIZE_H

#include <stdbool.h>

#include "common.h"
#include "dfa.h"
#include "set.h"

#define ASCII0_63   0
#define ASCII64_127 1

typedef struct MinimalDFAState {
	int index;
	bool is_accept;
	Set *constituent_dfa_indices;
} MinimalDFAState;

typedef struct MinimalDFA {
	MinimalDFAState *start;
	Set *accepts;
	int **merge;
	/*
	Table of indistinguishable unminimal DFA states, which gets partitioned
	into all equivalent states after the quotient construction.
	Must be int since we store 3 possible values:
		 1: states are indistinguishable
		 0: states are distinguishable
		-1: row has been visited (needed for minimal state construction)
	The quotient construction requires us to only consider
		dimensions: (number of states - 1) x (number of states)
		indices: [0,N-2] x [0,N-1]
	NOTE: the second dimension could also decrease by 1, but I don't do that
	because it makes all for loops more self-documenting:
		for (int i = 0; i < min_dfa->rows; i++) {
			for (int j = i+1; j < min_dfa->cols; j++) {
				...
			}
		}
	*/
	bool collected_last_state;
	// since merge[][] doesn't have a row index = (number of states - 1), we
	// will keep this flag that indicates whether the last state (whose
	// index equals (number of states - 1)) was already collapsed into an
	// equivalence class
	// see construct_minimal_states() for more

	U64 ***delta;
	/*
	Inverse transition table
	Regular DFA maps a char to a destination state
	This table flips that: it maps a destination state to a char(s), and
	can hold all the minimal DFA state transitions, whereas the regular DFA
	states each need their own individual array of transitions

	This table stores char bitfields that denote all the valid transitions
	eg: delta[0][2][ASCII64_127] = 0x1
		This indicates that d0 transitions to d2 on ASCII value 64
	*/
	int rows, cols;  // delta dimensions

	int *numbers;
	/*
	Just an array of numbers, [0] = 0, [1] = 1, etc
	We make sets of DFAState indices (ie plain integers) and use them
	throughout many minimization functions, so we can't just insert stack
	variables
	Reusing the same dynamically allocated memory won't cause issues because
	we only care about the integer value, so store all of them inside the
	MinimalDFA
	*/

	Set *mem_region;  // set of sets of DFAState indices
	                  // each set is doubly-linked to a MinimalDFAState, so
	                  // it functions as a (kinda) region-based memory
	                  // manager
	int size;  // determined after construct_minimal_states() is called
} MinimalDFA;

int compare_minimal_dfastates(const void *m1, const void *m2);
int compare_minimal_sets(const void *s1, const void *s2);
int compare_ints(const void *num1, const void *num2);

MinimalDFAState *init_minimal_dfastate(void);
void destroy_minimal_dfastate(MinimalDFAState *min_state);

MinimalDFA *init_minimal_dfa(DFA *dfa);
void destroy_minimal_dfa(MinimalDFA *min_dfa);

bool distinguishable(int i, int j, MinimalDFA *min_dfa, DFA *dfa);
int quotient(MinimalDFA *min_dfa, DFA *dfa);
MinimalDFA *construct_minimal_states(MinimalDFA *min_dfa, DFA *dfa);
MinimalDFA *construct_transition_table(MinimalDFA *min_dfa, DFA *dfa);
MinimalDFA *minimize(DFA *dfa);

int gen_minimal_dfa_graphviz(MinimalDFA *min_dfa, const char *file_name);

#endif