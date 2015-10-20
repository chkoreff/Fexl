#include <value.h>
#include <basic.h>
#include <input.h>
#include <num.h>
#include <stdio.h>
#include <str.h>
#include <type_file.h>
#include <type_input.h>
#include <type_num.h>
#include <type_str.h>

/*
Reference for UTF-8: http://czyborra.com/utf/

The binary representation of the character's integer value is thus simply
spread across the bytes and the number of high bits set in the lead byte
announces the number of bytes in the multibyte sequence:

bytes | bits | representation
    1 |    7 | 0vvvvvvv
    2 |   11 | 110vvvvv 10vvvvvv
    3 |   16 | 1110vvvv 10vvvvvv 10vvvvvv
    4 |   21 | 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
    5 |   26 | 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
    6 |   31 | 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv

(Actually, UTF-8 continues to represent up to 31 bits with up to 6 bytes, but
it is generally expected that the one million code points of the 20 bits
offered by UTF-16 and 4-byte UTF-8 will suffice to cover all characters and
that we will never get to see any Unicode character definitions beyond that.)
*/

/* Return the width of a UTF-8 character whose leading byte is ch. */
static unsigned char char_width(unsigned char ch)
	{
	unsigned char mask = 0x80;
	unsigned char n = 0;
	while (ch & mask)
		{
		n++;
		mask >>= 1;
		}
	if (n == 0) return 1;
	return n;
	}

/* Get the next UTF-8 character from the current input. */
static string get_utf8(void)
	{
	int ch = getd();
	if (ch == -1) return 0;
	{
	char buf[6];
	unsigned pos = 0;
	unsigned len = char_width(ch);

	if (len >= sizeof(buf))
		len = sizeof(buf);

	while (1)
		{
		buf[pos++] = (char)ch;
		if (pos >= len) break;
		ch = getd();
		if (ch == -1) return 0;
		}
	return str_new_data(buf,len);
	}
	}

/* get = {ch}, where ch is the next UTF-8 character from stdin, or void if
no more characters. */
value type_get(value f)
	{
	string ch = get_utf8();
	(void)f;
	return single(ch ? Qstr(ch) : Q(type_void));
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
		double yn = *((number)data(y));
		if (yn >= 0)
			{
			string xs = data(x);
			unsigned long pos = yn;
			if (pos < xs->len)
				{
				char n = char_width(xs->data[pos]);
				reduce_num(f, num_new_ulong(n));
				return 0;
				}
			}
		}
	reduce_void(f);
	return 0;
	}
	}

string cur_text;
unsigned long cur_pos;
int getd_string(void)
	{
	return cur_pos < cur_text->len ? cur_text->data[cur_pos++] : -1;
	}

/* (get_from source content)
Evaluate the content with the current input temporarily set to the source,
which may be a file or a string.
LATER Might allow source to be an arbitrary list.
*/
value type_get_from(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_file)
		{
		input save_getd = getd;
		void *save_cur_in = cur_in;
		FILE *fh = data(x);

		get_from(fh);
		f = eval(hold(f->R));

		getd = save_getd;
		cur_in = save_cur_in;
		return f;
		}
	else if (x->T == type_str)
		{
		input save_getd = getd;
		string save_cur_text = cur_text;
		unsigned long save_cur_pos = cur_pos;

		getd = getd_string;
		cur_text = data(x);
		cur_pos = 0;

		f = eval(hold(f->R));

		getd = save_getd;
		cur_text = save_cur_text;
		cur_pos = save_cur_pos;
		return f;
		}
	reduce_void(f);
	return 0;
	}
	}
