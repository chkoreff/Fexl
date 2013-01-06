/* This code is for testing and debugging, and is normally not linked in. */
void nl(void)
	{
	putchar('\n');
	}

extern value type_C(value f);
extern value type_S(value f);
extern value type_I(value f);
extern value type_F(value f);
extern value type_R(value f);
extern value type_L(value f);
extern value type_Y(value f);
extern value type_item(value f);
extern value type_query(value f);
extern value type_long(value f);
extern value type_double(value f);
extern value type_string(value f);
extern value type_long_add(value f);
extern value type_string_append(value f);

extern long long_val(value f);
extern double double_val(value f);
extern const char *string_data(value f);

const char *name_of_type(type t)
	{
	if (t == type_C) return "C";
	if (t == type_S) return "S";
	if (t == type_I) return "I";
	if (t == type_F) return "F";
	if (t == type_R) return "R";
	if (t == type_L) return "L";
	if (t == type_Y) return "Y";
	if (t == type_query) return "?";
	if (t == type_item) return "item";
	if (t == type_long) return "long";
	if (t == type_long_add) return "long_add";
	if (t == type_string_append) return "string_append";
	return "_";
	}

void show(value f)
	{
	if (f->L)
		{
		printf("(");
		show(f->L);
		printf(" ");
		show(f->R);
		printf(")");
		}
	else if (f->T == type_long)
		printf("%ld",long_val(f));
	else if (f->T == type_double)
		printf("%.15g", double_val(f));
	else if (f->T == type_string)
		printf("\"%s\"", string_data(f));
	else
		{
		const char *name = name_of_type(f->T);
		printf("%s",name);
		}
	}
