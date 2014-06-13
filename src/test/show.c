#include <value.h>

#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_var.h>

void put_quoted(const char *s)
	{
	put("\"");
	put(s);
	put("\"");
	}

void show_atom(value f)
	{
	if (f->T == type_C)
		put("C");
	else if (f->T == type_F)
		put("F");
	else if (f->T == type_I)
		put("I");
	else if (f->T == type_S)
		put("S");
	else if (f->T == type_R)
		put("R");
	else if (f->T == type_L)
		put("L");
	else if (f->T == type_Y)
		put("@");
	else if (f->T == type_query)
		put("query");
	else if (f->T == type_cons)
		put("cons");
	else if (f->T == type_say)
		put("say");
	else if (f->T == type_str)
		put_quoted(get_str(f)->data);
	else if (f->T == type_num)
		put_num(get_num(f));
	else if (f->T == type_var)
		{
		put("var:");
		show(f->R);
		}
	else if (f->T == type_sym)
		{
		struct sym *sym = get_sym(f);
		put("{");
		(sym->quoted ? put_quoted : put)(get_str(sym->name)->data);
		put("}");
		}
	else
		put("_");
	}

void show(value f)
	{
	if (f->L)
		{
		put(f->T == type_sym ? "{" : "(");
		show(f->L);
		put(" ");
		show(f->R);
		put(f->T == type_sym ? "}" : ")");
		}
	else
		show_atom(f);
	}

value type_show(value f)
	{
	if (!f->L) return 0;
	show(f->R);
	return hold(I);
	}
