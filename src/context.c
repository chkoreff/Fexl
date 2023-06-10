#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <parse.h>
// #include <string.h> // strcmp TODO
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <context.h>

value cx_std; // Current context TODO

void define(const char *key, value val)
	{
	// TODO use chain on the individual contexts
	cx_std = set(Qstr0(key),val,cx_std);
	}

void define_op(const char *name, value op(void))
	{
	define(name, Q(&type_Z,op));
	}

// Define initial context.

void beg_context(void)
	{
	beg_basic();

	cx_std = V(&type_record,0,0); // empty stack // TODO Qempty

	use_basic();
	use_num();
	use_str();
	use_math();
	use_output();
	use_meta();
	use_limit();
	use_var();
	use_parse();
	use_record();

	define("std",hold(cx_std));

	// LATER time
	// LATER rand
	// LATER crypto
	}

void end_context(void)
	{
	drop(cx_std);
	end_basic();
	clear_free_list();
	}
