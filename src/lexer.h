/** lexer.h

Module definition for lexical analysis.

*/

#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

// tokens for special regex chars like ( * ? ] etc.
#define TK_EOF          128
#define TK_LPAREN       129
#define TK_RPAREN       130
#define TK_LBRACKET     131
#define TK_PIPE         132
#define TK_STAR         133
#define TK_QUESTION     134
#define TK_PLUS         135
#define TK_RBRACKET     136
#define TK_ILLEGAL      137
/*
If the user wants to literally match them like \( \) \* etc. then the plain
ASCII value will function as the token.

Thus these token values are easily used in the NFAs/DFAs to indicate transition
chars.

Full character mapping:
        0: indicates epsilon transition (NOT a token)
  [1,127]: regular ASCII (what the user enters)
[128,255]: special tokens
*/

typedef uint8_t U8;

typedef struct Lexer {
	char *buffer;
	int pos;
	int buffer_len;
} Lexer;

Lexer *init_lexer(void);
void destroy_lexer(Lexer *lexer);

int read_file(Lexer *lexer, const char *file_name);
U8 get_char(Lexer *lexer);
U8 lex(Lexer *lexer);

void print_error(const Lexer *lexer, const char *msg);

#endif