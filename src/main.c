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

#define ABORT(file_name, cc, nfa, dfa, min_dfa, exit_msg) \
	do { \
		free((file_name)); \
		destroy_cmpctrl((cc)); \
		destroy_nfa_and_states((nfa)); \
		destroy_dfa((dfa)); \
		destroy_minimal_dfa((min_dfa)); \
		fprintf(stderr, (exit_msg)); \
		return EXIT_FAILURE; \
	} while (0);

int main(int argc, char **argv)
{
	char *file_name = NULL;
	CmpCtrl *cc = NULL;
	NFA *nfa = NULL;
	DFA *dfa = NULL;
	MinimalDFA *min_dfa = NULL;

	if (argc != 2)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "invalid cmdline args\n");

	char *begin = argv[1];
	char *true_begin = begin + strlen(begin);
	while (*true_begin != '.')
		true_begin--;

	char *true_end = true_begin;
	while (true_begin > begin) {
		true_begin--;
		if (*true_begin == '/' || *true_begin == '\\') {
			true_begin++;
			break;
		}
	}
	int len = true_end - true_begin;

	// +5 for "dots/"
	// +4 for ".dot"
	// +1 for \0
	file_name = calloc(len + 10, 1);
	if (!file_name)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "fatal memory error\n");
	memcpy(file_name, "dots/", 5);
	memcpy(file_name + 5, true_begin, len);
	memcpy(file_name + 5 + len, ".dot", 4);

	cc = init_cmpctrl();
	if (!cc)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "fatal memory error\n");
	if (read_file(cc, argv[1]) != 0)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "couldn't open input file\n")

	nfa = parse(cc);
	if (!nfa || cc->flags & CC_ABORT)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "compilation failed\n");

	dfa = convert_nfa_to_dfa(nfa);
	if (!dfa)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "DFA construction failed\n");

	min_dfa = minimize(dfa);
	if (!min_dfa)
		ABORT(file_name, cc, nfa, dfa, min_dfa, "DFA minimization failed\n");
	gen_minimal_dfa_graphviz(min_dfa, file_name);
	printf("success: produced file '%s'\n", file_name);

	free(file_name);
	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	return 0;
}