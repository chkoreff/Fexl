#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "die.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "double.h"
#include "form.h"
#include "long.h"
#include "parse.h"
#include "qfile.h"
#include "qstr.h"
#include "resolve.h"

static void *find_symbol(const char *prefix0, struct str *name)
	{
	struct str *prefix = str_new_data0(prefix0);
	struct str *full_name = str_append(prefix, name);

	void *def = dlsym(NULL, full_name->data);

	str_free(prefix);
	str_free(full_name);

	return def;
	}

/* This is the core context (environment) for Fexl. */
static value define_name(struct str *name)
	{
	/* Integer number (long) */
	{
	long num;
	if (string_long(name->data,&num)) return Qlong(num);
	}

    /* Floating point number (double) */
	{
	double num;
	if (string_double(name->data,&num)) return Qdouble(num);
	}

	/* Look up fexl_<name>. */
	{
	value (*fn)(value) = find_symbol("fexl_",name);
	if (fn) return Q(fn);
	}

	/* Look up const_fexl_<name>. */
	{
	value (*fn)(void) = find_symbol("const_fexl_",name);
	if (fn) return fn();
	}

	if (strcmp(name->data,"@") == 0) return Y;

	return 0;
	}

/* (define_name name) Looks up name in the core context and returns (yes val)
or no. */
value fexl_define_name(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value def = define_name(get_str(x));
	value result = maybe(def);
	drop(x);
	return result;
	}

static void report_undef(value sym)
	{
	const char *kind = (sym->R->L->T == fexl_C) ? "string" : "symbol";
	const char *name = get_str(sym->L)->data;
	long line = get_long(sym->R->R);

	warn("Undefined %s %s on line %ld%s%s", kind, name, line,
		curr_name[0] ? " of " : "",
		curr_name
		);
	}

static value curr_define_string;
static value curr_define_name;
static int curr_strict;

static value do_resolve(value form)
	{
	value sym = first_symbol(form);
	if (sym == 0) return form;

	value define = (sym->R->L->T == fexl_C)
		? curr_define_string : curr_define_name;
	value name = sym->L;

	value def = eval(A(A(A(define,name),C),yes));

	value fn;
	if (def->L == yes)
		fn = def->R;
	else
		{
		fn = sym;
		if (curr_strict)
			report_undef(sym);
		if (def != C)
			bad_type();
		}

	value new_form = abstract(sym,form);
	value resolved = do_resolve(new_form);
	value result = apply(resolved,fn);

	drop(new_form);
	drop(def);

	return result;
	}

static value resolve(value form, value define_string, value define_name,
	value source_name, value strict)
	{
	form = eval(form);
	curr_define_string = eval(define_string);
	curr_define_name = eval(define_name);

	source_name = eval(source_name);
	curr_name = get_str(source_name)->data;

	strict = eval(A(A(strict,C),F));
	if (strict != C && strict != F)
		bad_type();
	curr_strict = (strict == C);

	value result = do_resolve(form);
	if (curr_strict && result->T == type_form)
		die(0); /* Form has undefined symbols. */

	drop(form);
	drop(curr_define_string);
	drop(curr_define_name);
	drop(source_name);
	drop(strict);

	return result;
	}

/* (resolve form define_string define_name source_name strict) */
value fexl_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L
		|| !f->L->L->L->L->L) return f;
	return A(later,resolve(f->L->L->L->L->R, f->L->L->L->R, f->L->L->R,
		f->L->R, f->R));
	}

static value resolve_source(const char *name)
	{
	if (strcmp(name,"source_file") == 0) return Qfile(curr_fh);
	if (strcmp(name,"source_name") == 0) return Qstr0(curr_name);
	if (strcmp(name,"source_line") == 0) return Qlong(curr_line);
	return 0;
	}

static value standard_name(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	struct str *name = get_str(x);

	value def = define_name(name);
	if (def == 0)
		def = resolve_source(name->data);

	value result = maybe(def);
	drop(x);
	return result;
	}

/* Resolve the form in the standard context. */
value resolve_standard(value form)
	{
	return resolve(form,yes,Q(standard_name),Qstr0(curr_name),C);
	}
