#include <value.h>
#include <basic.h>
#include <input.h>
#include <str.h>
#include <type_input.h>
#include <type_str.h>

/*
Get the next logical character from the current input.  This reads multi-byte
sequences in the UTF-8 format.

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
static string get_utf8(void)
	{
	char buf[6];
	int mask = 0x80;
	unsigned width = 1;

	int ch = getd();
	if (ch == -1) return 0;

	buf[0] = (char)ch;
	if ((ch & mask) == 0)
		return str_new_data(buf,1);

	/* Read another byte for each high bit after the first one. */
	while (1)
		{
		int next_ch;
		mask >>= 1;
		if ((ch & mask) == 0) break;
		next_ch = getd();
		if (next_ch == -1) break;
		buf[width++] = (char)next_ch;
		if (width >= sizeof(buf)) break;
		}
	return str_new_data(buf,width);
	}

/* get = {ch}, where ch is the next UTF-8 character from the input, or void if
no more characters. */
value type_get(value f)
	{
	string ch = get_utf8();
	(void)f;
	return A(Q(type_single), ch ? Qstr(ch) : Q(type_void));
	}

/* LATER get_from_file get_from_input get_from_string get_from_source */

/*LATER note that this works and is cool:
~END
\ch=get
put "ch = " put ch nl
\\X
END

Using get_from_source, we can make a version of that which reads *only* from
the source file, whether it's stdin or not.
*/
