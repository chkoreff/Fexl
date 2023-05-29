#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

// Define standard context.

value cx_std;

void define(const char *key, value val)
	{
	cx_std = A(A(Qstr0(key),val),cx_std);
	}

void define_op(const char *name, value op(void))
	{
	define(name, Q(&type_Z,op));
	}

void beg_std(void)
	{
	// LATER Define fexl functions for these and don't call them here.
	limit_time(1); // LATER Perhaps use alarm for sub-second limits.
	limit_stack(20000);
	limit_memory(10000000);

	beg_basic();

	cx_std = hold(QI); // empty stack

	use_basic();
	use_num();
	use_str();
	use_math();
	use_output();
	use_meta();

	// LATER time
	// LATER rand
	// LATER crypto
	}

void end_std(void)
	{
	drop(cx_std);
	end_basic();
	}
