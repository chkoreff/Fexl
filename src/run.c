#include "run.h"

/* Command line arguments */
int main_argc;
char **main_argv;

static int cache_envc = -1; /* auto-initialized count of envp entries */
char **main_envp;

void beg_run(int argc, char *argv[], char *envp[])
	{
	main_argc = argc;
	main_argv = argv;
	main_envp = envp;
	}

int main_envc(void)
	{
	if (cache_envc < 0)
		{
		cache_envc = 0;
		if (main_envp != 0)
			while (1)
				{
				char *s = main_envp[cache_envc];
				if (s == 0) break;
				cache_envc++;
				}
		}

	return cache_envc;
	}
