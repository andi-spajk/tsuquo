/** control.c

Compiler control stores all necessary info for each phase of the program.

*/

#include <stdio.h>
#include <stdlib.h>

#include "control.h"

/* init_cmpctrl()
	@return         ptr to dynamically allocated compiler control structure,
	                NULL if fail

	Dynamically allocate a compiler control structure and initialize its
	default values.
*/
CmpCtrl *init_cmpctrl(void)
{
	CmpCtrl *cc = calloc(1, sizeof(CmpCtrl));
	if (!cc)
		return NULL;
	cc->buffer = NULL;
	cc->pos = 0;
	cc->buffer_len = 0;
	return cc;
}

/* destroy_cmpctrl()
	@cc             ptr to CmpCtrl struct

	Free a CmpCtrl and its buffer from memory.
*/
void destroy_cmpctrl(CmpCtrl *cc)
{
	if (cc)
		free(cc->buffer);
	free(cc);
}

/* read_file()
	@cc             ptr to CmpCtrl struct
	@file_name      name of file to read

	@return         0 if file is successfully read, otherwise -1

	Read a file which contains a regular expression into the CmpCtrl's
	internal buffer. If the buffer already holds data, it is freed and
	overwritten with new data.
*/
int read_file(CmpCtrl *cc, const char *file_name)
{
	FILE *f = fopen(file_name, "r");
	if (!f)
		return -1;

	// determine file size in bytes
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	rewind(f);

	free(cc->buffer);
	cc->buffer = malloc(fsize);
	if (!cc->buffer)
		return -1;
	fread(cc->buffer, 1, fsize, f);

	cc->buffer_len = fsize;
	// don't forget to reset position
	cc->pos = 0;

	fclose(f);
	return 0;
}