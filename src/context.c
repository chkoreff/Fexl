#include <stdio.h>
#include <str.h>
#include <value.h>


#include <basic.h>
#include <die.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <context.h>

value type_form(value fun, value arg)
	{
	return type_void(fun,arg);
	}

void put_error_location(unsigned long line, const char *label)
	{
	fprintf(stderr," on line %lu", line);
	if (label[0])
		fprintf(stderr," of %s\n", label);
	else
		fprintf(stderr,"\n");
	}

static void undefined_symbol(const char *name, unsigned long line,
	const char *label)
	{
	fprintf(stderr,"Undefined symbol %s",name);
	put_error_location(line,label);
	}

static value do_restrict(value cx, value form)
	{
	value label = form->L;
	value exp = form->R;
	value map = exp->L;
	value body = hold(exp->R);
	int has_undef = 0;

	while (map->T == 0)
		{
		if (cx->L == 0)
			{
			value sym = map->L->L->L;
			unsigned long line = map->L->L->R->v_double;
			undefined_symbol(str_data(sym),line,str_data(label));
			has_undef = 1;
			map = map->R;
			}
		else
			{
			value key = cx->L->L;
			value sym = map->L->L->L;
			int cmp = str_cmp(key->v_ptr,sym->v_ptr);

			if (cmp < 0)
				cx = cx->R;
			else if (cmp == 0)
				{
				value val = cx->L->R;
				value pattern = map->L->R;
				value new = subst(pattern,body,val);
				drop(body);
				body = new;
				cx = cx->R;
				map = map->R;
				}
			else
				{
				unsigned long line = map->L->L->R->v_double;
				undefined_symbol(str_data(sym),line,str_data(label));
				has_undef = 1;
				map = map->R;
				}
			}
		}
	if (has_undef)
		die(0);

	drop(form);
	return body;
	}

value type_restrict(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_record);
	else
		{
		arg = eval(arg);
		if (arg->T == type_form)
			{
			value exp = do_restrict(fun->R, arg);
			drop(fun);
			return exp;
			}
		else
			return type_void(fun,arg);
		}
	}

value cx_std; // Current context

void define(const char *key, value val)
	{
	// TODO use chain on the individual contexts
	cx_std = def(Qstr0(key),val,cx_std);
	}

// Define initial context.

void beg_context(void)
	{
	cx_std = Q(type_record); // empty stack

	use_basic();
	use_num();
	use_str();
	use_math();
	use_output();
	use_meta();
	use_limit();
	use_var();
	use_record();

	define("restrict",Q(type_restrict));
	define("std",hold(cx_std));

	// LATER time
	// LATER rand
	// LATER crypto
	}

void end_context(void)
	{
	drop(cx_std);
	}
