#include "die.h"
#include "value.h"
#include "basic.h"
#include "eval.h"
#include "long.h"
#include "parse_file.h"
#include "resolve.h"
#include "run.h"
#include "string.h"

/* Parse the given file name, or 0 for stdin, and resolve all symbols therein,
returning an evaluable function if all went well, or 0 otherwise.  Report any
errors along the way, including syntax errors and undefined symbols. */
value parse_main(char *name)
	{
	value result = parse_file(name);

	hold(result);

	int ok = result->L->R->T == fexl_C;
	value f = result->R->L->R;

	if (!ok)
		{
		line = get_long(f->R);
		warn(string_data(f->L->R));
		}

	value symbols = result->R->R;

	while (symbols->L)
		{
		value entry = symbols->L->R;
		value sym = entry->L->R;
		value def = resolve(sym);

		if (def)
			f = A(f,def);
		else
			{
			line = get_long(entry->R);
			warn("Undefined symbol %s",string_data(sym));
			ok = 0;
			}

		symbols = symbols->R;
		}

	drop(result);

	if (!ok)
		{
		hold(f);
		drop(f);
		f = 0;
		}

	return f;
	}

/*
This is the main routine of the Fexl interpreter.  The exit status is 0 for
success, or 1 for failure.

It parses the file named on the command line, or stdin if no name is given, and
evaluates that function.
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);

	value f = parse_main(argc > 1 ? argv[1] : 0);

	hold(f);

	if (f)
		eval(f);
	else
		main_exit = 1;

	drop(f);

	end_value();
	return main_exit;
	}
