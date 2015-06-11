#include <value.h>
#include <basic.h>
#include <die.h>
#include <memory.h>
#include <num.h>
#include <pattern.h>
#include <report.h>
#include <str.h>
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(symbol sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct symbol));
	}

value type_sym(value f)
	{
	return type_void(f);
	}

value Qsym(string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym,(type)sym_free);
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	value h = A(f,g);
	if (f->T == type_sym || g->T == type_sym)
		h->T = type_sym;
	return h;
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(data(x->name),data(y->name));
	}

/* Replace all occurrences of sym in exp with I, returning a pair with the
replacement pattern and the updated exp. */
static value remove_symbol(value sym, value exp)
	{
	if (exp->T != type_sym)
		return A(none(),hold(exp));
	else if (exp->L == 0)
		{
		if (sym_eq(data(sym),data(exp)))
			return A(here(),Q(type_I));
		else
			return A(none(),hold(exp));
		}
	else
		{
		value f = remove_symbol(sym,exp->L);
		value g = remove_symbol(sym,exp->R);
		value h = A(
			fuse(hold(f->L),hold(g->L)),
			app(hold(f->R),hold(g->R)));
		drop(f);
		drop(g);
		return h;
		}
	}

/* Return a function that calls subst(p,f,x) when applied to x. */
value Qsubst(value p, value f)
	{
	return app(A(Q(type_subst),p),f);
	}

/* Abstract the symbol from exp, returning a form which is a function of that
symbol, and no longer contains that symbol. */
value lam(value sym, value exp)
	{
	value pair = remove_symbol(sym,exp);
	value f = Qsubst(hold(pair->L),hold(pair->R));
	drop(pair);
	drop(sym);
	drop(exp);
	return f;
	}

/* (subst pattern form arg) */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	replace(f,subst(f->L->L->R,f->L->R,f->R));
	return f;
	}

/* Resolve an individual symbol x with cur_context. */
static value cur_context;
static value dynamic_context(value x)
	{
	{
	/* Define numeric literals. */
	const char *name = ((string)data(x))->data;
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value exp = eval(A(A(hold(cur_context),hold(x)),Q(type_single)));

	value def;
	if (exp->L && exp->L->T == type_single)
		def = hold(exp->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

static short undefined = 0;

/* Resolve all symbols in exp with cur_context. */
static value do_resolve(value exp)
	{
	if (exp->T != type_sym)
		return exp;
	else if (exp->L == 0)
		{
		symbol sym = data(exp);
		value def = dynamic_context(sym->name);
		if (!def)
			{
			const char *name = ((string)data(sym->name))->data;
			undefined_symbol(name,sym->line);
			undefined = 1;
			def = Q(type_void);
			}
		drop(exp);
		return def;
		}
	else
		{
		value f = do_resolve(hold(exp->L));
		value g = do_resolve(hold(exp->R));
		value result = A(f,g);
		drop(exp);
		return result;
		}
	}

/* Resolve all symbols in exp with the definitions given by context. */
static value resolve(value exp, value context)
	{
	value save = cur_context;
	cur_context = context;

	exp = do_resolve(exp);
	if (undefined)
		die(0); /* The expression had undefined symbols. */

	drop(context);
	cur_context = save;
	return exp;
	}

/* (resolve label form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	const char *save = source_label;
	string label = data(f->L->L->R);
	source_label = label->data;

	replace(f, resolve(hold(f->L->R), hold(f->R)));

	source_label = save;
	return f;
	}
	}
