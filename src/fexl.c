#include "die.h"
#include "value.h"
#include "basic.h"
#include "file.h"
#include "long.h"
#include "parse_file.h"
#include "resolve.h"
#include "run.h"
#include "string.h"

/*
Parse the top Fexl configuration file "base/share/fexl/main.fxl", where "base"
is the base directory of the running program.

Check the result of the parse, resolving all open symbols with the standard
"resolve" function.  Report any errors, including syntax errors and undefined
symbols.  Return an evaluable function if all went well, or 0 otherwise.
*/
static value parse_top(void)
	{
	value full_path = get_full_path("share/fexl/main.fxl");
	hold(full_path);

	if (string_len(full_path) == 0)
		{
		warn("Can't locate the base path for the top level program.");
		drop(full_path);
		return 0;
		}

	char *name = string_data(full_path);
	value result = parse_file(name);
	hold(result);

	int ok = result->L->T == reduce_C;
	value exp = result->R->L;

	if (!ok)
		{
		long line = get_long(exp->R);
		if (line == 0)
			warn("Can't open file %s", name);
		else
			warn("%s on line %ld of %s", string_data(exp->L), line, name);
		}

	value symbols = result->R->R;

	while (symbols->T != reduce_C)
		{
		value entry = symbols->L;
		value sym = entry->L;
		value def = resolve(sym);

		if (def)
			exp = A(exp,def);
		else
			{
			long line = get_long(entry->R);
			warn("Undefined symbol %s on line %ld of %s", string_data(sym),
				line, name);
			ok = 0;
			}

		symbols = symbols->R;
		}

	drop(full_path);
	drop(result);

	if (!ok)
		{
		hold(exp);
		drop(exp);
		exp = 0;
		}

	return exp;
	}

/*
This is the main routine of the Fexl interpreter.  The exit status is 0 for
success or non-zero for failure.

It parses the top Fexl configuration script and evaluates that function.  That
function defines the standard context of predefined function definitions, then
runs the script specified on the command line in that standard context.  This
is how we configure Fexl using Fexl itself.
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);

	value exp = parse_top();

	hold(exp);
	if (exp) eval(exp);
	drop(exp);

	end_value();
	return exp ? 0 : 1;
	}
