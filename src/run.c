#include "run.h"

/* Command line arguments */
int main_argc;
char **main_argv;
char **main_envp;
int main_exit;
char *error;

long line = 1; /* line number */

/*
Limit the depth of recursive routines to avoid segmentation fault caused by
stack overflow.  Unfortunately at this point I have to guess a "reasonably high
value" for max_depth, which can vary between machines.  For example, parsing an
infinite stream of '(' characters triggers a segfault at depth 74772 on this
machine.  Later I might be able to do some magic with getrlimit and getrusage
to make it more portable, or possibly even catching the SIGSEGV signal.
*/
long max_depth = 50000;
long cur_depth = 0;

/*
Limit the number of steps the program can run.  If max_steps is negative, there
is no limit; otherwise the program will terminate after max_steps steps.
*/
long max_steps = -1;  /* Allow infinite loop by default. */
long cur_steps = 0;

void beg_run(int argc, char *argv[], char *envp[])
	{
	main_argc = argc;
	main_argv = argv;
	main_envp = envp;
	}
