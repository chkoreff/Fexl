#include <input.h>
#include <stdio.h>
#include <file_input.h>

static FILE *source;

static int get(void)
	{
	return fgetc(source);
	}

void get_from_file(const char *name, struct file_input *save)
	{
	save->getd = getd;
	save->source = source;

	source = name ? fopen(name,"r") : stdin;
	getd = source ? get : 0;
	}

void restore_file_input(struct file_input *save)
	{
	getd = save->getd;
	source = save->source;
	}
