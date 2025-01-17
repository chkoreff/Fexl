#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <report.h>
#include <type_record.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_with.h>

static value Qstd;

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

// For each ref key in exp, use cx to get its val, then store key-val in obj.
static void imprint(value exp, value cx, value obj)
	{
	if (exp->T == type_quo)
		;
	else if (exp->T == type_ref)
		{
		string name = exp->R->v_ptr;
		if (!record_find(obj,name))
			{
			value key = hold(Qstr(str_copy(name)));
			value val = eval(A(A(hold(cx),key),hold(Qyield)));

			if (val->L == Qyield)
				record_set(obj,key,hold(val->R));

			drop(key);
			drop(val);
			}
		}
	else
		{
		imprint(exp->L,cx,obj);
		imprint(exp->R,cx,obj);
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
			value obj = record_empty();
			imprint(exp,cx,obj);
			exp = resolve(exp,obj);
			drop(obj);

			if (exp->T == type_quo)
				f = tail(exp);
			else
				{
				report_undef(exp,str_data(form->L));
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

// \extend=(\cx value (def "std" cx; cx))
value type_extend(value f)
	{
	value cx = eval(f->R);
	cx = Qassoc(hold(Qstd),yield(hold(cx)),cx);
	f->R = cx;
	f->T = type_value;
	return hold(f);
	}

void beg_sym(void)
	{
	Qstd = Qstr0("std");
	}

void end_sym(void)
	{
	drop(Qstd);
	}
