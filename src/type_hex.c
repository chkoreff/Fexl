#include <value.h>
#include <str.h>

#include <standard.h>
#include <type_hex.h>
#include <type_str.h>

/* Map number 0-15 to a hex digit. */
static unsigned char nibble_to_hex(unsigned char n)
	{
	if (n < 10)
		return n + '0';
	else
		return n - 10 + 'a';
	}

/* Map hex digit to a number 0-15. */
static unsigned char hex_to_nibble(unsigned char n)
	{
	if (n >= 48 && n <= 57) /* '0'-'9' */
		return n - 48;
	else if (n >= 97 && n <= 102) /* 'a'-'f' */
		return n - 87;
	else if (n >= 65 && n <= 70) /* 'A'-'F' */
		return n - 55;
	else
		return 0;
	}

static string map_raw_bytes_to_hex(string str_raw)
	{
	string str_hex = str_new(2 * str_raw->len);
	int pos;
	for (pos = 0; pos < str_raw->len; pos++)
		{
		unsigned char ch = str_raw->data[pos];
		str_hex->data[2*pos] = nibble_to_hex(ch >> 4);
		str_hex->data[2*pos+1] = nibble_to_hex(ch & 15);
		}
	return str_hex;
	}

static string map_hex_to_raw_bytes(string str_hex)
	{
	string str_raw = str_new(str_hex->len / 2);
	int pos;
	for (pos = 0; pos < str_raw->len; pos++)
		{
		unsigned char hi = hex_to_nibble(str_hex->data[2*pos]);
		unsigned char lo = hex_to_nibble(str_hex->data[2*pos+1]);
		str_raw->data[pos] = (hi << 4) | lo;
		}
	return str_raw;
	}

/* Map raw bytes to their hexadecimal values. */
value type_map_raw_bytes_to_hex(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qstr(map_raw_bytes_to_hex(get_str(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Map a string specified in hexadecimal into the raw bytes. */
value type_map_hex_to_raw_bytes(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qstr(map_hex_to_raw_bytes(get_str(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}