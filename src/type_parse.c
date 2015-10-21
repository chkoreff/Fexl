#include <value.h>
#include <basic.h>
#include <parse.h>
#include <str.h>
#include <type_parse.h>
#include <type_resolve.h>
#include <type_str.h>

/* (parse label context) = f, where f is the function specified in the current
input in the given context.  The label is for the purpose of reporting any
error messages.
*/
value type_parse(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_str)
		{
		string name = data(x);
		value exp = parse(name->data);
		value label = hold(x);
		value context = hold(f->R);
		return later(op_resolve(label,exp,context));
		}
	reduce_void(f);
	return 0;
	}
	}
