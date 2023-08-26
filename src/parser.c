/** parser.c

Parse regular expressions and build a Thompson NFA representation.

My definitions that probably aren't standard
	Pattern: a sequence of literal/escaped ASCII characters, plus
	         possible quantifiers
	Group: a parenthetical regular expression, a sequence of 1 or more
	patterns, or a sequence of 1 or more ranges

*/

#include <stddef.h>

#include "common.h"
#include "control.h"
#include "lexer.h"
#include "nfa.h"
#include "parser.h"

/* parse()
	@cc             ptr to CmpCtrl struct

	@return         NFA representation of a regex, NULL if fail

	Parse a regular expression and build a Thompson NFA representation of
	it.
*/
NFA *parse(CmpCtrl *cc)
{
	NFA *god;
	lex(cc);
	if ((god = regex(cc))) {
		if (cc->token == TK_EOF)
			return god;
	}
	return NULL;
}

NFA *regex(CmpCtrl *cc)
{
	NFA *local;
	if ((local = group(cc))) {
		// gprime() modifies local, so no need to assign the return val
		// to anything
		if (gprime(cc, local))
			return local;
		else if (cc->token == TK_RPAREN)
			return local;
		else
			print_error(cc, "expected '|', ')', or pattern");
	}
	return NULL;
}

NFA *group(CmpCtrl *cc)
{
	NFA *g;
	if (cc->token == TK_LPAREN) {
		lex(cc);
		if (!(g = regex(cc)))
			return NULL;
		if (cc->token != TK_RPAREN) {
			print_error(cc, "expected ')'");
			return NULL;
		}
		lex(cc);
		return g = quantifier(cc, g);
	} else if ((g = pattern(cc))) {
		return gprime(cc, g);
	}// else if (g = range())
	return NULL;
}

NFA *gprime(CmpCtrl *cc, NFA *local)
{
	NFA *g;
	if ((g = group(cc))) {
		local = nfa_append(local, g);
		return gprime(cc, local);
	} else if (cc->token == TK_PIPE) {
		lex(cc);
		if ((g = group(cc))) {
			local = nfa_union(local, g);
			return gprime(cc, local);
		}
	} else if (cc->token == TK_EOF || cc->token == TK_RPAREN) {
		return local;
	}
	return NULL;
}

NFA *quantifier(CmpCtrl *cc, NFA *nfa)
{
	U8 q;
	switch (cc->token) {
	case TK_STAR:     q = '*'; break;
	case TK_QUESTION: q = '?'; break;
	case TK_PLUS:     q = '+'; break;
	default: q = '\0'; break;
	}
	if (q) {
		nfa = transform(nfa, q);
		lex(cc);
		return nfa;
	} else if (cc->token <= TK_PIPE) {
		// ASCII, EOF, (, ), [, |
		return nfa;
	}
	print_error(cc, "expected '(', ')', '[', '|', or pattern");
	return NULL;
}

NFA *pattern(CmpCtrl *cc)
{
	NFA *final_nfa = NULL;
	NFA *thompson;
	while (cc->token < 128) {
		thompson = init_thompson_nfa(cc->token);
		if (!thompson) {
			print_error(cc, "FATAL MEMORY ERROR");
			return NULL;
		}
		lex(cc);
		if (!quantifier(cc, thompson)) {
			destroy_nfa_and_states(final_nfa);
			destroy_nfa_and_states(thompson);
			return NULL;
		}
		final_nfa = nfa_append(final_nfa, thompson);
	}
	if (TK_EOF <= cc->token && cc->token <= TK_PIPE) {
		// EOF, (, ), [, |
		return final_nfa;
	} else {
		print_error(cc, "expected '(', ')', '[', '|', or pattern");
		destroy_nfa_and_states(final_nfa);
		return NULL;
	}
	return NULL;
}