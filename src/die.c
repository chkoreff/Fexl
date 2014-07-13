#include <die.h>
#include <output.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	if (msg[0])
		{
		put_to_error();
		put(msg);nl();
		}
	exit(1);
	}
