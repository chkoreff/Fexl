#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <type_str.h>

#include <type_output.h>

static value op_print(value fun, value arg, void op(string))
	{
	arg = eval(arg);
	if (arg->T == type_str)
		op(arg->v_ptr);
	return type_F(fun,arg);
	}

static void say_str(string x)
	{
	put_str(x);
	nl();
	}

value type_say(value fun, value arg) { return op_print(fun,arg,say_str); }
value type_put(value fun, value arg) { return op_print(fun,arg,put_str); }

value type_nl(value fun, value arg)
	{
	nl();
	return type_F(fun,arg);
	}

void use_output(void)
	{
	define("nl", A(Q(type_nl), hold(QI)));
	define("say", Q(type_say));
	define("put", Q(type_put));
	// LATER fput fsay
	}
