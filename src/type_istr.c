#include <str.h>

#include <input.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_input.h>
#include <type_istr.h>
#include <type_str.h>

/* An istr structure is used to iterate through a string. */

static struct istr *istr_new(value x)
	{
	struct istr *in = new_memory(sizeof(struct istr));
	in->pos = 0;
	in->str = x;
	return in;
	}

static void istr_free(struct istr *in)
	{
	drop(in->str);
	free_memory(in,sizeof(struct istr));
	}

int sgetc(struct istr *in)
	{
	string str = get_str(in->str);
	return in->pos < str->len ? str->data[in->pos++] : -1;
	}

value Qistr(value x)
	{
	struct istr *in = istr_new(x);
	return D(type_istr,in);
	}

struct istr *get_istr(value x)
	{
	return (struct istr *)data(x);
	}

value type_istr(value f)
	{
	if (f->T == 0)
		{
		istr_free(get_istr(f));
		return 0;
		}
	return type_void(f);
	}

/* (readstr str) returns an iterator on the string. */
value type_readstr(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qistr(hold(x));
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}

/* (sgetc in) returns the next single byte from the string, or void if none. */
value type_sgetc(value f)
	{
	return op_getc(f,type_istr,(input)sgetc);
	}

/* (sget in) returns the next UTF-8 character from the string, or void if none.
*/
value type_sget(value f)
	{
	return op_get(f,type_istr,(input)sgetc);
	}
