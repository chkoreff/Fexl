#include <value.h>
#include <basic.h>
#include <memory.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
#include <type_cmp.h>
#include <type_convert.h>
#include <type_math.h>
#include <type_num.h>
#include <type_str.h>

/*
TODO is_num is_str yes

\yes=(\x \yes\no yes x)
\yes=(R (R C) (L I))

\no=(\yes\no no)
\no=(C I)
\no=F  # Don't need a separate operator for this.

is_num x (\x yes x) no  # is_num is built-in
is_str x (\x yes x) no  # is_str is built-in
is_def x (\x yes x) no  # Here is_def is an arbitrary context.
*/

static value Qstr0(const char *p)
	{
	return Qstr(str_new_data0(p));
	}

static value Qnum_double(double x)
	{
	return Qnum(num_new_double(x));
	}

static value type_ping(value f)
	{
	if (!f->L) return 0;
	put("ping!");nl();
	return f->R;
	}

static void test_eval(value f)
	{
	put("beg f = ");show(f);nl();
	hold(f);
	{
	value g = eval(hold(f));
	put("end f = ");show(f);nl();
	put("end g = ");show(g);nl();
	drop(g);
	}
	drop(f);
	nl();
	}

static void show_test_atom(value f)
	{
	if (f->T == type_ping) put("ping");
	else put_ch('_');
	}

static void limit_test_eval(value f,
	unsigned long new_remain_steps,
	unsigned long new_max_words)
	{
	unsigned long save_remain_steps = remain_steps;
	unsigned long save_max_words = max_words;

	clear_free_list();
	remain_steps = new_remain_steps;
	max_words = new_max_words;

	test_eval(f);

	remain_steps = save_remain_steps;
	max_words = save_max_words;
	}

static void run_test_suite(void)
	{
	show_other = show_test_atom;

	test_eval(A(A(C,S),C));
	test_eval(C);
	test_eval(A(C,C));
	test_eval(A(A(C,C),C));

	{
	value f = A(A(C,C),C);
	f = A(f,f);
	test_eval(f);
	}

	test_eval(A(A(A(S,C),S),A(C,C)));
	test_eval(A(A(C,A(A(C,C),C)),C));

	{
	value f;
	f = A(A(C,A(S,C)),C);
	f = A(A(A(S,C),S),f);
	f = A(A(A(S,C),S),f);
	f = A(A(A(S,C),S),f);
	f = A(A(A(S,C),S),f);
	test_eval(f);
	}

	{
	value f = C;
	f = A(f,f);
	f = A(f,f);
	f = A(f,f);
	f = A(f,f);
	test_eval(f);
	}

	test_eval(A(A(A(A(A(S,S),S),S),C),C));

	test_eval(A(A(A(S,I),I),C));

	test_eval(A(A(A(S,A(A(S,C),C)),
		A(A(C,C),C)),A(A(C,S),C)));

	test_eval(Q(type_ping));
	test_eval(A(I,Q(type_ping)));
	test_eval(C);
	test_eval(A(C,C));
	test_eval(A(A(C,C),C));
	test_eval(A(A(C,S),C));

	{
	/*
	This function pings 12 times:
		\f=ping
		\f=(\next f; f; f; next)
		\f=(\next f; f; next)
		\f=(\next f; f; next)
		f;

	The equivalent combinator form is:
		R (R (R (L I I) (S R I)) (S R I)) (S R (S R I)) ping
	*/
	test_eval(A(A(A(R,A(A(R,A(A(R,A(A(L,I),
		I)),A(A(S,R),I))),A(A(S,R),
		I))),A(A(S,R),A(A(S,R),I))),
		Q(type_ping)));
	}

	test_eval(Y);
	test_eval(A(Y,C));

	limit_test_eval(A(Y,I), 10000, 1000);
	limit_test_eval(A(Y,Y), 1000000, 50);
	limit_test_eval(A(A(A(Y,S),S),S), 1000000, 60);

	{
	test_eval(Qstr0("abc"));
	test_eval(A(I,Qstr0("abc")));
	test_eval(A(Qstr0("abc"),Qstr0("de")));
	test_eval(A(A(Q(type_concat),Qstr0("abc")),Qstr0("de")));
	test_eval(A(A(I,A(I,Qstr0("abc"))),Qstr0("de")));
	test_eval(A(A(Q(type_concat),A(I,Qstr0("abc"))),Qstr0("de")));
	test_eval(A(A(Q(type_concat),A(C,Qstr0("abc"))),Qstr0("de")));
	test_eval(A(
		A(Q(type_concat),
		A(A(I,I),Qstr0("abc"))),
		A(A(Q(type_concat),Qstr0("")),Qstr0("de"))
		));
	}

	{
	test_eval(Qnum_double(3.14159265979265));
	test_eval(Qnum_ulong(42));
	test_eval(A(Q(type_length),Qstr0("12345")));
	test_eval(A(Q(type_length),
		A(A(Q(type_concat),A(I,Qstr0("abc"))),
			A(A(Q(type_concat),Qstr0("")),Qstr0("d")))));
	test_eval(A(A(A(Q(type_slice),
		A(A(C,
			A(A(Q(type_concat),Qstr0("abcd")),Qstr0("efghi"))
			),S)),
		Qnum_ulong(2)),Qnum_ulong(5)));
	}

	test_eval(A(A(Q(type_add),Qnum_double(-14.87)),Qnum_double(482.73)));
	test_eval(
		A(A(Q(type_div),
		A(A(Q(type_add),Qnum_double(1)),A(Q(type_sqrt),Qnum_double(5)))),
		Qnum_double(2)));

	{
	test_eval(A(A(C,Qstr0("yes")),Qstr0("no")));
	test_eval(A(A(F,Qstr0("yes")),Qstr0("no")));
	test_eval(A(A(Q(type_ge),A(I,Qnum_double(5.9))),Qnum_double(5.8)));
	test_eval(A(A(Q(type_lt),Qnum_double(5.9)),A(I,Qnum_double(5.8))));
	test_eval(A(A(Q(type_ge),A(I,Qstr0("ab"))),Qstr0("a")));
	test_eval(A(A(Q(type_lt),A(I,Qstr0("ab"))),A(I,Qstr0("a"))));
	test_eval(A(A(Q(type_eq),A(I,Qstr0("x"))),Qstr0("x")));
	test_eval(A(A(Q(type_eq),A(I,Qnum_double(-7.2))),Qnum_double(-7.2)));
	test_eval(A(A(Q(type_lt),
		A(I,Qnum_double(5.9))),A(I,Qstr0("ab"))));
	test_eval(A(A(Q(type_lt),
		A(I,Qstr0("ab"))),A(I,Qnum_double(5.9))));
	test_eval(A(A(Q(type_eq),A(I,S)),A(I,S)));
	}

	{
	test_eval(A(Q(type_num_str),
		A(A(Q(type_div),
		A(A(Q(type_add),Qnum_double(1)),A(Q(type_sqrt),Qnum_double(5)))),
		Qnum_double(2))));

	test_eval(A(Q(type_str_num),A(A(Q(type_concat),
		Qstr0("+")),
		A(A(Q(type_concat),Qstr0("1.")),Qstr0("61803398874989"))
		)));
	test_eval(A(Q(type_str_num),A(I,Qnum_double(5.8))));
	test_eval(A(Q(type_str_num),A(I,S)));
	}
	}

int main(int argc, char *argv[])
	{
	(void)argc;
	(void)argv;
	beg_basic();
	run_test_suite();
	end_basic();
	end_value();
	return 0;
	}
