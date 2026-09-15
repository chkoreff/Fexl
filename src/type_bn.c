#include <stdint.h>

#include <str.h>
#include <types.h>
#include <value.h>

#include <basic.h>
#include <bn.h>
#include <type_bn.h>
#include <type_num.h>
#include <type_str.h>

value type_bn(value f)
	{
	return type_void(f);
	}

static void clear_bn(value f)
	{
	bn_free(f->v_ptr);
	}

static value Qbn(struct bn *x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_bn}};
	return V(type_bn,&clear,(value)x);
	}

static value op_pred(value f, int op(const struct bn *x))
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = boolean(op(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value type_bn_eq0(value f)
	{
	return op_pred(f,bn_eq0);
	}

static value type_bn_is_neg(value f)
	{
	return op_pred(f,bn_is_neg);
	}

static value type_bn_neg(value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qbn(bn_neg(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value type_bn_cmp(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qnum(bn_cmp(x->v_ptr,y->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value op_cmp(value f, int op(int))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = boolean(op(bn_cmp(x->v_ptr,y->v_ptr)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static int lt(int x) { return x < 0; }
static int le(int x) { return x <= 0; }
static int eq(int x) { return x == 0; }
static int ne(int x) { return x != 0; }
static int ge(int x) { return x >= 0; }
static int gt(int x) { return x > 0; }

static value type_bn_lt(value f) { return op_cmp(f,lt); }
static value type_bn_le(value f) { return op_cmp(f,le); }
static value type_bn_eq(value f) { return op_cmp(f,eq); }
static value type_bn_ne(value f) { return op_cmp(f,ne); }
static value type_bn_ge(value f) { return op_cmp(f,ge); }
static value type_bn_gt(value f) { return op_cmp(f,gt); }

static value type_bn_from_dec(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qbn(bn_from_dec(str_data(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value type_bn_to_dec(value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qstr(bn_to_dec(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value op_2(value f,
	struct bn *op(const struct bn *, const struct bn *))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qbn(op(x->v_ptr,y->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value type_bn_add(value f) { return op_2(f,bn_add); }
static value type_bn_sub(value f) { return op_2(f,bn_sub); }
static value type_bn_mul(value f) { return op_2(f,bn_mul); }
static value type_bn_mod(value f) { return op_2(f,bn_mod); }

static value type_bn_div(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		{
		struct bn *q;
		struct bn *r;
		bn_div(x->v_ptr,y->v_ptr,&q,&r);
		f = pair(Qbn(q),Qbn(r));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value type_is_bn(value f)
	{
	return op_is_type(f,type_bn);
	}

// big numbers
void define_bn(void)
	{
	define("bn_eq0",Q(type_bn_eq0));
	define("bn_is_neg",Q(type_bn_is_neg));
	define("bn_neg",Q(type_bn_neg));
	define("bn_cmp",Q(type_bn_cmp));
	define("bn_lt",Q(type_bn_lt));
	define("bn_le",Q(type_bn_le));
	define("bn_eq",Q(type_bn_eq));
	define("bn_ne",Q(type_bn_ne));
	define("bn_ge",Q(type_bn_ge));
	define("bn_gt",Q(type_bn_gt));
	define("bn_from_dec",Q(type_bn_from_dec));
	define("bn_to_dec",Q(type_bn_to_dec));
	define("bn_add",Q(type_bn_add));
	define("bn_sub",Q(type_bn_sub));
	define("bn_mul",Q(type_bn_mul));
	define("bn_mod",Q(type_bn_mod));
	define("bn_div",Q(type_bn_div));
	define("is_bn",Q(type_is_bn));
	}
