/** lexer.c

Lexical analyzer for regular expressions.

*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "control.h"
#include "lexer.h"

/* get_char()
	@cc             ptr to CmpCtrl struct

	@return         next raw character in the buffer

	Read a character from control's buffer and advance the internal state
	to the next character.
*/
U8 get_char(CmpCtrl *cc)
{
	if (cc->pos >= cc->buffer_len)
		return TK_EOF;
	return cc->buffer[cc->pos++];
}

/* lex()
	@cc             ptr to CmpCtrl struct

	@return         next token

	Fetch the next token.
*/
U8 lex(CmpCtrl *cc)
{
	U8 ch = get_char(cc);

	// ignore literal newlines
	while (ch == '\r' || ch == '\n')
		ch = get_char(cc);

	if (ch == '\\') {
		ch = get_char(cc);
		switch (ch) {
		case '(':  // fallthrough
		case ')':
		case '[':
		case '|':
		case '*':
		case '?':
		case '+':
		case ']':
		case '\\': break;
		case 'n': ch = '\n'; break;
		case 't': ch = '\t'; break;
		default: ch = TK_ILLEGAL; break;  // invalid escape sequence
		}
	} else {
		switch (ch) {
		case '(': ch = TK_LPAREN; break;
		case ')': ch = TK_RPAREN; break;
		case '[': ch = TK_LBRACKET; break;
		case '|': ch = TK_PIPE; break;
		case '*': ch = TK_STAR; break;
		case '?': ch = TK_QUESTION; break;
		case '+': ch = TK_PLUS; break;
		case ']': ch = TK_RBRACKET; break;
		default: break;
		}
	}
	cc->token = ch;
	return ch;
}

/* print_error()
	@cc             ptr to CmpCtrl struct
	@msg            string containg an error message

	Print an error message, the original regex, and an error arrow.
*/
void print_error(const CmpCtrl *cc, const char *msg)
{
	if (cc->flags & CC_DISABLE_ERROR_MSG)
		return;

	char str[4] = "\'\'\'";

	printf("ERROR: %s", msg);

	if (!(cc->flags & CC_DISABLE_INSTEAD_FOUND)) {
		printf(", instead found");
		switch (cc->token) {
		case TK_EOF: printf(" end of regex"); break;
		case TK_LPAREN: printf(" '('"); break;
		case TK_RPAREN: printf(" ')'"); break;
		case TK_LBRACKET: printf(" '['"); break;
		case TK_PIPE: printf(" '|'"); break;
		case TK_STAR: printf(" '*'"); break;
		case TK_QUESTION: printf(" '?'"); break;
		case TK_PLUS: printf(" '+'"); break;
		case TK_RBRACKET: printf(" ']'"); break;
		case TK_ILLEGAL: printf(" illegal escape sequence"); break;
		default:
			str[1] = cc->token;
			printf(" %s", str);
		}
	}
	putchar('\n');

	if (cc->flags & CC_DISABLE_LINE_PRINT) {
		putchar('\n');
		return;
	}

	// TODO: arrow is misaligned when there are trailing newlines in the
	// buffer. Probably modify skip variable, or the buffer reader

	U8 ch;
	int skip = 0;
	for (int i = 0; i < cc->buffer_len; i++) {
		ch = cc->buffer[i];
		while (ch == '\r' || ch == '\n') {
			i++;
			skip++;
			ch = cc->buffer[i];
		}
		putchar(ch);
	}
	putchar('\n');

	// align error arrow with spaces, must handle 8-width tabs
	int num_spaces = 0;
	int tab_align = 0;
	// use pos-1 because pos always sits one char ahead of the previously
	// fetched token, which receives the error arrow
	for (int i = 0; i < cc->pos-1-skip; i++) {
		if (tab_align == 8)
			tab_align = 0;

		if (cc->buffer[i] == '\t') {
			for (int j = 0; j < (8 - tab_align); j++)
				num_spaces++;
			tab_align = 0;
		} else {
			num_spaces++;
			tab_align++;
		}
	}

	for (int i = 0; i < num_spaces; i++)
		putchar(' ');

	// if the previous token was EOF, arrow belongs to the space right after
	// the end of everything (at index=pos)
	// but last char is index=pos-1 so putting the arrow there won't point
	// at EOF
	// so add extra space
	if (cc->token == TK_EOF)
		putchar(' ');

	printf("^\n\n");
}