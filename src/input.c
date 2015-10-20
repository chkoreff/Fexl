#include <input.h>
#include <stdio.h>

void *cur_in;

static int getd_file(void)
	{
	return fgetc(cur_in);
	}

input getd;

void get_from(void *in)
	{
	getd = getd_file;
	cur_in = in;
	}
