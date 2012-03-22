#include "value.h"
#include "stack.h"

struct value *stack = 0;

void push(struct value *f)
	{
	struct value *next = create();
	next->L = f;
	next->R = stack;
	stack = next;
	}

void pop(void)
	{
	struct value *next = stack->R;
	stack->L = 0;
	stack->R = 0;
	recycle(stack);
	stack = next;
	}

int arg(struct value *(*T)(struct value *), struct value *f)
	{
	if (f->T == T && !f->L) return 1;
	if (f->T == 0) push(f);
	return 0;
	}
