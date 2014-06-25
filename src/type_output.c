#include <str.h>
#include <value.h>

#include <basic.h>
#include <num.h>
#include <output.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static void putv(value f)
	{
	if (f->T == type_str)
		put_str(get_str(f));
	else if (f->T == type_num)
		put_num(get_num(f));
	else
		bad_type();

	drop(f);
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	putv(arg(f->R));
	return hold(I);
	}

value type_nl(value f)
	{
	(void)f;
	nl();
	return hold(I);
	}

value type_say(value f)
	{
	if (!f->L) return 0;
	putv(arg(f->R)); nl();
	return hold(I);
	}
