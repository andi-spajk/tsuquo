#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "invalid cmdline args\n");
		return EXIT_FAILURE;
	}
	Lexer *lexer = init_lexer();
	U8 tk;
	if (!read_file(lexer, argv[1])) {
		while ((tk = lex(lexer)) != TK_EOF) {
			switch (tk) {
			case TK_LPAREN:
				printf("TK_LPAREN\n");
				break;
			case TK_RPAREN:
				printf("TK_RPAREN\n");
				break;
			case TK_LBRACKET:
				printf("TK_LBRACKET\n");
				break;
			case TK_PIPE:
				printf("TK_PIPE\n");
				break;
			case TK_STAR:
				printf("TK_STAR\n");
				break;
			case TK_QUESTION:
				printf("TK_QUESTION\n");
				break;
			case TK_PLUS:
				printf("TK_PLUS\n");
				break;
			case TK_RBRACKET:
				printf("TK_RBRACKET\n");
				break;
			case TK_ILLEGAL:
				printf("TK_ILLEGAL\n");
				break;
			case '\n':
				printf("newline\n");
				break;
			case '\t':
				printf("tab\n");
				break;
			default:
				printf("%c\n", tk);
				break;
			}
		}
	}
	destroy_lexer(lexer);
	return 0;
}