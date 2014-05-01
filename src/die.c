#include <output.h>

#include <die.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	beg_error();
	if (msg[0])
		{
		put(msg);nl();
		}
	exit(1);
	}
