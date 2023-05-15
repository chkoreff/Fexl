#include <memory.h>

#include <value.h>
#include <ref.h>
#include <lam.h>
#include <app.h>

value free_list = 0;

value new_exp(struct type *type)
	{
	value exp = free_list;
	if (exp)
		free_list = exp->next;
	else
		exp = new_memory(sizeof(struct value));

	exp->N = 1;
	exp->type = type;
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
		exp->type->clear(exp);
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

value eval(value pair)
	{
	while (1)
		{
		value next = pair->app.fun->type->step(pair);
		if (next == 0) return pair;
		drop(pair);
		pair = next;
		}
	}
