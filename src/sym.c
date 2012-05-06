#include "die.h"
#include "value.h"
#include "string.h"
#include "sym.h"

value type_name(value f) { return 0; }

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
	if (!f->L) return 0;
	value x = f->R;
	if (!arg(type_name,x)) return 0;

	value v = Q(type_string);
	v->R = x->R;
	hold(v->R);
	return v;
	}

/* Convert string to name. */
value fexl_string_name(value f)
	{
	if (!f->L) return 0;
	value x = f->R;
	if (!arg(type_string,x)) return 0;

	value v = Q(type_name);
	v->R = x->R;
	hold(v->R);
	return v;
	}
