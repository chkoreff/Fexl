#include <die.h>
#include <output.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	if (msg)
		{
		put(2,msg);
		nl(2);
		}
	exit(1);
	}
