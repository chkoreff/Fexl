#include <value.h>

#include <basic.h>
#include <define.h>
#include <memory.h>
#include <system.h>

int main(int argc, char *argv[])
	{
	beg_basic();
	{
	const char *name = argc > 1 ? argv[1] : "";
	drop(eval(resolve_file(name,define_sym)));
	}
	end_basic();
	end_memory();
	return 0;
	}
