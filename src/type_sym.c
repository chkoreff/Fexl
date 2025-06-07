#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <report.h>
#include <type_record.h>
#include <type_str.h>
#include <type_sym.h>

static value Qstd;

value type_std(value f)
	{
	return hold(Qstd);
	(void)f;
	}

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

value join(type t, value x, value y)
	{
	if (x->T == type_quo && y->T == type_quo)
		return quo(V(t,tail(x),tail(y)));
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
		f = quo(V(type,p,tail(f)));
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

static void set_std(value obj)
	{
	drop(Qstd);
	Qstd = obj;
	}

void define(const char *s_key, value val)
	{
	value key = Qstr0(s_key);
	record_set(Qstd,key,val);
	drop(key);
	}

// (define key val) Define key as val in std.
value type_define(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value key = arg(f->L->R);
	if (key->T == type_str)
		{
		record_set(Qstd,key,hold(f->R));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}
	}

static value resolve(value exp, value obj)
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		string key = exp->R->v_ptr;
		value val = record_find(obj,key);
		return val ? quo(hold(val)) : hold(exp);
		}
	else
		{
		value L = resolve(exp->L,obj);
		value R = resolve(exp->R,obj);
		return join(exp->T,L,R);
		}
	}

// (resolve form) Resolve any symbols defined in std.
value type_resolve(value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		f = resolve(form->R,Qstd);
		f = Qform(hold(form->L),f);
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}

static void report_undef(value exp, const char *label)
	{
	if (exp->T == type_quo)
		;
	else if (exp->T == type_ref)
		{
		string key = exp->R->v_ptr;
		undefined_symbol(key->data,exp->R->N,label);
		}
	else
		{
		report_undef(exp->L,label);
		report_undef(exp->R,label);
		}
	}

/*
(evaluate form) Evaluate the form in std.  If there are any undefined symbols,
report them all and die.
*/
value type_evaluate(value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		f = resolve(form->R,Qstd);
		if (f->T == type_quo)
			f = tail(f);
		else
			{
			report_undef(f,str_data(form->L));
			die(0);
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	}

// (set_std rec) Set the current context to the record.
value type_set_std(value f)
	{
	value x = arg(f->R);
	if (x->T == type_record)
		{
		set_std(hold(x));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

void beg_sym(void)
	{
	Qstd = record_empty();
	}

void end_sym(void)
	{
	drop(Qstd);
	}
