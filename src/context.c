#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_form.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <context.h>

static value def0(const char *key, value val, value cx)
	{
	return def(Qstr0(key),val,cx);
	}

// Define initial context.

static value cx_basic(void)
	{
	value cx = hold(Qempty);
	cx = def0("I", hold(QI),cx);
	cx = def0("T", hold(QT),cx);
	cx = def0("F", hold(QF),cx);
	cx = def0("@", Q(type_Y),cx);
	cx = def0("void", hold(Qvoid),cx);
	cx = def0("null", hold(Qnull),cx);
	cx = def0("yield", Q(type_yield),cx);
	cx = def0("cons", Q(type_cons),cx);
	cx = def0("list_to_tuple", Q(type_list_to_tuple),cx);
	cx = def0("tuple_to_list", Q(type_tuple_to_list),cx);
	return cx;
	}

static value cx_num(void)
	{
	value cx = hold(Qempty);
	cx = def0("num_str", Q(type_num_str),cx);
	return cx;
	}

static value cx_str(void)
	{
	value cx = hold(Qempty);
	cx = def0(".", Q(type_concat),cx);
	// LATER length
	// LATER slice
	// LATER search
	// LATER str_num
	// LATER ord
	// LATER chr
	// LATER char_width
	// LATER dirname
	// LATER basename
	// LATER length_common
	// LATER compare_at
	return cx;
	}

static value cx_math(void)
	{
	value cx = hold(Qempty);
	cx = def0("pi", Qnum(num_pi),cx);

	cx = def0("round", Q(type_round),cx);
	cx = def0("ceil", Q(type_ceil),cx);
	cx = def0("trunc", Q(type_trunc),cx);
	cx = def0("abs", Q(type_abs),cx);
	cx = def0("sqrt", Q(type_sqrt),cx);
	cx = def0("exp", Q(type_exp),cx);
	cx = def0("log", Q(type_log),cx);
	cx = def0("sin", Q(type_sin),cx);
	cx = def0("cos", Q(type_cos),cx);

	cx = def0("+", Q(type_add),cx);
	cx = def0("-", Q(type_sub),cx);
	cx = def0("*", Q(type_mul),cx);
	cx = def0("/", Q(type_div),cx);
	cx = def0("^", Q(type_pow),cx);
	cx = def0("xor", Q(type_xor),cx);
	return cx;
	}

static value cx_output(void)
	{
	value cx = hold(Qempty);
	cx = def0("nl", A(Q(type_nl), hold(QI)),cx);
	cx = def0("say", Q(type_say),cx);
	cx = def0("put", Q(type_put),cx);
	// LATER fput fsay
	return cx;
	}

static value cx_meta(void)
	{
	value cx = hold(Qempty);
	// LATER introspection functions
	// LATER die
	cx = def0("show_benchmark", Q(type_show_benchmark),cx);
	cx = def0("trace_benchmark", Q(type_trace_benchmark),cx);
	cx = def0("show", Q(type_show),cx);
	cx = def0("read", Q(type_read),cx);
	return cx;
	}

static value cx_limit(void)
	{
	value cx = hold(Qempty);
	cx = def0("limit_time", Q(type_limit_time),cx);
	cx = def0("limit_stack", Q(type_limit_stack),cx);
	cx = def0("limit_memory", Q(type_limit_memory),cx);
	return cx;
	}

static value cx_var(void)
	{
	value cx = hold(Qempty);
	cx = def0("var_new", A(Q(type_var_new),hold(QI)),cx);
	cx = def0("var_put", Q(type_var_put),cx);
	cx = def0("var_get", Q(type_var_get),cx);
	return cx;
	}

static value cx_record(void)
	{
	value cx = hold(Qempty);
	cx = def0("empty", hold(Qempty),cx);
	cx = def0("def", Q(type_def),cx);
	cx = def0("set", Q(type_set),cx);
	cx = def0("get", Q(type_get),cx);
	cx = def0("::", Q(type_chain),cx);
	cx = def0("record_pairs", Q(type_record_pairs),cx);
	return cx;
	}

static value cx_form(void)
	{
	value cx = hold(Qempty);
	cx = def0("restrict",Q(type_restrict),cx);
	return cx;
	}

// LATER time
// LATER rand
// LATER crypto

value std(void)
	{
	value cx = hold(Qempty);
	cx = chain(cx_basic(), cx);
	cx = chain(cx_num(), cx);
	cx = chain(cx_str(), cx);
	cx = chain(cx_math(), cx);
	cx = chain(cx_output(), cx);
	cx = chain(cx_meta(), cx);
	cx = chain(cx_limit(), cx);
	cx = chain(cx_var(), cx);
	cx = chain(cx_record(), cx);
	cx = chain(cx_form(), cx);
	cx = def0("std",hold(cx),cx);
	return cx;
	}
