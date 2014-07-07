#include <value.h>
#include <die.h>
#include <output.h>
#include <type_output.h>
#include <type_sys.h>

void type_die(value f)
	{
	putd = putd_err;
	putv(eval(f->R)); nl();
	die("");
	}
