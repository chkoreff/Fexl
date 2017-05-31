#include <stdio.h>

#include <die.h>
#include <file.h>
#include <report.h>

static void put_error_location(unsigned long line, const char *label)
	{
	fput(stderr," on line "); fput_ulong(stderr,line);
	if (label[0])
		{
		fput(stderr," of ");fput(stderr,label);
		}
	fnl(stderr);
	}

void fatal_error(const char *code, unsigned long line, const char *label)
	{
	fput(stderr,code); put_error_location(line,label);
	die(0);
	}

void undefined_symbol(const char *name, unsigned long line, const char *label)
	{
	fput(stderr,"Undefined symbol "); fput(stderr,name);
	put_error_location(line,label);
	}
