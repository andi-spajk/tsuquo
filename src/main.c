#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	char *file_name = argv[1];
	while (*file_name != '.')
		file_name++;
	int file_name_len = file_name - argv[1];

	// +4 for ".dot" and +1 for \0
	file_name = calloc(file_name_len + 4 + 1, sizeof(char));
	if (!file_name)
		ABORT("fatal memory error\n");
	memcpy(file_name, argv[1], file_name_len);
	memcpy(file_name + file_name_len, ".dot", 4);

	CmpCtrl *cc = init_cmpctrl();
	if (!cc) {
		free(file_name);
		ABORT("fatal memory error\n");
	}

	if (read_file(cc, argv[1]) != 0) {
		free(file_name);
		destroy_cmpctrl(cc);
		ABORT("couldn't open input file\n")
	}

	NFA *nfa = parse(cc);
	if (!nfa || cc->flags & CC_ABORT) {
		free(file_name);
		destroy_cmpctrl(cc);
		ABORT("compilation failed\n");
	}

	DFA *dfa = convert_nfa_to_dfa(nfa);
	if (!dfa) {
		free(file_name);
		destroy_cmpctrl(cc);
		destroy_nfa_and_states(nfa);
		ABORT("DFA construction failed\n");
	}

	MinimalDFA *min_dfa = minimize(dfa);
	if (!min_dfa) {
		free(file_name);
		destroy_cmpctrl(cc);
		destroy_nfa_and_states(nfa);
		destroy_dfa(dfa);
		ABORT("DFA minimization failed\n");
	}
	gen_minimal_dfa_graphviz(min_dfa, file_name);
	printf("success: produced file '%s'\n", file_name);

	free(file_name);
	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	return 0;
}