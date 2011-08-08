#include <string.h>
#include "memory.h"
#include "value.h"
#include "string.h"

void hold_string(struct value *value)
	{
	if (value->L)
		hold_both(value);
	else
		{
		struct atom_string *atom = (struct atom_string *)value->R;
		atom->N++;
		}
	}

void drop_string(struct value *value)
	{
	if (value->L)
		drop_both(value);
	else
		{
		struct atom_string *atom = (struct atom_string *)value->R;
		if (--atom->N <= 0)
			free_memory(atom, sizeof(struct atom_string) + atom->len + 1);
		}
	}

struct type type_string = { reduce_none, hold_string, drop_string, "string" };

/*
Make a string capable of holding len characters plus a trailing NUL byte.  Then
if data is non-zero, copy len characters from data into the new string and add
the trailing NUL byte.  If data is zero, don't copy anything.

Note that a string can hold any characters, including NUL bytes themselves, but
include a trailing NUL byte so we can call system functions without having to
copy the characters into a buffer and add the NUL byte there.
*/
struct value *new_chars(const char *data, int len)
	{
	if (len < 0) len = 0;

	struct atom_string *atom = new_memory(sizeof(struct atom_string) + len + 1);

	atom->N = 0;
	atom->len = len;
	if (data) memcpy(atom->data, data, len);
	atom->data[len] = '\000'; /* add trailing NUL byte */

	return new_value(&type_string, 0, (struct value *)atom);
	}

/* Make a string from null-terminated data. */
struct value *new_string(const char *data)
	{
	return new_chars(data, strlen(data));
	}

/* Evaluate a value of type string. */
struct atom_string *evaluate_string(struct value *value)
	{
	evaluate_type(value, &type_string);
	return (struct atom_string *)value->R;
	}
