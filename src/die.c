#include <die.h>
#include <memory.h>
#include <output.h>
#include <stdlib.h> /* exit */

const char *error_code;

void out_of_memory(void)
	{
	error_code = "The program ran out of memory.";
	}

void die(const char *msg)
	{
	put_to_error();
	put(msg);
	nl();
	exit(1);
	}
