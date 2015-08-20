#include <die.h>
#include <output.h>
#include <report.h>

const char *source_label; /* name of current source file */

static void put_error_location(unsigned long line)
	{
	put(2," on line "); put_ulong(2,line);
	if (source_label[0])
		{
		put(2," of ");put(2,source_label);
		}
	nl(2);
	}

void syntax_error(const char *code, unsigned long line)
	{
	put(2,code); put_error_location(line);
	die(0);
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	put(2,"Undefined symbol "); put(2,name); put_error_location(line);
	}
