#include "die.h"
#include "value.h"
#include "string.h"
#include "sym.h"

void type_name(value f) { type_error(); }

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
void reduce_name_string(value f)
	{
	replace(f,arg(type_name,f->R));
	f->T = type_string;
	}

/* Convert string to name. */
void reduce_string_name(value f)
	{
	replace(f,arg(type_string,f->R));
	f->T = type_name;
	}
