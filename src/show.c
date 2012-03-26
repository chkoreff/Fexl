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

/*TODO won't need most of these header files when you do "show" in Fexl */

/*TODO won't need these externs either */
extern value fexl_lambda(value);
extern value fexl_show(value);
extern value fexl_parse(value);

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
		printf("%.16g", get_double(f));
	else if (f->T == type_string)
		quote_string_put(f);
	else if (f->T == type_name)
		string_put(f);
	else
		{
		char *name =
		f->T == fexl_C ? "C" :
		f->T == fexl_S ? "S" :
		f->T == fexl_I ? "I" :
		f->T == fexl_R ? "R" :
		f->T == fexl_L ? "L" :
		f->T == fexl_Y ? "Y" :
		f->T == fexl_query ? "?" :

		f->T == fexl_long_add ? "long_add" :
		f->T == fexl_long_sub ? "long_sub" :
		f->T == fexl_long_mul ? "long_mul" :
		f->T == fexl_long_div ? "long_div" :
		f->T == fexl_is_long ? "is_long" :
		f->T == fexl_long_double ? "long_double" :

		f->T == fexl_double_add ? "double_add" :
		f->T == fexl_double_sub ? "double_sub" :
		f->T == fexl_double_mul ? "double_mul" :
		f->T == fexl_double_div ? "double_div" :

		f->T == fexl_string_append ? "string_append" :

		f->T == fexl_show ? "show" :
		f->T == fexl_parse ? "parse" :
		f->T == fexl_nl ? "nl" :
		f->T == fexl_string_put ? "string_put" :
		f->T == fexl_resolve ? "resolve" :
		f->T == fexl_lambda ? "lam" :
		"_";

		print(name);
		}

	cur_depth--;
	}

/*TODO Do the show routine in Fexl itself. (see type.c) */
value fexl_show(value f)
	{
	if (!f->L->L) return f;
	show(f->L->R);
	return f->R;
	}
