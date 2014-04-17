#include <value.h>
#include <basic.h>
#include <output.h>
#include <stdio.h>
#include <str.h>
#include <type_long.h>
#include <type_str.h>

/* I removed the tail recursion in the cases for fexl_item and type_var. */
void print(value f)
	{
	while (1)
		{
		if (f->T == type_str)
			{
			struct str *str = get_str(f);
			size_t count = fwrite(str->data, 1, str->len, stdout);
			(void)count;  /* Ignore the return count. */
			}
		else if (f->T == type_long)
			printf("%ld", get_long(f));
		#if 0
		else if (f->T == type_double)
			printf("%.15g", get_double(f));
		#endif
		else if (f->T == type_cons)
			{
			if (!f->L || !f->L->L) bad_type();
			print(eval(f->L->R));
			drop(f);
			f = eval(f->R);
			continue;
			}
		else if (f->T == type_C)
			;
		#if 0
		else if (f->T == type_var)
			{
			drop(f);
			f = eval(f->R);
			continue;
			}
		#endif
		else
			bad_type();

		drop(f);
		break;
		}
	}

void nl(void)
	{
	putchar('\n');
	}

value type_nl(value f)
	{
	(void)f;
	putchar('\n');
	return I;
	}

value type_print(value f)
	{
	if (!f->L) return f;
	print(eval(f->R));
	return I;
	}

value type_say(value f)
	{
	if (!f->L) return f;
	print(eval(f->R)); nl();
	return I;
	}
