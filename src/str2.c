#include <string.h>
#include "memory.h"
#include "str.h"

/* Return the number of starting bytes the strings have in common. */
long str_common(struct str *x, struct str *y)
	{
	long min_len = x->len < y->len ? x->len : y->len;

	const char *xd = x->data;
	const char *yd = y->data;

	long len = 0;
	while (len < min_len)
		{
		if (xd[len] != yd[len]) break;
		len++;
		}
	return len;
	}
