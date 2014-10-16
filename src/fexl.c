#include <value.h>
#include <basic.h>
#include <source.h>

int main(int argc, char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : 0;
	beg_basic();
	drop(eval_file(name));
	end_basic();
	end_value();
	return 0;
	}
