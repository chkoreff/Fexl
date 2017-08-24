#include <value.h>

#include <standard.h>

#if DEV
#include <stdio.h>

#include <file.h>
#include <memory.h>

static void show_stats(void)
	{
	fnl(stderr);
	fput(stderr,"num_steps = ");fput_ulong(stderr,num_steps);fnl(stderr);
	fput(stderr,"max_bytes = ");fput_ulong(stderr,max_bytes);fnl(stderr);
	fput(stderr,"count_V = ");fput_ulong(stderr,count_V);fnl(stderr);
	}
#endif

/* Evaluate the named file.  Use stdin if the name is missing or empty.  If the
name designates a directory it behaves like an empty file. */
int main(int argc, char *argv[])
	{
	eval_standard(argc,argv);

	#if DEV
	if (0) show_stats();
	#endif

	return 0;
	}
