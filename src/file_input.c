#include <file_input.h>
#include <input.h>
#include <stdio.h>

static FILE *source;

static int get(void)
	{
	return fgetc(source);
	}

void get_from_file(const char *name)
	{
	source = name ? fopen(name,"r") : stdin;
	getd = source ? get : 0;
	}
