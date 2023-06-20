#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h> // TODO
#include <memory.h>
#include <parse.h>
#include <type_form.h>
#include <type_str.h>

#include <run.h>

static void run_fh(const char *name, FILE *fh)
	{
	value label = Qstr0(name);
	value exp = parse_fh(label,fh);
	value cx = std();
	exp = A(A(Q(type_restrict),cx),exp);
	exp = eval(exp);
	drop(exp);
	}

void run(const char *name)
	{
	FILE *fh = open_source(name);
	beg_basic();
	run_fh(name,fh);
	end_basic();
	clear_free_list();
	end_memory();
	}
