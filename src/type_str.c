#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_num.h>
#include <type_str.h>

void type_str(value f)
	{
	replace_void(f);
	}

value Qstr(string p)
	{
	return D(type_str,(type)str_free,(value)p);
	}

string get_str(value f)
	{
	return (string)get_D(f,type_str);
	}

void replace_str(value f, string x)
	{
	replace_D(f,type_str,(type)str_free,(value)x);
	}

/* (. x y) is the concatenation of strings x and y. */
void type_concat(value f)
	{
	if (f->L->L)
		{
		string x = get_str(eval(f->L->R));
		string y = get_str(eval(f->R));
		if (x == 0 || y == 0)
			replace_void(f);
		else
			replace_str(f,str_concat(x,y));
		}
	}

/* (length x) is the length of string x */
void type_length(value f)
	{
	string x = get_str(eval(f->R));
	if (x == 0)
		replace_void(f);
	else
		replace_num(f,num_new_ulong(x->len));
	}

/* (slice str pos len) returns the len bytes of str starting at pos, or 0 bytes
if pos or len exceeds the bounds of str. */
void type_slice(value f)
	{
	if (f->L->L && f->L->L->L)
		{
		string str = get_str(eval(f->L->L->R));
		number y = get_num(eval(f->L->R));
		number z = get_num(eval(f->R));
		if (str == 0 || y == 0 || z == 0)
			replace_void(f);
		else
			{
			long pos = (long)*y;
			long len = (long)*z;

			if (pos < 0 || len < 0
				|| pos >= str->len
				|| len > str->len
				|| pos > str->len - len
				)
				{
				pos = 0;
				len = 0;
				}

			replace_str(f,str_new_data(str->data + pos,len));
			}
		}
	}
