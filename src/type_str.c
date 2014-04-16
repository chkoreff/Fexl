#include <str.h>

#include <value.h>
#include <type_str.h>

value type_str(value f)
	{
	if (!f->N) str_free(atom_str(f));
	return f;
	}

struct str *atom_str(value f)
	{
	return (struct str *)atom_data(f,type_str);
	}

value Qstr(struct str *p)
	{
	return atom(type_str,p);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

/* (concat x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qstr(str_concat(atom_str(x),atom_str(y)));
	drop(x);
	drop(y);
	return z;
	}
