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

static value case_no;
static value case_yes;
static value curr_define_string;
static value curr_define_name;

static void report_undef(value sym)
	{
	const char *kind = (sym->R->L->T == fexl_C) ? "string" : "symbol";
	const char *name = get_str(sym->L)->data;
	long line = get_long(sym->R->R);

	warn("Undefined %s %s on line %ld%s%s", kind, name, line,
		source_name[0] ? " of " : "",
		source_name
		);
	}

/* This function is called from resolve below. */
static value do_resolve(value form)
	{
	value sym = first_symbol(form);
	if (sym == 0)
		{
		form->N--;
		return form;
		}

	value define = (sym->R->L->T == fexl_C)
		? curr_define_string : curr_define_name;
	value name = sym->L;

	value def = eval(A(A(A(define,name),case_no),case_yes));

	value fn;
	if (def->L == case_yes)
		fn = def->R;
	else
		{
		report_undef(sym);
		fn = sym;
		if (def != case_no)
			bad_type();
		}

	value result = apply(do_resolve(abstract(sym,form)),fn);

	drop(def);
	drop(form);
	return result;
	}

/* Resolve the form in the given context, reporting any undefined symbols to
stderr. */
static value resolve(value form, value define_string, value define_name,
	value new_source_name)
	{
	form = eval(form);
	curr_define_string = eval(define_string);
	curr_define_name = eval(define_name);
	new_source_name = eval(new_source_name);

	source_name = get_str(new_source_name)->data;

	case_no = Qlong(1); hold(case_no);
	case_yes = Qlong(2); hold(case_yes);

	value result = do_resolve(form);
	if (result->T == type_form)
		die(0); /* Form has undefined symbols. */

	drop(case_no);
	drop(case_yes);

	drop(curr_define_string);
	drop(curr_define_name);
	drop(new_source_name);

	return result;
	}

/* (resolve form define_string define_name source_name) */
value fexl_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	return A(later,resolve(f->L->L->L->R,f->L->L->R,f->L->R,f->R));
	}

static value resolve_source(const char *name)
	{
	if (strcmp(name,"source_file") == 0) return Qfile(source_fh);
	if (strcmp(name,"source_name") == 0) return Qstr0(source_name);
	if (strcmp(name,"source_line") == 0) return Qlong(source_line);
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
	return resolve(form,yes,Q(standard_name),Qstr0(source_name));
	}
