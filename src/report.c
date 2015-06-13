#include <die.h>
#include <output.h>
#include <report.h>

const char *source_label; /* name of current source file */

static void put_error_location(unsigned long line)
	{
	put(" on line "); put_ulong(line);
	if (source_label[0])
		{
		put(" of ");put(source_label);
		}
	nl();
	}

void syntax_error(const char *code, unsigned long line)
	{
	put_to_error();
	put(code); put_error_location(line);
	die(0);
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	put_to_error();
	put("Undefined symbol "); put(name); put_error_location(line);
	put_to_output();
	}
