/** lexer.c

Lexical analyzer for TSHazer.

*/

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

/* init_lexer()
	@return         ptr to dynamically allocated Lexer struct, NULL if fail

	Dynamically allocate a Lexer struct and initialize its default values.
*/
Lexer *init_lexer(void)
{
	Lexer *lexer = calloc(1, sizeof(Lexer));
	if (!lexer)
		return NULL;
	lexer->buffer = NULL;
	lexer->pos = 0;
	lexer->buffer_len = 0;
	return lexer;
}

/* destroy_lexer()
	@lexer          ptr to Lexer struct

	Free a Lexer and its buffer from memory.
*/
void destroy_lexer(Lexer *lexer)
{
	if (lexer)
		free(lexer->buffer);
	free(lexer);
}

/* read_file()
	@lexer          ptr to Lexer struct
	@file_name      name of file to read

	@return         0 if file is successfully read, otherwise -1

	Read a file which contains a regular expression into the Lexer's
	internal buffer. If the buffer already holds data, it is freed and
	overwritten with new data.
*/
int read_file(Lexer *lexer, const char *file_name)
{
	FILE *f = fopen(file_name, "r");
	if (!f)
		return -1;

	// determine file size in bytes
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	rewind(f);

	free(lexer->buffer);
	lexer->buffer = malloc(fsize);
	if (!lexer->buffer)
		return -1;
	fread(lexer->buffer, 1, fsize, f);

	lexer->buffer_len = fsize;
	// don't forget to reset position
	lexer->pos = 0;

	fclose(f);
	return 0;
}

/* get_char()
	@lexer          ptr to Lexer struct

	@return         next raw character in the buffer

	Read a character from the lexer's buffer and advance the internal state
	to the next character.
*/
U8 get_char(Lexer *lexer)
{
	if (lexer->pos >= lexer->buffer_len)
		return TK_EOF;
	return lexer->buffer[lexer->pos++];
}

/* lex()
	@lexer          ptr to Lexer struct

	@return         next token

	Fetch the next token.
*/
U8 lex(Lexer *lexer)
{
	U8 ch = get_char(lexer);
	if (ch == '\\') {
		ch = get_char(lexer);
		switch (ch) {
		case '(':  // fallthrough
		case ')':
		case '[':
		case '|':
		case '*':
		case '?':
		case '+':
		case ']':
		case '\\':
			return ch;
		case 'n':
			return '\n';
		case 't':
			return '\t';
		default:  // invalid escape sequence
			return TK_ILLEGAL;
		}
	} else {
		switch (ch) {
		case '(': return TK_LPAREN;
		case ')': return TK_RPAREN;
		case '[': return TK_LBRACKET;
		case '|': return TK_PIPE;
		case '*': return TK_STAR;
		case '?': return TK_QUESTION;
		case '+': return TK_PLUS;
		case ']': return TK_RBRACKET;
		default: return ch;
		}
	}
}