/** control.h

Module definition for the compiler control structure.

*/

#ifndef CONTROL_H
#define CONTROL_H

#include "common.h"

#define CC_DISABLE_INSTEAD_FOUND    0x1
#define CC_DISABLE_ERROR_MSG        0x2
#define CC_DISABLE_LINE_PRINT       0x4
#define CC_ABORT                    0x8

// This project is, formally speaking, a compiler.
// No one shall stop me from calling it such.
typedef struct CompilerControl {
	U8 token;
	char *buffer;
	int pos;
	int buffer_len;
	int flags;
	/*
	flags are checked when printing error messages (see lexer.*)
	when these flag bits are set:
	CC_DISABLE_INSTEAD_FOUND 0x1
		disables the error message supplement ", instead found ???"
	CC_DISABLE_ERROR_MSG 0x2
		disables error message printing
	CC_DISABLE_LINE_PRINT 0x4
		disables printing the buffer during an error message
	CC_ABORT 0x8
		end parsing and clean things up, no matter what the parse
		procedures may indicate
	*/
} CmpCtrl;

CmpCtrl *init_cmpctrl(void);
void destroy_cmpctrl(CmpCtrl *cc);
int read_file(CmpCtrl *cc, const char *file_name);
int read_line(CmpCtrl *cc, const char *line, const int size);

#endif