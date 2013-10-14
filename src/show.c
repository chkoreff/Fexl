#include <stdio.h>
#include "str.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "qfile.h"
#include "qstr.h"
#include "show.h"

extern value fexl_S(value);
extern value fexl_I(value);
extern value fexl_L(value);
extern value fexl_R(value);
extern value fexl_Y(value);
extern value fexl_query(value);
extern value type_long(value);
extern value type_double(value);
extern value fexl_later(value);
extern value type_string(value);
extern value type_form(value);
extern value type_file(value);
extern value type_lib(value f);
extern value fexl_dlopen(value f);
extern value fexl_dlsym(value f);
extern value fexl_Q(value f);
extern value fexl_fwrite(value f);
extern value fexl_long_string(value f);
extern value type_var(value f);

void show(value f)
	{
	if (f->L)
		{
		if (f->T == type_form)
			{
			printf("{");
			if (f->L->T == type_string)
				{
				int quoted = (f->R->L->T == fexl_C);
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
		if (f->T == fexl_C)
			printf("C");
		else if (f->T == fexl_S)
			printf("S");
		else if (f->T == fexl_I)
			printf("I");
		else if (f->T == fexl_L)
			printf("L");
		else if (f->T == fexl_R)
			printf("R");
		else if (f->T == fexl_Y)
			printf("@");
		else if (f->T == fexl_fwrite)
			printf("fwrite");
		else if (f->T == fexl_long_string)
			printf("long_string");
		else if (f->T == fexl_query)
			printf("?");
		else if (f->T == fexl_later)
			printf("later");
		else if (f->T == fexl_dlopen)
			printf("dlopen");
		else if (f->T == fexl_dlsym)
			printf("dlsym");
		else if (f->T == fexl_Q)
			printf("Q");
		else if (f->T == type_lib)
			{
			long id = (long)f->R;
			printf("lib:%ld", id);
			}
		else if (f->T == type_file)
			printf("file:%d", fileno(get_file(f)));
		else if (f->T == type_long)
			printf("%ld",get_long(f));
		else if (f->T == type_string)
			printf("\"%s\"",get_str(f)->data);
		else if (f->T == type_double)
			{
			double *p = (double *)f->R;
			printf("%.15g",*p);
			}
		else if (f->T == type_var)
			{
			printf("var:");
			show(f->R);
			}
		else
			printf("_");
		}
	}

void nl(void)
	{
	putchar('\n');
	}

value fexl_show(value f)
	{
	if (!f->L) return f;
	show(f->R); nl();
	return I;
	}
