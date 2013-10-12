#include <dlfcn.h>
#include <stdio.h>
#include "str.h"

#include "value.h"
#include "basic.h"
#include "double.h"
#include "form.h"
#include "long.h"
#include "parse.h"
#include "qstr.h"
#include "resolve.h"

static void *find_symbol(const char *prefix0, struct str *label)
	{
	struct str *prefix = str_new_data0(prefix0);
	struct str *full_name = str_append(prefix, label);

	void *def = dlsym(NULL, full_name->data);

	str_free(prefix);
	str_free(full_name);

	return def;
	}

/* This is the core context (environment) for Fexl. */
static value context(struct str *label)
	{
	char *name = label->data;

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

	/* Look up type_<name>. */
	{
	value (*fn)(value) = find_symbol("type_",label);
	if (fn) return Q(fn);
	}

	/* Look up const_<name>. */
	{
	value (*fn)(void) = find_symbol("const_",label);
	if (fn) return fn();
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
	if (sym->R->L->T == type_C)
		def = name; /* quoted string */
	else
		{
		def = context(get_str(name));
		if (!def)
			{
			report_undef(sym);
			def = sym;
			}
		}

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
value type_pop_symbol(value f) /*TODO won't need */
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
value type_look_symbol(value f) /*TODO won't need */
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
