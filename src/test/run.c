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
#include <type_sym.h>

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

static value Mstr(const char *p)
	{
	return Qstr(str_new_data0(p));
	}

static value Mnum(double x)
	{
	return Qnum(num_new_double(x));
	}

static value Msym(const char *x)
	{
	return Qsym(0,Mstr(x),1);
	}

static value Mquo(const char *x)
	{
	return Qsym(1,Mstr(x),1);
	}

static value type_ping(value f)
	{
	if (!f->L) return 0;
	put("ping!");nl();
	return f->R;
	}

static void test_eval(value f)
	{
	hold(f);
	put("beg f = ");show(f);nl();
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
	test_eval(Mstr("abc"));
	test_eval(A(I,Mstr("abc")));
	test_eval(A(Mstr("abc"),Mstr("de")));
	test_eval(A(A(Q(type_concat),Mstr("abc")),Mstr("de")));
	test_eval(A(A(I,A(I,Mstr("abc"))),Mstr("de")));
	test_eval(A(A(Q(type_concat),A(I,Mstr("abc"))),Mstr("de")));
	test_eval(A(A(Q(type_concat),A(C,Mstr("abc"))),Mstr("de")));
	test_eval(A(
		A(Q(type_concat),
		A(A(I,I),Mstr("abc"))),
		A(A(Q(type_concat),Mstr("")),Mstr("de"))
		));
	}

	{
	test_eval(Mnum(3.14159265979265));
	test_eval(Qnum_ulong(42));
	test_eval(A(Q(type_length),Mstr("12345")));
	test_eval(A(Q(type_length),
		A(A(Q(type_concat),A(I,Mstr("abc"))),
			A(A(Q(type_concat),Mstr("")),Mstr("d")))));
	test_eval(A(A(A(Q(type_slice),
		A(A(C,
			A(A(Q(type_concat),Mstr("abcd")),Mstr("efghi"))
			),S)),
		Qnum_ulong(2)),Qnum_ulong(5)));
	}

	test_eval(A(A(Q(type_add),Mnum(-14.87)),Mnum(482.73)));
	test_eval(
		A(A(Q(type_div),
		A(A(Q(type_add),Mnum(1)),A(Q(type_sqrt),Mnum(5)))),
		Mnum(2)));

	{
	test_eval(A(A(C,Mstr("yes")),Mstr("no")));
	test_eval(A(A(F,Mstr("yes")),Mstr("no")));
	test_eval(A(A(Q(type_ge),A(I,Mnum(5.9))),Mnum(5.8)));
	test_eval(A(A(Q(type_lt),Mnum(5.9)),A(I,Mnum(5.8))));
	test_eval(A(A(Q(type_ge),A(I,Mstr("ab"))),Mstr("a")));
	test_eval(A(A(Q(type_lt),A(I,Mstr("ab"))),A(I,Mstr("a"))));
	test_eval(A(A(Q(type_eq),A(I,Mstr("x"))),Mstr("x")));
	test_eval(A(A(Q(type_eq),A(I,Mnum(-7.2))),Mnum(-7.2)));
	test_eval(A(A(Q(type_lt),
		A(I,Mnum(5.9))),A(I,Mstr("ab"))));
	test_eval(A(A(Q(type_lt),
		A(I,Mstr("ab"))),A(I,Mnum(5.9))));
	test_eval(A(A(Q(type_eq),A(I,S)),A(I,S)));
	}

	{
	test_eval(A(Q(type_num_str),
		A(A(Q(type_div),
		A(A(Q(type_add),Mnum(1)),A(Q(type_sqrt),Mnum(5)))),
		Mnum(2))));

	test_eval(A(Q(type_str_num),A(A(Q(type_concat),
		Mstr("+")),
		A(A(Q(type_concat),Mstr("1.")),Mstr("61803398874989"))
		)));
	test_eval(A(Q(type_str_num),A(I,Mnum(5.8))));
	test_eval(A(Q(type_str_num),A(I,S)));
	}

	/* Verify that J operators don't pile up. */
	test_eval(A(A(A(S,A(A(S,Mstr("x0")),Mstr("x1"))),Mstr("y")),Mstr("z")));

	test_eval(Msym("x"));
	test_eval(Mquo("x"));
	test_eval(A(Msym("x"),Mquo("x")));
	test_eval(A(Mquo("x"),Msym("x")));
	test_eval(A(I,app(app(app(I,I),Msym("x")),app(C,Msym("y")))));

	test_eval(lam(Msym("x"),Msym("x")));
	test_eval(lam(Msym("y"),Msym("x")));
	test_eval(lam(Msym("x"),app(Msym("x"),Msym("y"))));
	test_eval(lam(Msym("y"),app(Msym("x"),Msym("y"))));
	test_eval(lam(Msym("x"),app(Msym("y"),Msym("y"))));
	test_eval(lam(Msym("x"),app(Msym("y"),app(Msym("x"),Msym("x")))));
	test_eval(lam(Msym("x"),app(app(Msym("x"),Msym("x")),Msym("y"))));
	test_eval(lam(Msym("x"),
		app(
		app(Msym("x"),app(Msym("x"),Mquo("abc"))),
		app(app(Msym("y"),Msym("x")),Msym("x"))
		)));
	
	test_eval(lam(0,0));
	test_eval(lam(0,Msym("x")));
	test_eval(lam(Msym("x"),0));
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