#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <output.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sys.h>

value type_die(value f)
	{
	if (!f->L) return 0;
	putd = putd_err;
	type_say(f);
	die("");
	return 0;
	}
