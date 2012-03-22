#include "value.h"
#include "die.h"
#include "string.h"
#include "sym.h"

struct value *type_name(struct value *f) { return f; }

/* Convert a string to a name. */
struct value *Qname(struct value *f)
	{
	if (f->T != type_string) die("Qname");
	f->T = type_name;
	return f;
	}

int sym_eq(struct value *form, struct value *sym)
	{
	return (form->T == type_name || form->T == type_string)
		&& sym->T == form->T
		&& string_eq(sym,form);
	}
