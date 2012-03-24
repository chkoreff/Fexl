#include "value.h"
#include "stack.h"

/* The global stack used for tracking recursive goals during evaluation. */
struct value *stack = 0;

/* Push a value on the global stack. */
void push(struct value *f)
	{
	struct value *next = create();
	next->L = f;
	next->R = stack;
	stack = next;
	}

/* Pop a value from the global stack. */
void pop(void)
	{
	struct value *next = stack->R;
	stack->L = 0;
	stack->R = 0;
	recycle(stack);
	stack = next;
	}

/* Push a value on the designated list. */
void push_list(struct value **list, struct value *f)
	{
	struct value *save = stack;
	stack = *list;
	push(f);
	*list = stack;
	stack = save;
	}

/* Pop a value from the designated list. */
void pop_list(struct value **list)
	{
	struct value *save = stack;
	stack = *list;
	pop();
	*list = stack;
	stack = save;
	}

int arg(struct value *(*T)(struct value *), struct value *f)
	{
	if (f->T == T && !f->L) return 1;
	if (f->T == 0) push(f);
	return 0;
	}

/* Used for functions of the form (is_T x yes no), which checks the type of x
and returns yes if it's type T or no otherwise. */
struct value *arg_is_type(
	struct value *(*T)(struct value *),
	struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	struct value *x = f->L->L->R;
	if (arg(T,x)) return f->L->R;
	if (x->T == 0) return f;
	return f->R;
	}
