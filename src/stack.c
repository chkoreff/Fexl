#include "value.h"
#include "stack.h"

/* The global stack used for tracking recursive goals during evaluation. */
value stack = 0;

/* Push a value on the global stack. */
void push(value f)
	{
	value next = create();
	next->L = f;
	next->R = stack;
	stack = next;
	}

/* Pop a value from the global stack. */
void pop(void)
	{
	value next = stack->R;
	stack->L = 0;
	stack->R = 0;
	recycle(stack);
	stack = next;
	}

/* Push a value on the designated list. */
void push_list(value *list, value f)
	{
	value save = stack;
	stack = *list;
	push(f);
	*list = stack;
	stack = save;
	}

/* Pop a value from the designated list. */
void pop_list(value *list)
	{
	value save = stack;
	stack = *list;
	pop();
	*list = stack;
	stack = save;
	}

int arg(type T, value f)
	{
	if (f->T == T && !f->L) return 1;
	if (f->T == 0) push(f);
	return 0;
	}

/* Used for functions of the form (is_T x yes no), which checks the type of x
and returns yes if it's type T or no otherwise. */
value arg_is_type(type T, value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	value x = f->L->L->R;
	if (arg(T,x)) return f->L->R;
	if (x->T == 0) return f;
	return f->R;
	}
