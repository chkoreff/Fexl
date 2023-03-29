#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <die.h>
#include <input.h>
#include <memory.h>
#include <type_num.h>
#include <type_str.h>

string get_str(value x)
	{
	return (string)x->R;
	}

value type_str(value f)
	{
	return type_atom(f);
	}

static void drop_str(value f)
	{
	str_free((string)f->R);
	}

value Qstr(string x)
	{
	static struct value atom = {0, (type)drop_str};
	return V(type_str,&atom,(value)x);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

const char *str_data(value x)
	{
	return get_str(x)->data;
	}

value op_str(value f, string op(string))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string result = op(get_str(x));
		if (result)
			f = Qstr(result);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value op_str2(value f, string op(string,string))
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		{
		string result = op(get_str(x),get_str(y));
		if (result)
			f = Qstr(result);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value op_str3(value f, string op(string,string,string))
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);

	if (x->T == type_str && y->T == type_str && z->T == type_str)
		{
		string result = op(get_str(x),get_str(y),get_str(z));
		if (result)
			f = Qstr(result);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);

	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	return op_str2(f,str_concat);
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qnum((double)get_str(x)->len);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (slice str pos len) calls str_slice, except it returns void if pos or len is
negative. */
value type_slice(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_num && z->T == type_num)
		{
		double yn = get_double(y);
		double zn = get_double(z);
		if (yn >= 0 && zn >= 0)
			f = Qstr(str_slice(get_str(x),yn,zn));
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* (search haystack needle offset) calls str_search. */
value type_search(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_num)
		{
		double zn = get_double(z);
		if (zn >= 0)
			{
			string xs = get_str(x);
			string ys = get_str(y);
			unsigned long pos = str_search(xs,ys,zn);
			if (pos < xs->len)
				f = Qnum((double)pos);
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* Convert string to number if possible. */
value type_str_num(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		double val;
		if (str0_double(str_data(x),&val))
			f = Qnum(val);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (ord x) is the ordinal number of the first ASCII character of string x. */
value type_ord(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string xs = get_str(x);
		f = Qnum((double) (xs->len == 0 ? 0 : (unsigned char)xs->data[0]) );
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (chr x) is the ASCII character whose ordinal number is x. */
value type_chr(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		double xn = get_double(x);
		char ch = xn;
		f = Qstr(str_new_data(&ch,1));
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (char_width str pos) Return the width of the UTF-8 character which starts at
the given position. */
value type_char_width(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		double yn = get_double(y);
		if (yn >= 0)
			{
			string xs = get_str(x);
			unsigned long pos = yn;
			if (pos < xs->len)
				{
				char n = char_width(xs->data[pos]);
				f = Qnum((double)n);
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_dirname(value f) { return op_str(f,dirname); }
value type_basename(value f) { return op_str(f,basename); }

/* (length_common x y) Return the number of initial bytes which x and y have in
common. */
value type_length_common(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = Qnum((double)length_common(get_str(x),get_str(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (compare_at pos x y LT EQ GT)
Compare the corresponding characters at offset pos and return LT, EQ, or GT.
Equivalent to:

	(\pos\x\y\LT\EQ\GT
	\cx=(slice x pos 1)
	\cy=(slice y pos 1)
	gt cx cy GT; lt cx cy LT; EQ
	)
*/
value type_compare_at(value f)
	{
	if (0
		|| !f->L
		|| !f->L->L
		|| !f->L->L->L
		|| !f->L->L->L->L
		|| !f->L->L->L->L->L
		|| !f->L->L->L->L->L->L
		)
		return 0;
	{
	value pos = arg(f->L->L->L->L->L->R);
	value x = arg(f->L->L->L->L->R);
	value y = arg(f->L->L->L->R);

	if (pos->T == type_num && x->T == type_str && y->T == type_str)
		{
		string xs = get_str(x);
		string ys = get_str(y);
		unsigned long n = get_ulong(pos);
		int cx = n < xs->len ? xs->data[n] : -1;
		int cy = n < ys->len ? ys->data[n] : -1;

		if (cx < cy)
			f = hold(f->L->L->R);
		else if (cx == cy)
			f = hold(f->L->R);
		else
			f = hold(f->R);
		}
	else
		f = hold(Qvoid);

	drop(pos);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_is_str(value f)
	{
	return op_is_type(f,type_str);
	}

static unsigned long list_length(value p)
	{
	unsigned long len = 0;
	while (p->T == 0)
		{
		len++;
		p = p->R;
		}
	return len;
	}

/* Collect a vector of raw (char *) strings from a list and pass it to the op
function.  Any item that is not a string ends the list. */
value op_argv(value f, value op(const char *const *argv))
	{
	if (!f->L) return 0;
	{
	value items = hold(expand(f));
	unsigned long len = list_length(items);
	unsigned long size = (len+1) * sizeof(char *);
	const char **argv = new_memory(size);
	value p = items;
	unsigned long pos = 0;

	while (p->T == 0)
		{
		value item = (p->L = eval(p->L));
		if (item->T != type_str)
			break;
		argv[pos++] = str_data(item);
		p = p->R;
		}
	argv[pos] = 0; /* ending sentinel */

	f = op(argv);

	free_memory((void *)argv,size);
	drop(items);

	return f;
	}
	}
