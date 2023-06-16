#include <stdio.h>
#include <str.h> // TODO
#include <value.h>

#include <context.h>
#include <die.h>
#include <memory.h>
#include <parse.h>

int main(int argc, const char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	FILE *fh = open_source(name);

	beg_context();
	drop(eval(load(cx_std,name,fh)));
	end_context();

	end_memory();
	return 0;
	}
