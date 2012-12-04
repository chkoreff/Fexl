/*
NOTE:  This module is not ordinarily linked into fexl, but it's handy to keep
around for tracing and debugging purposes.
*/

extern value type_query(value f);
extern value type_file(value f);
extern value type_long_add(value f);
extern value type_long_mul(value f);
extern value type_name(value f);
extern value type_open(value f);

void nl(void)
	{
	putchar('\n');
	}

static char *name_of_type(type t)
	{
	if (t == 0) return "A";
	if (t == type_C) return "C";
	if (t == type_F) return "F";
	if (t == type_S) return "S";
	if (t == type_I) return "I";
	if (t == type_R) return "R";
	if (t == type_L) return "L";
	if (t == type_Y) return "Y";
	if (t == type_string) return "string";
	if (t == type_double) return "double";
	if (t == type_long) return "long";
	if (t == type_item) return "item";
	if (t == type_name) return "name";
	if (t == type_open) return "open";
	if (t == type_long_add) return "long_add";
	if (t == type_long_mul) return "long_mul";
	if (t == type_query) return "query";
	if (t == type_file) return "file";
	return "_";
	}

const int cformat = 0;
/*
0 : Show in normal bracketed format.
1 : Show in C language format where possible.
*/

/* LATER we can make the format nicer now that we're just promoting types
upward, instead of using X1, X2, etc. */

void show(value f)
	{
	if (f == 0)
		{
		printf("_");
		return;
		}

	if (f->L == 0)
		{
		if (f->R == 0)
			printf("%s",name_of_type(f->T));
		else
			{
			int bracket = 1;
			if (cformat && (f->T == type_string || f->T == type_double
					|| f->T == type_long))
				bracket = 0;

			if (bracket)
				{
				printf("[");
				printf("%s",name_of_type(f->T));
				printf(" ");
				}

			if (f->T == type_string || f->T == type_name)
				printf("\"%s\"", string_data(f));
			else if (f->T == type_double)
				printf("%.15g", double_val(f));
			else if (f->T == type_long)
				printf("%ld", long_val(f));
			else
				printf("_");

			if (bracket)
				printf("]");
			}
		}
	else if (cformat && f->T == 0)
		{
		printf("A(");
		show(f->L);
		if (f->R)
			{
			printf(",");
			show(f->R);
			}
		printf(")");
		}
	else
		{
		printf("[");
		printf("%s",name_of_type(f->T));
		printf(" ");
		show(f->L);
		if (f->R)
			{
			printf(" ");
			show(f->R);
			}
		printf("]");
		}
	}
