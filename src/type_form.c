#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <standard.h>
#include <string.h> /* strcmp */
#include <type_form.h>
#include <type_str.h>
#include <type_sym.h>

struct form
	{
	value exp;
	value label;
	};

static struct form *get_form(value x)
	{
	return (struct form *)x->R;
	}

static void form_free(struct form *form)
	{
	drop(form->exp);
	drop(form->label);
	free_memory(form,sizeof(struct form));
	}

value type_form(value f)
	{
	if (f->N == 0)
		{
		form_free(get_form(f));
		return 0;
		}
	return type_void(f);
	}

value Qform(value exp, value label)
	{
	struct form *form = new_memory(sizeof(struct form));
	form->exp = exp;
	form->label = label;
	return D(type_form,form);
	}

value form_exp(value f)
	{
	return get_form(f)->exp;
	}

const char *form_label(value f)
	{
	return str_data(get_form(f)->label);
	}

/* Resolve all the symbols in the expression which have a definition given by
resolve_name.  All other symbols are left unchanged. */
static value resolve(value exp, value resolve_name(const char *name))
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		value def = resolve_name(sym_name(exp));
		if (def)
			return def;
		else
			return hold(exp);
		}
	else
		{
		value L = resolve(exp->L,resolve_name);
		value R = resolve(exp->R,resolve_name);
		if (L == exp->L && R == exp->R)
			{
			drop(L);
			drop(R);
			return hold(exp);
			}
		else
			return app(L,R);
		}
	}

value op_resolve(value resolve_name(const char *name), value f)
	{
	if (!f->L) return 0;
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = resolve(form_exp(form),resolve_name);
		f = Qform(exp,hold(get_form(form)->label));
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}
	}

/* Return an expression where each occurrence of name is replaced with def. */
static value define(const char *name, value def, value exp)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		if (strcmp(name,sym_name(exp)) == 0)
			return hold(def);
		else
			return hold(exp);
		}
	else
		{
		value L = define(name,def,exp->L);
		value R = define(name,def,exp->R);
		if (L == exp->L && R == exp->R)
			{
			drop(L);
			drop(R);
			return hold(exp);
			}
		else
			return app(L,R);
		}
	}

/* (define name def form) Define name as def in form. */
value type_define(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value name = arg(f->L->L->R);
	if (name->T == type_str)
		{
		value form = arg(f->R);
		if (form->T == type_form)
			{
			value def = f->L->R;
			value exp = define(str_data(name),def,form_exp(form));
			f = Qform(exp,hold(get_form(form)->label));
			}
		else
			f = hold(Qvoid);
		drop(form);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}
	}

/* (is_resolved form) Return true if the form is fully resolved (contains no
symbols). */
value type_is_resolved(value f)
	{
	if (!f->L) return 0;
	{
	value form = arg(f->R);
	if (form->T == type_form)
		f = boolean(form_exp(form)->T != type_sym);
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}
	}

/* Report all symbols as undefined. */
static void report_undef(value exp, const char *label)
	{
	if (exp->T != type_sym)
		;
	else if (exp->L == 0)
		undefined_symbol(sym_name(exp),sym_line(exp),label);
	else
		{
		report_undef(exp->L,label);
		report_undef(exp->R,label);
		}
	}

/* (evaluate form) Evaluate the form expression if it is fully resolved,
otherwise report all distinct symbols as undefined and die. */
value type_evaluate(value f)
	{
	if (!f->L) return 0;
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		f = hold(form_exp(form));
		if (f->T == type_sym)
			{
			report_undef(f,form_label(form));
			die(0); /* The expression had undefined symbols. */
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}
	}

/* (evaluate_later form) Returns the expression for later evaluation. */
value type_evaluate_later(value f)
	{
	f = type_evaluate(f);
	if (!f || f == Qvoid) return f;
	return A(hold(Qlater),f);
	}
