#include <value.h>
#include <basic.h>
#include <num.h>
#include <stdio.h>
#include <str.h>
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

/* Get the next UTF-8 character from stdin. */
static string get_utf8(void)
	{
	int ch = getchar();
	if (ch == -1) return 0;
	char buf[6];
	unsigned pos = 0;
	unsigned len = char_width(ch);

	if (len >= sizeof(buf))
		len = sizeof(buf);

	while (1)
		{
		buf[pos++] = (char)ch;
		if (pos >= len) break;
		ch = getchar();
		if (ch == -1) return 0;
		}
	return str_new_data(buf,len);
	}

/* get = {ch}, where ch is the next UTF-8 character from stdin, or void if
no more characters. */
value type_get(value f)
	{
	string ch = get_utf8();
	(void)f;
	return A(Q(type_single), ch ? Qstr(ch) : Q(type_void));
	}

/* (char_width str pos) Return the width of the UTF-8 character which starts at
the given position. */
value type_char_width(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
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
				replace_num(f, num_new_ulong(n));
				}
			else
				replace_void(f);
			}
		else
			replace_void(f);
		}
	else
		replace_void(f);

	drop(x);
	drop(y);
	return f;
	}
