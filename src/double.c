#include <stdlib.h> /* strtod */
#include "memory.h"

#include "value.h"
#include "basic.h"
#include "double.h"

value type_double(value f)
	{
	if (!f->N)
		{
		double *p = (double *)f->R;
		free_memory(p, sizeof(double));
		}
	return f;
	}

value Qdouble(double val)
	{
	double *p = new_memory(sizeof(double));
	*p = val;
	return V(type_double,0,(value)p);
	}

double get_double(value f)
	{
	data_type(f,type_double);
	double *p = (double *)f->R;
	return *p;
	}

/* Convert string to double and return true if successful. */
int string_double(const char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}
