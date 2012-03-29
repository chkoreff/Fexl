#include "value.h"
#include "basic.h"
#include "eval.h"
#include "parse_file.h"
#include "run.h"

/*
This is the main routine of the Fexl interpreter.  The exit status is 0 for
success, or 1 for failure.

It parses the file named on the command line, or stdin if no name is given.
That yields a Fexl function which is applied to the "world" and then evaluated.
In this case, the "world" is simply the identity function (I).
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);
	eval(A(parse_file(argc > 1 ? argv[1] : 0), Q(fexl_I)));
	end_value();
	return 0;
	}
