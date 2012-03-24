#include "memory.h"
#include "value.h"
#include "long.h"
#include "run.h"
#include "stack.h"

struct value *type_put_max_steps(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	if (!arg(type_long,x)) return f;

	max_steps = get_long(x);
	return f->R;
	}

struct value *type_put_max_bytes(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	if (!arg(type_long,x)) return f;

	max_bytes = get_long(x);
	return f->R;
	}

struct value *type_get_max_steps(struct value *f)
	{
	return A(f->R,Qlong(max_steps));
	}

struct value *type_get_max_bytes(struct value *f)
	{
	return A(f->R,Qlong(max_bytes));
	}

#if 0
/* TODO a combinator to access envp */
int i = 0;
while (1)
	{
	char *s = envp[i];
	if (s == 0) break;
	i++;
	}

/* TODO a combinator to access argv */
#endif

/*
TODO a combinator to fork another Fexl and read stdout and stderr so you can
run other functions safely without affecting the parent, and gather stdout and
stderr too.
*/
