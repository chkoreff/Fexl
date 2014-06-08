#include <str.h>

#include <input.h>
#include <stdio.h>

/*
Get the next character from stdin.  This reads multi-byte sequences in the
UTF-8 format.  I looked in the C Reference Standard for a standard routine to
do this, but I haven't found anything yet, so I just wrote it myself.

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

string get_stdin(void)
	{
	int ch = getchar();
	if (ch == EOF) return str_new_data("",0);

	{
	int mask = 0x80;
	char buf[6];

	buf[0] = (char)ch;
	if ((ch & mask) == 0)
		return str_new_data(buf,1);

	/* Read another byte for each high bit after the first one. */
	{
	unsigned width = 1;
	while (1)
		{
		mask >>= 1;
		if ((ch & mask) == 0) break;
		{
		int next_ch = getchar();
		if (next_ch == EOF) break;
		buf[width++] = (char)next_ch;
		}
		if (width >= sizeof(buf)) break;
		}
	return str_new_data(buf,width);
	}
	}
	}

/* Read data from the current source. */
string (*getd)(void) = get_stdin;
