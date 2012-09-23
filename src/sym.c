#include "value.h"
#include "string.h"
#include "sym.h"

/* LATER 20120922 re-examine this in light of the new symbol method */

void type_name(value f) { type_error(); }

/* Convert name to string. */
void reduce_name_string(value f)
	{
	replace(f,arg(type_name,f->R));
	f->T = type_string;
	}

/* Convert string to name. */
void reduce_string_name(value f)
	{
	replace(f,arg(type_string,f->R));
	f->T = type_name;
	}
