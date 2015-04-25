#include <value.h>
#include <basic.h>
#include <die.h>
#include <memory.h>
#include <num.h>
#include <output.h>
#include <source.h>
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
	if (!f->L) return 0;
	replace_void(f);
	return 0;
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
	type t = (f->T == type_sym || g->T == type_sym) ? type_sym : type_A;
	return V(t,f,g);
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(data(x->name),data(y->name));
	}

static value combine_patterns(value p, value q)
	{
	if (p == C && q == C)
		{
		drop(p);
		drop(q);
		return hold(C);
		}
	else
		return A(p,q);
	}

/* Replace all occurrences of sym in exp with I, returning a pair with the
replacement pattern and the updated exp. */
static value remove_symbol(value sym, value exp)
	{
	if (exp->T != type_sym)
		return A(hold(C),hold(exp));
	else if (exp->L == 0)
		{
		if (sym_eq(data(sym),data(exp)))
			return A(hold(I),hold(I));
		else
			return A(hold(C),hold(exp));
		}
	else
		{
		value f = remove_symbol(sym,exp->L);
		value g = remove_symbol(sym,exp->R);
		value h = A(combine_patterns(hold(f->L),hold(g->L)),
			app(hold(f->R),hold(g->R)));
		drop(f);
		drop(g);
		return h;
		}
	}

/* Return a function that calls substitute(p,f) when applied to x. */
value Qsubst(value p, value f)
	{
	return app(V(type_subst,Q(type_subst),p),f);
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

/* Make a copy of f, but substitute x wherever I appears in pattern p.  There
are a couple of cases where I look ahead at p->L for optimization purposes.  At
this time it makes the code 1112 bytes smaller, and a certain test runs about
3.5% faster.  At some point we might want to replace substitution patterns with
trees of function pointers to avoid the conditional branching, which might be
faster in itself, but might also give us more opportunities for "long reach"
patterns that do certain common substitutions "all at once".
*/
static value x;
static value substitute(value p, value f)
	{
	if (p == C)
		return hold(f);
	else if (p == I)
		return hold(x);
	else if (p->L == I)
		return A(hold(x),substitute(p->R,f->R));
	else if (p->L == C)
		return A(hold(f->L),substitute(p->R,f->R));
	else
		return A(substitute(p->L,f->L),substitute(p->R,f->R));
	}

/* (subst pattern form arg) */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	x = f->R;
	replace(f,substitute(f->L->L->R,f->L->R));
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
	value single = Q(type_single);
	value exp = eval(A(A(hold(cur_context),hold(x)),single));

	value def;
	if (exp->L == single)
		def = hold(exp->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

static short undefined = 0;
static void undefined_symbol(const char *name, unsigned long line)
	{
	put_to_error();
	put("Undefined symbol "); put(name); put_error_location(line);
	undefined = 1;
	}

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
