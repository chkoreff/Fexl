#include <value.h>
#include <basic.h>
#include <die.h>
#include <input.h>
#include <output.h>
#include <parse.h>
#include <source.h>
#include <stdio.h> /* fgetc fopen */
#include <str.h>
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
	input save_getd = getd;
	value exp;

	source_label = name;
	source_fh = source_label[0] ? fopen(source_label,"r") : stdin;
	if (source_fh == 0)
		{
		putd = putd_err;
		put("Could not open file ");put(source_label);nl();
		die("");
		}

	source_line = 1;
	getd = get_ch;
	exp = parse();
	getd = save_getd;
	return exp;
	}

value resolve_file(const char *name, value context(value))
	{
	value exp = resolve(parse_file(name),context);
	if (exp->T == type_sym)
		die(""); /* Die if we saw any undefined symbols. */
	return exp;
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
	putd = putd_err;
	put(msg); put_loc(line);
	die("");
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	output save_putd = putd;
	put("Undefined symbol "); put(name); put_loc(line);
	putd = save_putd;
	}
