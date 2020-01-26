#include <stdio.h>
#include <str.h>

#include <file.h>
#include <file_str.h>

void fput_str(FILE *fh, string x)
	{
	fputd(fh,x->data,x->len);
	}
