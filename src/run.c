#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <memory.h>
#include <parse.h>
#include <type_form.h>
#include <type_record.h>
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

static void beg_const(void)
	{
	beg_basic();
	beg_record();
	}

static void end_const(void)
	{
	end_basic();
	end_record();
	}

void run(const char *name)
	{
	FILE *fh = open_source(name);
	beg_const();
	run_fh(name,fh);
	end_const();
	clear_free_list();
	end_memory();
	}
