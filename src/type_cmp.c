#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static value op_cmp(value f, int op(int))
	{
	if (!f->L || !f->L->L) return 0;
	{
	value xv = arg(&f->L->R);

	{
	number x = atom(type_num,xv);
	if (x)
		{
		number y = atom(type_num,arg(&f->R));
		if (!y) return 0;
		return Qboolean(op(num_cmp(x,y)));
		}
	}

	{
	string x = atom(type_str,xv);
	if (x)
		{
		string y = atom(type_str,arg(&f->R));
		if (!y) return 0;
		return Qboolean(op(str_cmp(x,y)));
		}
	}

	return 0;
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
