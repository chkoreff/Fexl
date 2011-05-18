#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "abstract.h"
#include "parse.h"
#include "resolve.h"
#include "string.h"
#include "var.h"

#include "C.h"
#include "S.h"
#include "I.h"
#include "L.h"
#include "R.h"
#include "Y.h"

#include "char_get.h"
#include "char_put.h"

#include "double.h"
#include "double_add.h"
#include "double_sub.h"
#include "double_mul.h"
#include "double_div.h"
#include "double_from.h"
#include "double_compare.h"

#include "long.h"
#include "long_add.h"
#include "long_sub.h"
#include "long_mul.h"
#include "long_div.h"
#include "long_from.h"
#include "long_compare.h"

#include "string_append.h"
#include "string_compare.h"
#include "string_from.h"
#include "string_put.h"
#include "string_from_char.h"
#include "string_at.h"
#include "string_len.h"
#include "string_slice.h"

/*
LATER do some standard handy definitions such as:

\print = (\item string_put (string_from item))

\nl = (string_put "
")
*/

/*
LATER Template expansion function, e.g.:

\env =
	(
	\sym
	string_eq sym "x" x;
	string_eq sym "y" y;
	string_eq sym "product" (long_mul x y);
	sym
	)

\template = "
The sum of ~x and ~y is ~(add x y).
The product of ~x and ~y is ~(product).
Thank you.
"
print (expand env "~" template); nl;
*/

/*
LATER We'll have the power to do embedded environments, that is, we parse the
user's Fexl program and resolve symbols inside a suitably restricted
environment.  When Fexl first starts, it has access to all functions, and it
can tune specific options such as max_steps and max_bytes within Fexl itself.
Then it can safely turn over control to a sandboxed user function.
*/

static struct value *var_stack = 0;

void push_var(struct value *var, struct value *def)
	{
	var_stack = new_value(&type_var, new_value(&type_var,var,def), var_stack);
	}

void pop_var(void)
	{
	if (!var_stack) return;
	struct value *tail = var_stack->R;
	hold(tail);
	drop(var_stack);
	tail->N--;
	var_stack = tail;
	}

void start_resolve(void)
	{
	push_var(new_string("C"), &value_C);
	push_var(new_string("S"), &value_S);
	push_var(new_string("I"), &value_I);
	push_var(new_string("L"), &value_L);
	push_var(new_string("R"), &value_R);
	push_var(new_string("Y"), &value_Y);

	push_var(new_string("long_add"), &value_long_add);
	push_var(new_string("long_sub"), &value_long_sub);
	push_var(new_string("long_mul"), &value_long_mul);
	push_var(new_string("long_div"), &value_long_div);
	push_var(new_string("long_from"), &value_long_from);
	push_var(new_string("long_compare"), &value_long_compare);

	push_var(new_string("double_add"), &value_double_add);
	push_var(new_string("double_sub"), &value_double_sub);
	push_var(new_string("double_mul"), &value_double_mul);
	push_var(new_string("double_div"), &value_double_div);
	push_var(new_string("double_from"), &value_double_from);
	push_var(new_string("double_compare"), &value_double_compare);

	push_var(new_string("string_append"), &value_string_append);
	push_var(new_string("string_compare"), &value_string_compare);
	push_var(new_string("string_from"), &value_string_from);
	push_var(new_string("string_put"), &value_string_put);
	push_var(new_string("string_from_char"), &value_string_from_char);
	push_var(new_string("string_at"), &value_string_at);
	push_var(new_string("string_len"), &value_string_len);
	push_var(new_string("string_slice"), &value_string_slice);

	push_var(new_string("char_get"), &value_char_get);
	push_var(new_string("char_put"), &value_char_put);
	}

void finish_resolve(void)
	{
	drop(var_stack);
	}

struct value *resolve(struct value *sym)
	{
	struct value *pos = var_stack;
	while (pos)
		{
		struct value *top = pos->L;
		if (sym_eq(top->L,sym)) return top->R;
		pos = pos->R;
		}

	struct atom_string *atom = (struct atom_string *)sym->R;

	/* Simple quoted strings */
	if (atom->len >= 2
			&& atom->data[0] == '"'
			&& atom->data[atom->len-1] == '"')
		return new_chars(atom->data + 1, atom->len - 2);

	/* Complex quoted strings */
	if (atom->len >= 4
		&& atom->data[0] == '~')
		{
		int term_len = 0;
		while (1)
			{
			if (1 + term_len >= atom->len) break;
			if (isspace(atom->data[1 + term_len]))
				{
				if (memcmp(atom->data + atom->len - term_len, atom->data + 1,
					term_len) == 0)
					return new_chars(atom->data + term_len + 2,
						atom->len - term_len - 2 - term_len);
				break;
				}
			term_len++;
			}
		}

	/* Integer number (long) */
	{
	char *end;
	long num = strtol(atom->data, &end, 10);
	if (end - atom->data == atom->len)
		return new_long(num);
	}

	/* Floating point number (double) */
	{
	char *end;
	double num = strtod(atom->data, &end);
	if (end - atom->data == atom->len)
		return new_double(num);
	}

	fprintf(stderr, "Undefined variable %s on line %d\n", atom->data, line_no);
	return new_value(sym->T,sym->L,sym->R);
	}
