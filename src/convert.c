#include <convert.h>
#include <stdlib.h> /* strtod */

int str0_double(const char *str, double *val)
	{
	if (str && *str)
		{
		char *end;
		*val = strtod(str, &end);
		if (*end == '\0')
			return 1;
		}
	return 0;
	}
