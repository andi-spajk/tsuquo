/** control.c

Compiler control stores all necessary info for each phase of the program.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	FILE *f = fopen(file_name, "rb");
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
	if (fread(cc->buffer, 1, fsize, f) != (size_t)fsize)
		return -1;

	cc->buffer_len = fsize;
	// don't forget to reset position
	cc->pos = 0;

	fclose(f);
	return 0;
}

/* read_line()
	@cc             ptr to CmpCtrl struct
	@line           regex string
	@size           length of string, EXCLUDING null terminator

	@return         0 if line is successfully read, otherwise -1

	Read a string of length @size, which contains a regular expression,
	into the CmpCtrl's internal buffer. If the buffer already holds data,
	it is freed and overwritten with new data.
*/
int read_line(CmpCtrl *cc, const char *line, const int size)
{
	free(cc->buffer);
	cc->buffer = malloc(size);
	if (!cc->buffer)
		return -1;
	memcpy(cc->buffer, line, size);
	cc->buffer_len = size;
	cc->pos = 0;
	return 0;
}
