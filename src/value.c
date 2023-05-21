#include <memory.h>

#include <value.h>

value free_list = 0;

value new_exp(struct type *type)
	{
	value exp = free_list;
	if (exp)
		free_list = exp->next;
	else
		exp = new_memory(sizeof(struct value));

	exp->N = 1;
	exp->T = type;
	return exp;
	}

value hold(value exp)
	{
	exp->N++;
	return exp;
	}

void drop(value exp)
	{
	if (--exp->N == 0)
		{
		exp->T->clear(exp);
		exp->next = free_list;
		free_list = exp;
		}
	}

void clear_free_list(void)
	{
	while (free_list)
		{
		value exp = free_list;
		free_list = exp->next;
		free_memory(exp, sizeof(struct value));
		}
	}

value no_step(value pair)
	{
	(void)pair;
	return 0;
	}

value no_apply(value fun, value arg, value cx)
	{
	(void)fun;
	(void)arg;
	(void)cx;
	return 0;
	}

void no_clear(value exp)
	{
	(void)exp;
	}

/* Reduce a pair until done.  A pair is an expression combined with a context
which defines any unbound references in the expression.  The context is a list
of zero or more pairs.  Each unbound reference in the expression refers by
position to a pair in the context, numbering from zero.

pair => A(exp,context)

exp => R(unsigned long pos)
exp => L(exp body)
exp => A(exp fun, exp arg)

context => R(0)             // empty context
context => A(pair,context)  // context with pair at position 0
*/
value eval(value pair)
	{
	while (1)
		{
		value next = pair->L->T->step(pair);
		if (next == 0) break;
		drop(pair);
		pair = next;
		}
	return pair;
	}
