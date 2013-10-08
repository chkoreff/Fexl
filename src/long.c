#include <stdlib.h> /* strtol */
#include "value.h"
#include "basic.h"
#include "long.h"

value type_long(value f)
	{
	return f;
	}

value Qlong(long val)
	{
	return V(type_long,0,(value)val);
	}

long get_long(value f)
	{
	data_type(f,type_long);
	return (long)f->R;
	}

/* Convert string to long and return true if successful. */
int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}
