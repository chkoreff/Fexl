#include <num.h>
#include <str.h>
#include <convert.h>
#include <format.h>
#include <stdlib.h> /* strtod */

string num_str(number x)
	{
	return str_new_data0(format_double(*x));
	}

number str0_num(const char *str, int *ok)
	{
	if (str && *str)
		{
		char *end;
		double val = strtod(str, &end);
		if (*end == '\0')
			{
			*ok = 1;
			return num_new_double(val);
			}
		}
	*ok = 0;
	return 0;
	}
