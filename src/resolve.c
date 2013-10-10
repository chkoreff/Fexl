#include <stdio.h>
#include <string.h>
#include "die.h"
#include "file.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "double.h"
#include "fexl.h"
#include "form.h"
#include "lib.h"
#include "long.h"
#include "parse.h"
#include "qfile.h"
#include "qstr.h"
#include "resolve.h"

static void report_undef(value sym)
	{
	char *name = get_str(sym->L)->data;
	long line = get_long(sym->R->R);

	warn("Undefined symbol %s on line %ld%s%s", name, line,
		source_name[0] ? " of " : "",
		source_name
		);
	}

value type_base_path(value f)
	{
	return Qstr(base_path());
	}

/* This is the core context needed to bootstrap a larger context in Fexl. */
static value context(char *name)
	{
	if (strcmp(name,"dlopen") == 0) return Q(type_dlopen);
	if (strcmp(name,"dlsym") == 0) return Q(type_dlsym);
	if (strcmp(name,"Q") == 0) return Q(type_Q);
	if (strcmp(name,"base_path") == 0) return Q(type_base_path);
	if (strcmp(name,"source_file") == 0) return Qfile(source_fh);
	if (strcmp(name,"source_name") == 0) return Qstr0(source_name);
	if (strcmp(name,"source_line") == 0) return Qlong(source_line);

	/* Integer number (long) */
	{
	long num;
	if (string_long(name,&num)) return Qlong(num);
	}

    /* Floating point number (double) */
	{
	double num;
	if (string_double(name,&num)) return Qdouble(num);
	}

	return 0;
	}

/* Resolve all symbols in form f with the core context, reporting any undefined
symbols to stderr. */
value resolve(value f)
	{
	hold(f);
	value sym = first_symbol(f);
	if (sym == 0) return f;

	value def = 0;

	value name = sym->L;
	if (sym->R->L->T != type_C)
		{
		def = context(get_str(name)->data);
		if (!def)
			{
			report_undef(sym);
			def = sym;
			}
		}
	else
		def = name; /* string literal */

	value form = abstract(sym,f);
	value resolved = resolve(form);
	value result = apply(resolved,def);

	hold(result);
	drop(form);
	drop(resolved);
	drop(f);
	return result;
	}

/* (pop_symbol format)
Get the first symbol in the form, in left to right order.  Return end if there
is no symbol, otherwise return (item sym new_form), where sym is the symbol,
and new_form is the form with sym abstracted from it.
*/
value type_pop_symbol(value f)
	{
	if (!f->L) return f;

	value form = eval(f->R);
	value sym = first_symbol(form);

	value g;

	if (sym == 0)
		g = C;
	else
		{
		value new_form = abstract(sym,form);
		g = item(sym,new_form);
		drop(new_form);
		}

	drop(form);
	return g;
	}

/* (look_symbol sym)  Look at the symbol and return:
	(\: : label quoted line)

where:
	label is the string label.
	quoted is true if it's a quoted string, or false if it's a name.
	line is the line number.
*/
value type_look_symbol(value f)
	{
	if (!f->L) return f;

	value x = eval(f->R);
	if (x->T != type_form) bad_type();
	data_type(x->L,type_string);

	value label = x->L;
	value quoted = x->R->L;
	value line = x->R->R;

	value g = yield(yield(yield(I,label),quoted),line);
	drop(x);
	return g;
	}
