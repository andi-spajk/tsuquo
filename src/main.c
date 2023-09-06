#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "control.h"
#include "dfa.h"
#include "lexer.h"
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
	index_states(nfa);

	DFA *dfa = convert_nfa_to_dfa(nfa);
	if (!dfa) {
		destroy_cmpctrl(cc);
		destroy_nfa_and_states(nfa);
		ABORT("DFA construction failed\n");
	}
	gen_dfa_graphviz(dfa, "dfa.dot", false);

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	printf("success\n");
	return 0;
}