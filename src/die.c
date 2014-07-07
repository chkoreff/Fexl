#include <die.h>
#include <output.h>
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
