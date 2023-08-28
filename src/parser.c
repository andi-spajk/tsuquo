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
	cc->flags = 0;
	NFA *god;
	lex(cc);
	if ((god = regex(cc))) {
		if (!(cc->flags & CC_ABORT)) {
			if (cc->token == TK_EOF)
				return god;
			else
				print_error(cc, "expected end of regex");
		}
	}
	destroy_nfa_and_states(god);
	return NULL;
}

NFA *regex(CmpCtrl *cc)
{
	NFA *local;
	if ((local = group(cc))) {
		// gprime() modifies `local`'s members, so no need to assign
		// the return val to anything
		if (gprime(cc, local))
			return local;
		else if (cc->flags & CC_ABORT)  // did gprime set ABORT flag?
			return NULL;
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
		if (!(g = regex(cc))) {
			cc->flags |= CC_DISABLE_INSTEAD_FOUND;
			print_error(cc, "malformed regex");
			return NULL;
		}
		if (cc->token != TK_RPAREN) {
			print_error(cc, "expected ')'");
			destroy_nfa_and_states(g);
			cc->flags |= CC_DISABLE_ERROR_MSG;
			return NULL;
		}
		lex(cc);
		return g = quantifier(cc, g);
	} else if ((g = pattern(cc))) {
		return gprime(cc, g);
	} else if ((g = range(cc))) {
		return g = quantifier(cc, g);
	}
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
		} else {
			print_error(cc, "expected '(', '[', or pattern");
			destroy_nfa_and_states(g);
			destroy_nfa_and_states(local);
			cc->flags |= CC_DISABLE_ERROR_MSG;
			// regexes like (a|) will fail gprime()
			// but then we propagate back to regex(), which would
			// match RPAREN
			// that's incorrect, so gprime() sets an abort flag
			// when PIPE fails to be parsed
			cc->flags |= CC_ABORT;
			return NULL;
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
	default:          q = '\0'; break;
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
			cc->flags |= CC_DISABLE_LINE_PRINT;
			print_error(cc, "!!!FATAL MEMORY ERROR!!!");
			// all subsequent errors will be meaningless
			cc->flags |= CC_DISABLE_ERROR_MSG;
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
		print_error(cc, "expected pattern, '(', '[', or end of regex");
		destroy_nfa_and_states(final_nfa);
		// try to minimize redundant messages when we propagate up the
		// call stack
		cc->flags |= CC_DISABLE_ERROR_MSG;
		return NULL;
	}
}

NFA *range(CmpCtrl *cc)
{
	NFA *r;
	if (cc->token == TK_LBRACKET) {
		lex(cc);
		if (!(r = allowed(cc)))
			return NULL;
		if (cc->token == TK_RBRACKET) {
			lex(cc);
			return r;
		}
		print_error(cc, "expected ']'");
		destroy_nfa_and_states(r);
	}
	return NULL;
}

// if you want to match - then put it right after the opening bracket
//   eg [-a] matches '-' or 'a'
NFA *allowed(CmpCtrl *cc)
{
	NFA *result = NULL;
	NFA *segment;
	U8 left, right;
	if (cc->token > '~') {
		print_error(cc, "expected ASCII or escape char");
		return NULL;
	}
	while (cc->token <= '~') {
		left = cc->token;
		lex(cc);
		if (cc->token == '-') {
			lex(cc);
			if (cc->token > '~') {
				print_error(cc, "expected ASCII or escape char");
				destroy_nfa_and_states(result);
				return NULL;
			}
			if (cc->token < left) {
				cc->flags |= CC_DISABLE_INSTEAD_FOUND;
				print_error(cc, "range's upper bound exceeds left bound");
				destroy_nfa_and_states(result);
				return NULL;
			}
			right = cc->token;
			lex(cc);
			segment = init_range_nfa(left, right);
		} else {
			segment = init_thompson_nfa(left);
		}
		result = nfa_union(result, segment);
	}
	return result;
}