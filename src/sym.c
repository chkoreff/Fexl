#include "die.h"
#include "value.h"
#include "eval.h"
#include "string.h"
#include "sym.h"

value type_name(value f) { return f; }

/* Convert a string to a name. */
/* LATER I may wrap a name functor around the string instead of changing its
type here, so when I convert between names and strings I don't have to make a
copy */
value Qname(value f)
	{
	if (f->T != type_string) die("Qname");
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
	return Qcopy_chars(string_data(x), string_len(x));
	}

/* Convert string to name. */
value fexl_string_name(value f)
	{
	value x = f->R;
	if (!arg(type_string,x)) return f;
	value name = Qcopy_chars(string_data(x), string_len(x));
	name->T = type_name;
	return name;
	}
