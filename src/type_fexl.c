#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_fexl.h>
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>

static value Qtype(type T)
	{
	unsigned long n = (unsigned long)T;
	return Qnum((double)n);
	}

// (fexl_type x) Return the type of x as a number.  The number is the address
// of the C function pointer, so it may change between invocations.
value type_fexl_type(value f)
	{
	return Qtype(f->R->T);
	}

// (fexl_look x case_atom case_data case_tree)
// Look at the value x and return one of three cases:
//    (case_atom T)     # if x is an atom of type T
//    (case_data T)     # if x is data of type T
//    (case_tree T L R) # if x is tree of type T with left L and right R
value type_fexl_look(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	if (f->L->L->L->L == 0) return keep(f);
	{
	value x = f->L->L->L->R;
	value type = Qtype(x->T);

	if (x->L == 0)
		f = A(hold(f->L->L->R),type); // atom
	else if (x->L->N == 0)
		f = A(hold(f->L->R),type); // data
	else
		f = A(A(A(hold(f->R),type),hold(x->L)),hold(x->R)); // tree
	return f;
	}
	}

// (fexl_look_quo x) If x is type_quo, return {y} where y is the right side.
value type_fexl_look_quo(value f)
	{
	if (f->R->T == type_quo)
		f = yield(hold(f->R->R));
	else
		f = hold(Qvoid);
	return f;
	}

// (fexl_look_ref x) If x is type_ref, return {sym line}.
value type_fexl_look_ref(value f)
	{
	if (f->R->T == type_ref)
		{
		value x = f->R;
		value sym = Qstr(str_copy(x->R->v_ptr));
		value line = Qnum(x->R->N);
		f = pair(sym,line);
		}
	else
		f = hold(Qvoid);
	return f;
	}
