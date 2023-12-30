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
#include <type_with.h>

static value Qdef_std;

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
	return f;
	(void)fun;
	}

// Define key as val in an expression.
static value def(string key, value val, value exp)
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		if (str_eq(key,get_str(exp->R)))
			return quo(hold(val));
		else
			return hold(exp);
		}
	else
		{
		value L = def(key,val,exp->L);
		value R = def(key,val,exp->R);
		if (L == exp->L && R == exp->R)
			{
			drop(L);
			drop(R);
			return hold(exp);
			}
		else
			return join(exp->T,L,R);
		}
	}

// (def key val form) Return form with key defined as val.
// LATER 20231223 "def" is deprecated
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

static const char *cur_name;

int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

// Use a C define function as a Fexl context function.
value op_context(value fun, value f, value define(void))
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		value val;
		cur_name = str_data(x);
		val = define();
		if (val)
			f = yield(val);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
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
// LATER 20231223 "value" is deprecated
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
			f = hold(Qvoid); // not reached
			}
		}
	else
		f = hold(Qvoid);
	drop(form);
	return f;
	(void)fun;
	}

static value find_cache(string name, value cache)
	{
	while (cache)
		{
		if (str_eq(name,get_str(cache->L->R)))
			return cache->R;
		cache = cache->next;
		}
	return 0;
	}

static value push_cache(value exp, value val, value cache)
	{
	value top = new_value();
	top->L = exp;
	top->R = val;
	top->next = cache;
	return top;
	}

static void clear_cache(value cache)
	{
	while (cache)
		{
		value next = cache->next;
		drop(cache->R);
		recycle(cache);
		cache = next;
		}
	}

// Build a cache of values for all symbols in exp using context cx.
static value build_cache(value exp, value cx, value cache)
	{
	if (exp->T == type_quo)
		return cache;
	else if (exp->T == type_ref)
		{
		string name = get_str(exp->R);
		value val = find_cache(name,cache);
		if (val)
			return cache;
		else
			{
			value val = eval(A(A(hold(cx),Qstr(str_copy(name))),hold(Qyield)));
			value new = (val->L == Qyield) ? quo(hold(val->R)) : hold(exp);
			drop(val);
			return push_cache(exp,new,cache);
			}
		}
	else
		{
		cache = build_cache(exp->L,cx,cache);
		cache = build_cache(exp->R,cx,cache);
		return cache;
		}
	}

static value resolve(value exp, value cache)
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		string name = get_str(exp->R);
		value val = find_cache(name,cache);
		return (val->T == type_quo) ? hold(val) : hold(exp);
		}
	else
		{
		value L = resolve(exp->L,cache);
		value R = resolve(exp->R,cache);
		return join(exp->T,L,R);
		}
	}

static value do_resolve(value exp, value cx)
	{
	value cache = build_cache(exp,cx,0);
	exp = resolve(exp,cache);
	clear_cache(cache);
	return exp;
	}

// (resolve cx form) Use context cx to resolve some symbols in the form.
value type_resolve(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = form->R;
		if (exp->T == type_quo)
			f = hold(form);
		else
			{
			value cx = (fun->R = eval(fun->R));
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

// (evaluate cx form) Resolve all the symbols in the form and return its value.
// If there are any undefined symbols, report them all and die.
// The cx function maps a name to (yield val) if defined, otherwise void.
value type_evaluate(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value form = arg(f->R);
	if (form->T == type_form)
		{
		value exp = form->R;
		if (exp->T == type_quo)
			f = hold(exp->R);
		else
			{
			value cx = (fun->R = eval(fun->R));
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

// \extend=(\cx evaluate (define "cx_std" cx; cx))
value type_extend(value fun, value f)
	{
	value cx = (f->R = eval(f->R));
	f->T = type_evaluate;
	f->R = A(A(hold(Qdef_std),cx),hold(cx));
	return hold(f);
	(void)fun;
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
	return f;
	(void)fun;
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
	return f;
	(void)fun;
	}

void beg_sym(void)
	{
	Qdef_std = A(Q(type_define),Qstr0("cx_std"));
	}

void end_sym(void)
	{
	drop(Qdef_std);
	}
