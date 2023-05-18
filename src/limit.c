#include <sys/resource.h>

#include <die.h>
#include <limit.h>
#include <stdio.h> /* perror */

/* Set the soft limit for a resource. */
static void set_limit(int resource, unsigned long n)
	{
	struct rlimit rlim;

	if (getrlimit(resource,&rlim) < 0)
		{
		perror("getrlimit");
		die(0);
		}

	rlim.rlim_cur = n;

	if (setrlimit(resource,&rlim) < 0)
		{
		perror("setrlimit");
		die(0);
		}
	}

/* Set the CPU time limit in seconds. */
void limit_time(unsigned long n)
	{
	set_limit(RLIMIT_CPU,n);
	}

/* Set the maximum size of the process stack in bytes. */
void limit_stack(unsigned long n)
	{
	set_limit(RLIMIT_STACK,n);
	}

/* Set the maximum size of the process' virtual memory (address space) in
bytes. */
void limit_memory(unsigned long n)
	{
	set_limit(RLIMIT_AS,n);
	}
