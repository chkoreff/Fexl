#include <value.h>
#include <basic.h>
#include <output.h>
#include <stdio.h>
#include <str.h>
#include <show.h>
#include <type_long.h>
#include <type_str.h>
#include <type_sym.h>

/* LATER: meta-quote strings or symbols with embedded quotes. */

int show_atom_default(value f)
	{
	if (f->T == type_C)
		printf("C");
	else if (f->T == type_F)
		printf("F");
	else if (f->T == type_I)
		printf("I");
	else if (f->T == type_S)
		printf("S");
	else if (f->T == type_R)
		printf("R");
	else if (f->T == type_L)
		printf("L");
	else if (f->T == type_Y)
		printf("@");
	else if (f->T == type_later)
		printf("later");
	else if (f->T == type_query)
		printf("?");
	else if (f->T == type_concat)
		printf("concat");
	else if (f->T == type_pair)
		printf("pair");
	else if (f->T == type_cons)
		printf("cons");
	else if (f->T == type_halt)
		printf("halt");
	else if (f->T == type_show)
		printf("show");
	else if (f->T == type_print)
		printf("print");
	else if (f->T == type_say)
		printf("say");
	else if (f->T == type_nl)
		printf("nl");
	else if (f->T == type_long_add)
		printf("long_add");
	else if (f->T == type_long_sub)
		printf("long_sub");
	else if (f->T == type_long_mul)
		printf("long_mul");
	else if (f->T == type_long_div)
		printf("long_div");
	else if (f->T == type_order)
		printf("order");
	else if (f->T == type_str)
		printf("\"%s\"",get_str(f)->data);
	else if (f->T == type_long)
		printf("%ld",get_long(f));
	else if (f->T == type_sym)
		{
		struct sym *sym = get_sym(f);
		if (sym->line >= 0)
			printf("{%s}",get_str(sym->name)->data);
		else
			printf("{\"%s\"}",get_str(sym->name)->data);
		}
	else
		return 0;

	return 1;
	}

int (*show_atom)(value f) = show_atom_default;

void show(value f)
	{
	if (f->L)
		{
		printf(f->T == type_sym ? "{" : "(");
		show(f->L);
		printf(" ");
		show(f->R);
		printf(f->T == type_sym ? "}" : ")");
		}
	else if (show_atom(f))
		;
	else
		printf("_");
	}

value type_show(value f)
	{
	if (!f->L) return f;
	show(f->R);
	return I;
	}
