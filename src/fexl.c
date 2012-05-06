#include "base.h"
#include "buf.h"
#include "die.h"
#include "memory.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "parse_file.h"
#include "resolve.h"
#include "run.h"
#include "string.h"

/*
Parse the top Fexl configuration file "base/share/fexl/main.fxl", where "base"
is the base directory set by the build script:  either `pwd` when building
locally, or "/usr" when doing a final install.

Check the result of the parse, resolving all open symbols with the standard
"resolve" function.  Report any errors, including syntax errors and undefined
symbols.  Return an evaluable function if all went well, or 0 otherwise.
*/
static value parse_top(void)
	{
	char *top = "share/fexl/main.fxl";

	struct buf *buf = 0;
	buf_add_string(&buf, base);
	buf_add(&buf,'/');
	buf_add_string(&buf, top);
	long len;
	char *name = buf_clear(&buf,&len);

	value result = parse_file(name);

	hold(result);

	int ok = result->L->R->T == fexl_C;
	value f = result->R->L->R;

	if (!ok)
		{
		long line = get_long(f->R);
		if (line == 0)
			warn("Can't open configuration script %s", name);
		else
			warn("%s on line %ld of %s", string_data(f->L->R), line, name);
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
			long line = get_long(entry->R);
			warn("Undefined symbol %s on line %ld of %s", string_data(sym),
				line, name);
			ok = 0;
			}

		symbols = symbols->R;
		}

	free_memory(name, len+1);
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
success or non-zero for failure.

It parses the top Fexl configuration script and evaluates that function.  That
function defines the standard context of predefined function definitions, then
runs the script specified on the command line in that standard context.  This
is how we configure Fexl using Fexl itself.
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);

	value f = parse_top();

	hold(f);
	if (f) eval(f);
	drop(f);

	end_value();
	return f ? 0 : 1;
	}
