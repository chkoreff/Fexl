#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <input.h>
#include <output.h>
#include <parse.h>
#include <parse_file.h>
#include <string.h> /* strcmp */
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_sym.h>

int exit_code = 0;

static void put_loc(unsigned long line)
	{
	if (line) { put(" on line "); put_ulong(line); }
	nl();
	}

static const char *curr_name;

static int match(const char *other)
	{
	return strcmp(curr_name,other) == 0;
	}

static value context(const char *name, unsigned long line)
	{
	{
	int ok;
	number n = str0_num(name,&ok);
	if (ok) return Qnum(n);
	}

	curr_name = name;
	if (match("say")) return Q(type_say);
	if (match("put")) return Q(type_put);
	if (match("nl")) return Q(type_nl);
	if (match("@")) return Y;
	if (match("+")) return Q(type_add);

	put_to_error();
	exit_code = 1;
	put("Undefined symbol "); put(name); put_loc(line);
	return 0;
	}

static void report_error(void)
	{
	if (!error_code) return;
	put_to_error();
	put(error_code); put_loc(error_line);
	exit_code = 1;
	}

static value read_program(const char *name)
	{
	value exp = new_parse_file(name);
	if (exp)
		{
		value g = A(resolve(exp,context),I);
		drop(exp);
		return g;
		}
	else
		{
		report_error();
		return 0;
		}
	}

int main(int argc, char *argv[])
	{
	beg_basic();
	{
	const char *name = argc > 1 ? argv[1] : "";
	value f = eval(hold(read_program(name)));
	if (f != I && exit_code == 0)
		{
		if (f == bad)
			error_code = "The program used a data type incorrectly.";
		else
			error_code = "The program used too much space or time.";
		report_error();
		}
	drop(f);
	}
	end_basic();
	end_value();
	return exit_code;
	}
