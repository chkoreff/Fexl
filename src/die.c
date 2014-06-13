#include <output.h>

#include <die.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	if (msg[0])
		{
		putd = putd_err;
		put(msg);nl();
		}
	exit(1);
	}
