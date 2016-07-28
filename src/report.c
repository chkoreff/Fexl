#include <stdio.h>
#include <die.h>
#include <output.h>
#include <report.h>

const char *source_label; /* name of current source file */

static void put_error_location(unsigned long line)
	{
	fput(stderr," on line "); fput_ulong(stderr,line);
	if (source_label[0])
		{
		fput(stderr," of ");fput(stderr,source_label);
		}
	fnl(stderr);
	}

void syntax_error(const char *code, unsigned long line)
	{
	fput(stderr,code); put_error_location(line);
	die(0);
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	fput(stderr,"Undefined symbol "); fput(stderr,name);
	put_error_location(line);
	}
