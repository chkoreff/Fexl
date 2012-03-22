#include <stdio.h>
#include "value.h"
#include "basic.h"
#include "die.h"
#include "double.h"
#include "io.h"
#include "long.h"
#include "parse_string.h"
#include "resolve.h"
#include "run.h"
#include "show.h"
#include "string.h"
#include "sym.h"

void print(const char *s)
	{
	fputs(s, stdout);
	}

/*TODO full string quoting */
void show_string(struct value *f)
	{
	char *quote = "\"";
	print(quote);
	string_put(f);
	print(quote);
	}

void show(struct value *f)
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
		show_string(f);
	else if (f->T == type_name)
		string_put(f);
	else
		{
		char *name =
		f->T == type_C ? "C" :
		f->T == type_S ? "S" :
		f->T == type_I ? "I" :
		f->T == type_R ? "R" :
		f->T == type_L ? "L" :
		f->T == type_Y ? "Y" :
		f->T == type_query ? "?" :

		f->T == type_long_add ? "long_add" :
		f->T == type_long_sub ? "long_sub" :
		f->T == type_long_mul ? "long_mul" :
		f->T == type_long_div ? "long_div" :
		f->T == type_is_long ? "is_long" :
		f->T == type_long_double ? "long_double" :

		f->T == type_double_add ? "double_add" :
		f->T == type_double_sub ? "double_sub" :
		f->T == type_double_mul ? "double_mul" :
		f->T == type_double_div ? "double_div" :

		f->T == type_string_append ? "string_append" :

		f->T == type_show ? "show" :
		f->T == type_parse ? "parse" :
		f->T == type_nl ? "nl" :
		f->T == type_string_put ? "string_put" :
		f->T == type_resolve ? "resolve" :
		f->T == type_lambda ? "lam" :
		"_";

		print(name);
		}

	cur_depth--;
	}

/*TODO perhaps do this in Fexl itself */
struct value *type_show(struct value *f)
	{
	if (!f->L->L) return f;
	show(f->L->R);
	return f->R;
	}
