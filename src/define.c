#include <value.h>
#include <basic.h>
#include <define.h>
#include <die.h>
#include <num.h>
#include <source.h>
#include <str.h>
#include <string.h>
#include <type_cmp.h>
#include <type_convert.h>
#include <type_input.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_sys.h>

static const char *curr_name;

static int match(const char *other)
	{
	return strcmp(curr_name,other) == 0;
	}

static value define_name(const char *name)
	{
	curr_name = name;

	{
	number num = str_num(name);
	if (num) return Qnum(num);
	}

	if (match("say")) return Q(type_say);
	if (match("put")) return Q(type_put);
	if (match("get")) return Q(type_get);

	if (match("nl")) return Q(type_nl);
	if (match("@")) return Q(type_Y);
	if (match("T")) return hold(C);
	if (match("F")) return hold(F);

	if (match("+")) return Q(type_add);
	if (match("-")) return Q(type_sub);
	if (match("*")) return Q(type_mul);
	if (match("/")) return Q(type_div);
	if (match("^")) return Q(type_pow);
	if (match(".")) return Q(type_concat);

	if (match("lt")) return Q(type_lt);
	if (match("le")) return Q(type_le);
	if (match("eq")) return Q(type_eq);
	if (match("ne")) return Q(type_ne);
	if (match("ge")) return Q(type_ge);
	if (match("gt")) return Q(type_gt);

	if (match("length")) return Q(type_length);
	if (match("slice")) return Q(type_slice);

	if (match("num_str")) return Q(type_num_str);
	if (match("str_num")) return Q(type_str_num);

	if (match("round")) return Q(type_round);
	if (match("trunc")) return Q(type_trunc);
	if (match("abs")) return Q(type_abs);
	if (match("sqrt")) return Q(type_sqrt);

	if (match("void")) return Q(type_void);
	if (match("die")) return Q(type_die);

	if (match("eval")) return Q(type_eval);
	return 0;
	}

value define_sym(value x)
	{
	struct sym *sym = get_sym(x);
	const char *name;
	value def;

	if (sym->quoted)
		return hold(sym->name); /* literal string */

	name = get_str(sym->name)->data;

	def = define_name(name);
	if (def) return def;

	undefined_symbol(name, sym->line);
	return hold(x);
	}
