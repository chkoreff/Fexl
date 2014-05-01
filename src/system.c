#include <value.h>

#include <die.h>
#include <basic.h>
#include <output.h>
#include <parse.h>
#include <stdio.h> /* fgetc fopen */
#include <str.h>
#include <system.h>
#include <type_str.h>
#include <type_sym.h>

const char *source_label; /* current name of source */
unsigned long source_line; /* current line number */

static FILE *source_fh;

static int get_ch(void)
	{
	return fgetc(source_fh);
	}

value parse_file(const char *name)
	{
	source_label = name;
	source_fh = source_label[0] ? fopen(source_label,"r") : stdin;
	if (source_fh == 0)
		{
		beg_error();
		put("Could not open file ");put(source_label);nl();
		die("");
		}

	source_line = 1;
	read_ch = get_ch;
	return parse();
	}

value resolve_file(const char *name, value context(value))
	{
	value f = resolve(parse_file(name),context);
	if (f->T == type_sym)
		die(""); /* Die if we saw any undefined symbols. */
	return f;
	}

static void put_loc(unsigned long line)
	{
	put(" on line "); put_ulong(line);
	if (source_label[0])
		{
		put(" of ");put(source_label);
		}
	nl();
	}

void syntax_error(const char *msg, unsigned long line)
	{
	beg_error();
	put(msg); put_loc(line);
	die("");
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	target old = beg_error();
	put("Undefined symbol "); put(name); put_loc(line);
	end_error(old);
	}

value type_die(value f)
	{
	if (!f->L) return 0;
	value x = arg(f->R);
	die(get_str(x)->data);
	drop(x);
	return hold(I);
	}
