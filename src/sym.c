#include "die.h"
#include "value.h"
#include "eval.h"
#include "string.h"
#include "sym.h"

value type_name(value f) { return f; }

/* Make a name from the given data. */
value Qname(char *data, long len)
	{
	value f = Qchars(data,len);
	f->T = type_name;
	return f;
	}

int sym_eq(value form, value sym)
	{
	return (form->T == type_name || form->T == type_string)
		&& sym->T == form->T
		&& string_eq(sym,form);
	}

/* Convert name to string. */
value fexl_name_string(value f)
	{
	value x = f->R;
	if (!arg(type_name,x)) return f;

	value v = Q(type_string);
	v->R = x->R;
	hold(v->R);
	return v;
	}

/* Convert string to name. */
value fexl_string_name(value f)
	{
	value x = f->R;
	if (!arg(type_string,x)) return f;

	value v = Q(type_name);
	v->R = x->R;
	hold(v->R);
	return v;
	}
