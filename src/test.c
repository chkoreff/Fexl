#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <file.h>
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

value type_test_argv(value f)
	{
	return op_argv(f,test_argv);
	}
