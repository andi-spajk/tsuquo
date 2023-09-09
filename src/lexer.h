/** lexer.h

Module definition for the lexical analyzer.

*/

#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "control.h"

// tokens for special regex chars like . ( * ? ] etc.
#define TK_WILDCARD     127  // replaces ASCII 127 = DEL
                             // who tf gonna match the DELETE char anyway???
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
  [1,127]: regular ASCII or . (what the user enters)
[128,255]: special tokens
*/

U8 get_char(CmpCtrl *cc);
U8 lex(CmpCtrl *cc);
void print_error(const CmpCtrl *cc, const char *msg);

#endif