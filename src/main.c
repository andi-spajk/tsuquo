#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "lexer.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "invalid cmdline args\n");
		return EXIT_FAILURE;
	}
	return 0;
}