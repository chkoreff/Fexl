#if 0
/*
NOTE we only keep this code around for tracing during development.  It's
normally disabled with "if 0" at the top.  To enable it, change it to "if 1".
*/
#include <stdio.h>
#include "value.h"
#include "basic.h"
#include "double.h"
#include "io.h"
#include "long.h"
#include "resolve.h"
#include "run.h"
#include "show.h"
#include "string.h"
#include "sym.h"

extern value type_lambda(value);
extern value fexl_parse(value);
extern value fexl_resolve(value);

void quote_string_put(value f)
	{
	char *quote = "\"";
	print(quote);
	string_put(f);
	print(quote);
	}

void show_type(type T)
	{
	char *name =
	T == fexl_C ? "C" :
	T == fexl_S ? "S" :
	T == fexl_I ? "I" :
	T == fexl_R ? "R" :
	T == fexl_L ? "L" :
	T == fexl_Y ? "Y" :
	T == fexl_query ? "?" :
	T == fexl_item ? "item" :
	T == fexl_pair ? "pair" :

	T == fexl_long_add ? "long_add" :
	T == fexl_long_sub ? "long_sub" :
	T == fexl_long_mul ? "long_mul" :
	T == fexl_long_div ? "long_div" :
	T == fexl_is_long ? "is_long" :
	T == fexl_long_double ? "long_double" :

	T == fexl_double_add ? "double_add" :
	T == fexl_double_sub ? "double_sub" :
	T == fexl_double_mul ? "double_mul" :
	T == fexl_double_div ? "double_div" :

	T == fexl_string_append ? "string_append" :

	T == fexl_parse ? "parse" :
	T == fexl_nl ? "nl" :
	T == fexl_string_put ? "string_put" :
	T == fexl_resolve ? "resolve" :
	T == type_lambda ? "lam" :
	"_";

	print(name);
	}

void show(value f)
	{
	if (cur_depth > max_depth)
		{
		print("...");
		return;
		}
	cur_depth++;

	if (f->L)
		{
		print("(");
		show(f->L);
		print(" ");
		show(f->R);
		print(")");
		}
	else if (f->T == type_long)
		printf("%ld", get_long(f));
	else if (f->T == type_double)
		printf("%.15g", get_double(f));
	else if (f->T == type_string)
		quote_string_put(f);
	else if (f->T == type_name)
		string_put(f);
	else
		show_type(f->T);

	cur_depth--;
	}
#endif
