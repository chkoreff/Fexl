#include <value.h>
#include <basic.h>
#include <define.h>
#include <source.h>

int main(int argc, char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	beg_basic();
	drop(eval(A(resolve_file(name,define_sym),hold(I))));
	end_basic();
	end_value();
	return 0;
	}
