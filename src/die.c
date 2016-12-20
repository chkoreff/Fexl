#include <stdio.h>

#include <die.h>
#include <file.h>
#include <stdlib.h> /* exit */

void die(const char *msg)
	{
	if (msg)
		{
		fput(stderr,msg);fnl(stderr);
		}
	exit(1);
	}
