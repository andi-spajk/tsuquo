/** control.h

Module definition for the compiler control structure.

*/

#ifndef CONTROL_H
#define CONTROL_H

#include "common.h"

// This project is, formally speaking, a compiler.
// No one shall stop me from calling it such.
typedef struct CompilerControl {
	U8 token;
	char *buffer;
	int pos;
	int buffer_len;
} CmpCtrl;

CmpCtrl *init_cmpctrl(void);
void destroy_cmpctrl(CmpCtrl *cc);
int read_file(CmpCtrl *cc, const char *file_name);
int read_line(CmpCtrl *cc, const char *line, const int size);

#endif