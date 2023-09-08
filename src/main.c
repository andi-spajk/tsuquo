#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "control.h"
#include "dfa.h"
#include "lexer.h"
#include "minimize.h"
#include "nfa.h"
#include "parser.h"

#define ABORT(exit_msg) { \
	fprintf(stderr, (exit_msg)); \
	return EXIT_FAILURE; \
}

int main(int argc, char **argv)
{
	if (argc != 2)
		ABORT("invalid cmdline args\n");

	CmpCtrl *cc = init_cmpctrl();
	if (!cc)
		ABORT("fatal memory error\n");

	if (read_file(cc, argv[1]) != 0) {
		destroy_cmpctrl(cc);
		ABORT("couldn't open input file\n")
	}

	NFA *nfa = parse(cc);
	if (!nfa) {
		destroy_cmpctrl(cc);
		ABORT("compilation failed\n");
	}

	DFA *dfa = convert_nfa_to_dfa(nfa);
	if (!dfa) {
		destroy_cmpctrl(cc);
		destroy_nfa_and_states(nfa);
		ABORT("DFA construction failed\n");
	}

	MinimalDFA *min_dfa = minimize(dfa);
	if (!min_dfa) {
		destroy_cmpctrl(cc);
		destroy_nfa_and_states(nfa);
		destroy_dfa(dfa);
		ABORT("DFA minimization failed\n");
	}
	gen_minimal_dfa_graphviz(min_dfa, "min.dot");
	if (system("dot -Tsvg min.dot -o min.svg") != 0)
		printf("failure\n");
	else
		printf("success\n");

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	return 0;
}