#include "die.h"
#include "value.h"
#include "eval.h"
#include "string.h"
#include "sym.h"

value type_name(value f) { return f; }

/* Convert a string to a name. */
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

value fexl_name_string(value f)
	{
	value x = f->R;
	if (!arg(type_name,x)) return f;
	return Qcopy_chars(string_data(x), string_len(x));
	}
