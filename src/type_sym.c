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

static int is_closed(value exp)
	{
	return exp->R->T == type_quo;
	}

// Return true if the form has no undefined symbols.
value type_is_closed(value fun, value f)
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		f = boolean(is_closed(exp));
	else
		f = hold(Qvoid);
	drop(exp);
	(void)fun;
	return f;
	}

// Define key as val in a form.
static value def(string key, value val, value exp)
	{
	value pattern = get_pattern(key,exp->R);
	value result;

	if (pattern == QF)
		result = hold(exp);
	else
		{
		value x = quo(hold(val));
		value new = subf(pattern,exp->R,x);
		drop(x);
		result = Qform(hold(exp->L),new);
		}

	drop(pattern);
	return result;
	}

value type_def(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	{
	value key = arg(fun->L->R);
	if (key->T == type_str)
		{
		value exp = arg(f->R);
		if (exp->T == type_form)
			f = def(get_str(key),fun->R,exp);
		else
			f = hold(Qvoid);
		drop(exp);
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
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		if (is_closed(exp))
			f = hold(exp->R->R);
		else
			{
			report_undef(str_data(exp->L),exp->R);
			die(0);
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
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
static value get_list_undef(value exp, value list)
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
		list = get_list_undef(exp->R,list);
		list = get_list_undef(exp->L,list);
		return list;
		}
	}

static value get_def(string name, value define(void))
	{
	cur_name = name->data;
	return define();
	}

// Map a list of symbols to a list of {name val} pairs, for all defined names.
static value get_defs(value list, value define(void))
	{
	if (list->T == type_list)
		{
		string name = get_str(list->L);
		value val = get_def(name,define);
		value tail = get_defs(list->R,define);
		if (val)
			{
			value top = pair(hold(list->L),val);
			return V(type_list,top,tail);
			}
		else
			return tail;
		}
	else
		return hold(Qnull);
	}

// Get the list of {name val} pairs needed to resolve an expression.
static value skim_context(value exp, value define(void))
	{
	value list = get_list_undef(exp,hold(Qnull));
	value cx = get_defs(list,define);
	drop(list);
	return cx;
	}

static value resolve_name(value cx, string name)
	{
	while (cx->T == type_list)
		{
		value top = (cx->L = eval(cx->L));
		if (top->T == type_pair)
			{
			value key = (top->L = eval(top->L));
			if (key->T == type_str && str_eq(name,get_str(key)))
				return hold(top->R);
			}
		cx = (cx->R = eval(cx->R));
		}
	return 0;
	}

static value do_resolve(value cx, value exp)
	{
	if (exp->T == type_quo)
		return hold(exp);
	else if (exp->T == type_ref)
		{
		string name = get_str(exp->R);
		value val = resolve_name(cx,name);
		if (val)
			return quo(val);
		else
			return hold(exp);
		}
	else
		return join(exp->T,
			do_resolve(cx,exp->L),
			do_resolve(cx,exp->R));
	}

value op_resolve(value fun, value f, value define(void))
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		if (is_closed(exp))
			f = hold(exp);
		else
			{
			value cx = skim_context(exp->R,define);
			value new = do_resolve(cx,exp->R);
			drop(cx);
			f = Qform(hold(exp->L),new);
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
	(void)fun;
	return f;
	}

// (resolve_pairs cx form) Partially resolve a form using the list of pairs cx.
value type_resolve_pairs(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		value cx = arg(fun->R);
		f = Qform(hold(exp->L),do_resolve(cx,exp->R));
		drop(cx);
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}

// (evaluate cx form) Resolve form using the list of pairs cx.  If fully
// resolved, return the value, otherwise report undefined symbols and die.
value type_evaluate(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		value cx = arg(fun->R);
		f = do_resolve(cx,exp->R);
		if (f->T == type_quo)
			f = unquo(f);
		else
			{
			report_undef(str_data(exp->L),f);
			die(0);
			}
		drop(cx);
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
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

// (form_undefs form) Return the list of unique undefined symbols in the form.
value type_form_undefs(value fun, value f)
	{
	value exp = arg(f->R);
	(void)fun;
	if (exp->T == type_form)
		f = get_list_undef(exp->R,hold(Qnull));
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}

// (form_refs form) Return {label list}, where label is the name of the source
// file and list is the list of {name line} symbol references in the form.
value type_form_refs(value fun, value f)
	{
	value exp = arg(f->R);
	(void)fun;
	if (exp->T == type_form)
		f = pair(hold(exp->L),form_refs(exp->R,hold(Qnull)));
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
