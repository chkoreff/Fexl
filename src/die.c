#include <stdio.h>
#include <stdlib.h> // exit

#include <die.h>

void die(const char *msg)
	{
	if (msg)
		fprintf(stderr,"%s\n",msg);
	exit(1);
	}
