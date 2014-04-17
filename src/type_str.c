#include <value.h>
#include <str.h>
#include <type_str.h>

value type_str(value f)
	{
	if (!f->N) str_free(get_str(f));
	return f;
	}

struct str *get_str(value f)
	{
	return (struct str *)get_data(f,type_str);
	}

value Qstr(struct str *p)
	{
	return V(type_str,0,(value)p);
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
	value z = Qstr(str_concat(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}
