#include <stdio.h>
#include "str.h"

#include "value.h"
#include "basic.h"
#include "lib.h"
#include "long.h"
#include "qstr.h"
#include "show.h"

extern value type_S(value);
extern value type_I(value);
extern value type_L(value);
extern value type_R(value);
extern value type_Y(value);
extern value type_query(value);
extern value type_long(value);
extern value type_double(value);
extern value type_later(value);
extern value type_string(value);
extern value type_form(value);
extern value type_file(value);

void show(value f)
	{
	if (f->L)
		{
		if (f->T == type_form)
			{
			printf("{");
			if (f->L->T == type_string)
				{
				int quoted = (f->R->L->T == type_C);
				const char *format = quoted ? "\"%s\"" : "%s";
				printf(format,get_str(f->L)->data);
				}
			else
				{
				show(f->L);
				printf(" ");
				show(f->R);
				}
			printf("}");
			}
		else
			{
			printf("(");
			show(f->L);
			printf(" ");
			show(f->R);
			printf(")");
			}
		}
	else
		{
		if (f->T == type_C)
			printf("C");
		else if (f->T == type_S)
			printf("S");
		else if (f->T == type_I)
			printf("I");
		else if (f->T == type_L)
			printf("L");
		else if (f->T == type_R)
			printf("R");
		else if (f->T == type_Y)
			printf("@");
		else if (f->T == type_query)
			printf("?");
		else if (f->T == type_later)
			printf("later");
		else if (f->T == type_dlopen)
			printf("dlopen");
		else if (f->T == type_dlsym)
			printf("dlsym");
		else if (f->T == type_Q)
			printf("Q");
		else if (f->T == type_lib)
			{
			long id = (long)f->R;
			printf("lib:%ld", id);
			}
		else if (f->T == type_file)
			{
			long id = (long)f->R;
			printf("file:%ld", id);
			}
		else if (f->T == type_long)
			printf("%ld",get_long(f));
		else if (f->T == type_string)
			printf("\"%s\"",get_str(f)->data);
		else if (f->T == type_double)
			{
			double *p = (double *)f->R;
			printf("%.15g",*p);
			}
		else
			printf("_");
		}
	}

void nl(void)
	{
	putchar('\n');
	}

value type_show(value f)
	{
	if (!f->L) return f;
	printf(":: ");
	show(f->R);
	nl();
	return I;
	}
