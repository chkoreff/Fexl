#include <str.h>

#include <value.h>
#include <type_str.h>

value type_str(value f, value g)
	{
	if (g) bad_type();
	str_free(atom_str(f));
	return 0;
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
value type_concat(value f, value g)
	{
	if (!f->L) return collect(f,g);
	value x = eval(f->R);
	value y = eval(g);
	value z = Qstr(str_concat(atom_str(x),atom_str(y)));
	drop(x);
	drop(y);
	return z;
	}
