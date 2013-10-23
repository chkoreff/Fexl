#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "die.h"
#include "file.h"
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
	struct str *full_name = str_concat(prefix, name);

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

static value curr_define_string;
static value curr_define_name;
static int curr_strict;
static FILE *curr_fh;
static const char *curr_name;
static long curr_line;

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

static int eval_boolean(value form)
	{
	form = eval(A(A(form,C),F));
	if (form != C && form != F)
		bad_type();
	drop(form);
	return form == C;
	}

static value eval_maybe(value form)
	{
	value def = eval(A(A(form,C),yes));

	value fn;
	if (def->L == yes)
		{
		fn = def->R;
		hold(fn);
		}
	else
		{
		fn = 0;
		if (def != C)
			bad_type();
		}
	drop(def);
	return fn;
	}

static value do_resolve(value form)
	{
	value sym = first_symbol(form);
	if (sym == 0) return form;

	value define = (sym->R->L->T == fexl_C)
		? curr_define_string : curr_define_name;
	value name = sym->L;

	value fn = eval_maybe(A(define,name));
	if (fn == 0)
		{
		fn = sym;
		hold(fn);
		if (curr_strict)
			report_undef(sym);
		}

	value new_form = abstract(sym,form);
	value resolved = do_resolve(new_form);
	value result = apply(resolved,fn);

	drop(new_form);
	drop(fn);

	return result;
	}

value resolve(value form, value define_string, value define_name,
	value source_name, value strict)
	{
	value save_define_string = curr_define_string;
	value save_define_name = curr_define_name;
	const char *save_name = curr_name;

	form = eval(form);
	curr_define_string = eval(define_string);
	curr_define_name = eval(define_name);

	source_name = eval(source_name);
	curr_name = get_str(source_name)->data;

	curr_strict = eval_boolean(strict);

	value result = do_resolve(form);
	if (curr_strict && result->T == type_form)
		die(0); /* Form has undefined symbols. */

	drop(form);
	drop(curr_define_string);
	drop(curr_define_name);
	drop(source_name);

	curr_define_string = save_define_string;
	curr_define_name = save_define_name;
	curr_name = save_name;

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

static struct str *get_full_path(struct str *name)
	{
	struct str *base = base_path();
	struct str *path = str_concat(base,name);
	str_free(base);
	return path;
	}

static value parse_local(struct str *name)
	{
	struct str *path = get_full_path(name);
	FILE *fh = fopen(path->data,"r");
	str_free(path);

	if (fh == 0)
		die("Missing file: %s", name->data);

	long line = 1;
	value exp = parse(fh,name->data,&line);
	fclose(fh);

	return exp;
	}

static value resolve_source(const char *name)
	{
	if (strcmp(name,"source_file") == 0) return Qfile(curr_fh);
	if (strcmp(name,"source_name") == 0) return Qstr0(curr_name);
	if (strcmp(name,"source_line") == 0) return Qlong(curr_line);
	return 0;
	}

static value cache_context = 0;

static value enhanced_context(void)
	{
	value context = cache_context;
	if (context == 0)
		{
		struct str *file_name = str_new_data0("share/fexl/fexl.fxl");
		value exp = parse_local(file_name);
		context = resolve(exp,yes,Q(fexl_define_name),Qstr(file_name),C);
		hold(context);
		cache_context = context;
		}
	return context;
	}

/* LATER make this whole context available within Fexl. */
static value type_standard_name(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	struct str *name = get_str(x);

	value def = define_name(name);
	if (def == 0)
		def = resolve_source(name->data);

	if (def == 0)
		def = eval_maybe(A(enhanced_context(),x));
	else
		hold(def);

	value result = maybe(def);
	if (def)
		drop(def);
	drop(x);
	return result;
	}

/* Resolve the form in the standard context. */
value resolve_standard(value form, FILE *fh, const char *name, long line)
	{
	curr_fh = fh;
	curr_name = name;
	curr_line = line;

	value result = resolve(form,yes,Q(type_standard_name),Qstr0(name),C);
	if (cache_context)
		drop(cache_context);
	return result;
	}
