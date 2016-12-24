#include <value.h>

#include <output.h>
#include <show.h>
#include <trace.h>

static unsigned long depth;

static void indent(void)
	{
	if (0)
	{
	unsigned long i;
	for (i = 1; i < depth; i++)
		put("  ");
	}
	else
	{
	put_ulong(depth);put(" ");
	}
	}

static void trace_line(const char *name, value f)
	{
	if (0) return; /* Disable here. */
	if (0 && depth > 0) return; /* Configure max depth here. */
	indent();show_line(name,f);
	}

static void begin_eval(value f)
	{
	trace_line("[ ",f);
	depth++;
	}

static value step(value f)
	{
	value g = f->T(f);
	if (g) trace_line(": ",g);
	return g;
	}

static void end_eval(value f)
	{
	depth--;
	trace_line("] ",f);
	}

/* Reduce the value until done, printing trace lines along the way. */
value trace_eval(value f)
	{
	begin_eval(f);
	while (1)
		{
		value g = step(f);
		if (g == 0) break;
		if (g != f)
			{
			drop(f);
			f = g;
			}
		}
	end_eval(f);
	return f;
	}
