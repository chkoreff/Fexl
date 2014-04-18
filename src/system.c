#include <value.h>
#include <stdio.h>
#include <basic.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <system.h>
#include <type_long.h>
#include <type_str.h>

int argc;
char **argv;
char **envp;

FILE *source_fh;
const char *source_label;
int source_line;

value type_argv(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long i = get_long(x);
	value z = Qstr0(i >= 0 && i < argc ? argv[i] : "");
	drop(x);
	return z;
	}

/*TODO env*/

value type_exit(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long status = get_long(x);
	drop(x);
	exit(status);
	return I;
	}

/* (setrlimit resource soft hard) = status */
value type_setrlimit(value f) /*TODO test*/
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;

	value x = eval(f->L->L->R);
	value y = eval(f->L->R);
	value z = eval(f->R);

	long resource = get_long(x);
	struct rlimit rlim;
	rlim.rlim_cur = get_long(y);
	rlim.rlim_max = get_long(z);

	int status = setrlimit(resource, &rlim);

	drop(x);
	drop(y);
	drop(z);
	return Qlong(status);
	}

/*LATER more functions
getenv
setenv
*/
