#include "value.h"
#include "basic.h"
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

struct value *arg_is_type(
	struct value *(*T)(struct value *),
	struct value *f)
	{
	struct value *x = f->R;
	if (arg(T,x)) return C;
	if (x->T == 0) return f;
	return F;
	}

#if 0
/* TODO .bench2 is slower, 44s vs 42.5s -- test again tho */
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
#endif
