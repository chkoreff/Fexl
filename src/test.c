#include <str.h>
#include <value.h>

#include <basic.h>
#include <output.h>
#include <type_str.h>
#include <type_sym.h>

static value test_argv(const char *const *argv)
	{
	unsigned long i = 0;
	put("= test_argv");nl();
	put("[");nl();
	while (argv[i])
		{
		put_ulong(i);put(" : ");put(argv[i]);nl();
		i++;
		}
	put("]");nl();
	return hold(QI);
	}

static value type_test_argv(value fun, value f)
	{
	return op_argv(fun,f,test_argv);
	}

static value define(void)
	{
	if (match("test_argv")) return Q(type_test_argv);
	return 0;
	}

value type_cx_test(value fun, value f)
	{
	return op_resolve(fun,f,define);
	}
