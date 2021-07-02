#include <convert.h>
#include <ctype.h>
#include <stdlib.h> /* strtod */

/* Convert string to double with strtod(3). */
int str0_double(const char *str, double *val)
	{
	char *end;
	if (str == 0 || *str == 0) return 0;
	*val = strtod(str, &end);
	return (*end == '\0');
	}
