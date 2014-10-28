#include <output.h>
#include <source.h>

const char *source_label; /* name of current source file */

void put_error_location(unsigned long line)
	{
	put(" on line "); put_ulong(line);
	if (source_label && source_label[0])
		{
		put(" of ");put(source_label);
		}
	nl();
	}
