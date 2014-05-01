#include <die.h>
#include <stdio.h> /* perror */
#include <sys/resource.h>
#include <test/rlimit.h>

/*
Set the soft limit for a resource, e.g.:

RLIMIT_CPU   Limit CPU time.
RLIMIT_AS    Limit mapped virtual memory.
RLIMIT_DATA  Limit data segment (how far sbrk can go).

NOTE: I haven't yet seen a case where RLIMIT_DATA makes any difference, so it
seems that RLIMIT_AS is the more useful limit.
*/
void set_limit(int resource, rlim_t limit)
	{
	int status;
	struct rlimit rlim;

	status = getrlimit(resource, &rlim);
	if (status != 0)
		{
		perror("getrlimit");
		die("");
		}

	rlim.rlim_cur = limit;
	status = setrlimit(resource, &rlim);
	if (status != 0)
		{
		perror("setrlimit");
		die("");
		}
	}

/*LATER unlimit (restore to hard limit) */
