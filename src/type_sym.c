#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <report.h>
#include <string.h> // strcmp
#include <type_str.h>
#include <type_sym.h>

value type_quo(value f)
	{
	return type_void(f);
	}

value type_ref(value f)
	{
	return type_void(f);
	}

static void clear_quo(value f)
	{
	drop(f->R);
	}

value quo(value val)
	{
	static struct value clear = {{.N=0}, {.clear=clear_quo}};
	return V(type_quo,&clear,val);
	}

static void clear_ref(value f)
	{
	value x = f->R;
	str_free(x->v_ptr);
	recycle(x);
	}

value ref(string name, unsigned long line)
	{
	static struct value clear = {{.N=0}, {.clear=clear_ref}};
	value x = new_value();
	x->v_ptr = name;
	x->N = line;
	return V(type_ref,&clear,x);
	}

static value unquo(value f)
	{
	value x = hold(f->R);
	drop(f);
	return x;
	}

value join(type t, value x, value y)
	{
	if (x->T == type_quo && y->T == type_quo)
		return quo(V(t,unquo(x),unquo(y)));
	else
		return V(t,x,y);
	}

value app(value x, value y)
	{
	return join(0,x,y);
	}

static value get_pattern(string name, value e)
	{
	if (e->T == type_quo)
		return hold(QF);
	else if (e->T == type_ref)
		{
		if (str_eq(name,e->R->v_ptr))
			return hold(QT);
		else
			return hold(QF);
		}
	else
		{
		value L = get_pattern(name,e->L);
		value R = get_pattern(name,e->R);
		if (L == QF && R == QF)
			{
			drop(L);
			drop(R);
			return hold(QF);
			}
		else
			return A(L,R);
		}
	}

// Substitute into a form.
static value subf(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return join(e->T,subf(p->L,e->L,x),subf(p->R,e->R,x));
	}

value lam(type type, string name, value e)
	{
	value p = get_pattern(name,e);
	value x = quo(hold(QI));
	value f = subf(p,e,x);
	drop(x);
	if (f->T == type_quo)
		f = quo(V(type,p,unquo(f)));
	else
		f = V(type,quo(p),f);
	str_free(name);
	drop(e);
	return f;
	}

value type_form(value f)
	{
	return type_void(f);
	}

value Qform(value label, value exp)
	{
	return V(type_form,label,exp);
	}

// Use pattern p to make a copy of expression e with argument x substituted in
// the places designated by the pattern.
static value sub(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return V(e->T,sub(p->L,e->L,x),sub(p->R,e->R,x));
	}

// Direct substitution
value type_D(value f)
	{
	return sub(f->L->L,f->L->R,f->R);
	}

// Eager substitution
value type_E(value f)
	{
	value x = arg(f->R);
	value g = sub(f->L->L,f->L->R,x);
	drop(x);
	return g;
	}

// Return true if the form has no undefined symbols.
value type_is_closed(value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		f = boolean(form->R->T == type_quo);
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}

static const char *cur_name;

int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

// Use a C define function as a Fexl context function.
value op_context(value f, value define(void))
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		value val;
		cur_name = str_data(x);
		val = define();
		f = val ? yield(val) : hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value cache;

static value find_cache(string name)
	{
	value pos = cache;
	while (pos)
		{
		if (str_eq(name,pos->L->R->v_ptr))
			return pos->R;
		pos = pos->next;
		}
	return 0;
	}

static void push_cache(value exp, value val)
	{
	value top = new_value();
	top->L = exp;
	top->R = val;
	top->next = cache;
	cache = top;
	}

static void clear_cache(void)
	{
	while (cache)
		{
		value next = cache->next;
		drop(cache->R);
		recycle(cache);
		cache = next;
		}
	}

static value resolve(value exp, value cx)
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		string name = exp->R->v_ptr;
		value val = find_cache(name);
		if (val)
			return (val->T == type_quo) ? hold(val) : hold(exp);
		else
			{
			value key = hold(Qstr(name));
			value val = eval(A(A(hold(cx),key),hold(Qyield)));
			value new = (val->L == Qyield) ? quo(hold(val->R)) : hold(exp);

			if (key->N == 1)
				recycle(key);
			else
				{
				// Copy the string only if you held onto the key.
				key->v_ptr = str_copy(name);
				drop(key);
				}

			drop(val);
			push_cache(exp,new);
			return hold(new);
			}
		}
	else
		{
		value L = resolve(exp->L,cx);
		value R = resolve(exp->R,cx);
		return join(exp->T,L,R);
		}
	}

static value do_resolve(value exp, value cx)
	{
	value save_cache = cache;
	cache = 0;
	exp = resolve(exp,cx);
	clear_cache();
	cache = save_cache;
	return exp;
	}

// (resolve cx form) Use context cx to resolve some symbols in the form.
value type_resolve(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = form->R;
		if (exp->T == type_quo)
			f = hold(form);
		else
			{
			value cx = (f->L->R = eval(f->L->R));
			exp = do_resolve(exp,cx);
			f = Qform(hold(form->L),exp);
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}
	}

static void report_undef(const char *label, value exp)
	{
	if (exp->T == type_quo)
		;
	else if (exp->T == type_ref)
		undefined_symbol(str_data(exp->R),exp->R->N,label);
	else
		{
		report_undef(label,exp->L);
		report_undef(label,exp->R);
		}
	}

// (value cx form) Resolve all the symbols in the form and return its value.
// If there are any undefined symbols, report them all and die.
// The cx function maps a name to (yield val) if defined, otherwise void.
value type_value(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = form->R;
		if (exp->T == type_quo)
			f = hold(exp->R);
		else
			{
			value cx = (f->L->R = eval(f->L->R));
			exp = do_resolve(exp,cx);

			if (exp->T == type_quo)
				f = unquo(exp);
			else
				{
				report_undef(str_data(form->L),exp);
				drop(exp);
				die(0);
				f = hold(Qvoid); // not reached
				}
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}
	}
