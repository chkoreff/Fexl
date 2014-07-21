#include <die.h>
#include <memory.h>
#include <output.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	put_to_error();
	put(msg);
	nl();
	exit(1);
	}
