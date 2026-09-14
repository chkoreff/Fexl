#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <define.h>
#include <file.h>
#include <show.h>
#include <test.h>
#include <type_str.h>

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

void define_test(void)
	{
	define("test_argv",Q(type_test_argv));
	define("show",Q(type_show));
	}

static value type_use_test(value f)
	{
	define_test();
	return hold(QI);
	(void)f;
	}

void define_use_test(void)
	{
	define("use_test",Q0(type_use_test)); // LATER 20260914 deprecated
	}
