#include "run.h"

/* Command line arguments */
int main_argc;
char **main_argv;
char **main_envp;
int main_exit;

long line = 1; /* line number */

/*
Limit the depth of recursive routines (C routines, not Fexl functions) to avoid
segmentation fault caused by stack overflow.
*/
long max_depth = 100000;  /* a reasonably high value */
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
