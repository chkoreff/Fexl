#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <string.h> // strcmp
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>

value type_quo(value fun, value f)
	{
	return type_void(fun,f);
	}

value type_ref(value fun, value f)
	{
	return type_void(fun,f);
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
		if (str_eq(name,get_str(e->R)))
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

value type_form(value fun, value f)
	{
	return type_void(fun,f);
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
value type_D(value fun, value f)
	{
	return sub(fun->L,fun->R,f->R);
	}

// Eager substitution
value type_E(value fun, value f)
	{
	value x = arg(f->R);
	value g = sub(fun->L,fun->R,x);
	drop(x);
	return g;
	}

// Return true if the form has no undefined symbols.
value type_is_closed(value fun, value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		f = boolean(form->R->T == type_quo);
	else
		f = hold(Qvoid);
	drop(form);
	(void)fun;
	return f;
	}

// Define key as val in an expression.
static value def(string key, value val, value exp)
	{
	value pattern = get_pattern(key,exp);
	if (pattern == QF)
		exp = hold(exp);
	else
		{
		value x = quo(hold(val));
		exp = subf(pattern,exp,x);
		drop(x);
		}
	drop(pattern);
	return exp;
	}

// (def key val form) Return form with key defined as val.
value type_def(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	{
	value key = arg(fun->L->R);
	if (key->T == type_str)
		{
		value form = arg(f->R);
		if (form->T == type_form)
			{
			if (form->R->T == type_quo)
				f = hold(form);
			else
				f = Qform(hold(form->L),def(get_str(key),fun->R,form->R));
			}
		else
			f = hold(Qvoid);
		drop(form);
		}
	else
		f = hold(Qvoid);
	drop(key);
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

// Evaluate the form if all symbols are defined, otherwise report the undefined
// symbols and die.
value type_value(value fun, value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		if (form->R->T == type_quo)
			f = hold(form->R->R);
		else
			{
			report_undef(str_data(form->L),form->R);
			die(0);
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	(void)fun;
	return f;
	}

static const char *cur_name;

int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

// Like type_value, except it yields the form value to the caller without
// evaluating it.
value type_resolve(value fun, value f)
	{
	return yield(type_value(fun,f));
	}

static value cache = 0;

static value find_cache(string name)
	{
	value pos = cache;
	while (pos)
		{
		if (str_eq(name,get_str(pos->L->R)))
			return pos->R;
		pos = pos->next;
		}
	return 0;
	}

static void push_cache(value exp, value val)
	{
	value top = new_value();
	top->L = hold(exp);
	top->R = hold(val);
	top->next = cache;
	cache = top;
	}

static void pop_cache(void)
	{
	value next = cache->next;
	drop(cache->L);
	drop(cache->R);
	recycle(cache);
	cache = next;
	}

static value do_resolve(value exp, value define())
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		string name = get_str(exp->R);
		value val = find_cache(name);
		if (val != 0)
			{
			if (val->T == type_quo)
				return hold(val);
			else
				return hold(exp);
			}
		else
			{
			cur_name = name->data;
			val = define();
			if (val != 0)
				{
				val = quo(val);
				push_cache(exp,val);
				return val;
				}
			else
				{
				push_cache(exp,Qvoid);
				return hold(exp);
				}
			}
		}
	else
		return join(exp->T,
			do_resolve(exp->L,define),
			do_resolve(exp->R,define));
	}

value op_resolve(value fun, value f, value define(void))
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = do_resolve(form->R,define);
		while (cache)
			pop_cache();
		f = Qform(hold(form->L),exp);
		}
	else
		f = hold(Qvoid);
	drop(form);
	(void)fun;
	return f;
	}

static int in_list(string s, value list)
	{
	while (list->T == type_list)
		{
		if (str_eq(s,get_str(list->L)))
			return 1;
		list = list->R;
		}
	return 0;
	}

// Get the list of unique undefined symbols in exp.
static value form_undefs(value exp, value list)
	{
	if (exp->T == type_quo)
		return list;
	else if (exp->T == type_ref)
		{
		string s = get_str(exp->R);
		if (in_list(s,list))
			return list;
		else
			{
			value top = Qstr(str_copy(s));
			return V(type_list,top,list);
			}
		}
	else
		{
		list = form_undefs(exp->R,list);
		list = form_undefs(exp->L,list);
		return list;
		}
	}

// (form_undefs form) Return the list of unique undefined symbols in the form.
value type_form_undefs(value fun, value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		f = form_undefs(form->R,hold(Qnull));
	else
		f = hold(Qvoid);
	drop(form);
	(void)fun;
	return f;
	}

static value form_refs(value exp, value list)
	{
	if (exp->T == type_quo)
		return list;
	else if (exp->T == type_ref)
		{
		value x = exp->R;
		value top = pair(Qstr(str_copy(get_str(x))),Qnum(x->N));
		return V(type_list,top,list);
		}
	else
		{
		list = form_refs(exp->R,list);
		list = form_refs(exp->L,list);
		return list;
		}
	}

// (form_refs form) Return {label list}, where label is the name of the source
// file and list is the list of {name line} symbol references in the form.
value type_form_refs(value fun, value f)
	{
	value form = arg(f->R);
	if (form->T == type_form)
		f = pair(hold(form->L),form_refs(form->R,hold(Qnull)));
	else
		f = hold(Qvoid);
	drop(form);
	(void)fun;
	return f;
	}
