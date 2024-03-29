#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_input.h>
#include <type_istr.h>
#include <type_str.h>

// An istr structure is used to iterate through a string.

static struct istr *istr_new(value x)
	{
	struct istr *in = new_memory(sizeof(struct istr));
	in->pos = 0;
	in->str = x;
	return in;
	}

static void clear_istr(value f)
	{
	struct istr *in = f->v_ptr;
	drop(in->str);
	free_memory(in,sizeof(struct istr));
	}

value Qistr(value x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_istr}};
	return V(type_istr,&clear,(value)istr_new(x));
	}

int sgetc(struct istr *in)
	{
	string str = in->str->v_ptr;
	return in->pos < str->len ? (unsigned char)str->data[in->pos++] : -1;
	}

static int slook(struct istr *in)
	{
	string str = in->str->v_ptr;
	return in->pos < str->len ? (unsigned char)str->data[in->pos] : -1;
	}

value type_istr(value f)
	{
	return type_void(f);
	}

// (readstr str) returns an iterator on the string.
value type_readstr(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qistr(hold(x));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// (sgetc in) returns the next single byte from the string, or void if none.
value type_sgetc(value f)
	{
	return op_getc(f,type_istr,(input)sgetc);
	}

// (sget in) returns the next UTF-8 character from the string, or void if none.
value type_sget(value f)
	{
	return op_get(f,type_istr,(input)sgetc);
	}

// (slook in) returns the next byte from the string without consuming it.
value type_slook(value f)
	{
	value x = arg(f->R);
	if (x->T == type_istr)
		{
		int ch = slook(x->v_ptr);
		if (ch == -1)
			f = hold(Qvoid);
		else
			{
			char c = (char)ch;
			f = Qstr(str_new_data(&c,1));
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
