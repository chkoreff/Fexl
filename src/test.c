#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <file.h>
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

static value type_test_argv(value f)
	{
	return op_argv(f,test_argv);
	}

static value define(void)
	{
	if (match("test_argv")) return Q(type_test_argv);
	return 0;
	}

value type_cx_test(value f)
	{
	return op_context(f,define);
	}
