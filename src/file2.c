#include <num.h>
#include <stdio.h>
#include <str.h>

#include <file.h>
#include <file2.h>

void fput_str(FILE *fh, string x)
	{
	fputd(fh,x->data,x->len);
	}

void fput_num(FILE *fh, number x)
	{
	fput_double(fh,*x);
	}
