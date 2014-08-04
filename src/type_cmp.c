#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static value op_cmp(value f, int op(int))
	{
	if (!f->L || !f->L->L) return f;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	value g = 0;
	if (is_atom(type_num,x) && is_atom(type_num,y))
		g = Qboolean(op(num_cmp((number)x->R,(number)y->R)));
	else if (is_atom(type_str,x) && is_atom(type_str,y))
		g = Qboolean(op(str_cmp((string)x->R,(string)y->R)));
	drop(x);
	drop(y);
	return g;
	}
	}

static int lt(int x) { return x < 0; }
static int le(int x) { return x <= 0; }
static int eq(int x) { return x == 0; }
static int ne(int x) { return x != 0; }
static int ge(int x) { return x >= 0; }
static int gt(int x) { return x > 0; }

value type_lt(value f) { return op_cmp(f,lt); }
value type_le(value f) { return op_cmp(f,le); }
value type_eq(value f) { return op_cmp(f,eq); }
value type_ne(value f) { return op_cmp(f,ne); }
value type_ge(value f) { return op_cmp(f,ge); }
value type_gt(value f) { return op_cmp(f,gt); }
