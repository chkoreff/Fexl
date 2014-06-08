#include <str.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <die.h>
#include <output.h>
#include <string.h>
#include <test/show.h>
#include <type_file.h>
#include <type_math.h>
#include <type_str.h>
#include <type_sym.h>

void test_eval(value f, int full)
	{
	value g;
	put(": eval ");
	if (full)
		show(f);
	else
		put("...");
	nl();

	g = eval(f);
	put("= ");show(g);nl();
	nl();
	drop(g);
	}

void test_lam(value sym, value body)
	{
	value f;
	put(": lam ");show(sym);put(" ");show(body);nl();
	f = lam(sym,body);
	put("= ");show(f);nl();
	nl();
	drop(f);
	}

value x;
value y;
value z;
value ping;

value Qsym0(int quoted, const char *name)
	{
	return Qsym(quoted,str_new_data0(name),1);
	}

void beg_test(void)
	{
	x = Qsym0(0,"x");
	y = Qsym0(0,"y");
	z = Qsym0(0,"z");
	ping = A(Q(type_say),Qstr0("ping!"));
	}

void end_test(void)
	{
	drop(x);
	drop(y);
	drop(z);
	drop(ping);
	}

void test_all_eval(void)
	{
	test_eval(lam(hold(x),hold(x)),1);
	test_eval(lam(hold(x),hold(y)),1);
	test_eval(lam(hold(x),lam(hold(y),lam(hold(z),
		app(app(hold(x),hold(z)),hold(y))))),1);
	test_eval(lam(hold(x),lam(hold(y),lam(hold(z),
		app(hold(x),app(hold(y),hold(z)))))),1);
	test_eval(lam(hold(x),lam(hold(y),lam(hold(z),
		app(app(hold(x),hold(z)),app(hold(y),hold(z)))))),1);

	test_eval(A(A(hold(C),hold(x)),hold(y)),1);
	test_eval(A(hold(I),hold(x)),1);
	test_eval(A(A(A(hold(S),hold(x)),hold(y)),hold(z)),1);
	test_eval(A(A(A(hold(L),hold(x)),hold(y)),hold(z)),1);
	test_eval(A(A(A(hold(R),hold(x)),hold(y)),hold(z)),1);
	test_eval(app(hold(x),hold(y)),1);
	test_eval(A(A(A(hold(S),hold(C)),hold(C)),hold(C)),1);
	test_eval(A(A(A(hold(S),hold(C)),hold(S)),hold(Y)),1);

	/*
	Verify that type_S builds the right side lazily so it's not evaluated
	prematurely.  Otherwise the ("hi" x) on the right side yields a type error.
	: S C "hi" x
	= C x ("hi" x)
	= x
	*/
	test_eval(A(A(A(hold(S),hold(C)),Qstr0("hi")),hold(x)),1);
	test_eval(A(A(A(A(hold(S),hold(S)),hold(C)),hold(x)),hold(y)),1);

	{
	value f = A(A(Q(type_concat),Qstr0("x")),Qstr0("y"));

	int i;
	for (i = 0; i < 5; i++)
		f = A(A(Q(type_concat),f),hold(f));

	test_eval(f,0);
	}

	test_eval(Qstr0("x"),1);
	test_eval(A(A(hold(C),Qstr0("x")),hold(I)),1);
	test_eval(A(hold(y),Qstr0("x")),1);
	test_eval(A(A(hold(x),hold(I)),hold(C)),1);
	test_eval(A(Qstr0("x"),hold(C)),1);
	test_eval(A(A(Qstr0("x"),hold(C)),hold(I)),1);
	test_eval(A(A(hold(I),Qstr0("x")),hold(C)),1);

	{
	test_eval(hold(ping),1);
	test_eval(A(hold(ping),hold(I)),1);
	test_eval(A(hold(ping),hold(ping)),1);
	test_eval(A(A(hold(ping),hold(ping)),hold(I)),1);
	test_eval(A(A(hold(ping),hold(ping)),hold(x)),1);
	}

	test_eval(A(hold(C),A(hold(I),hold(S))),1);
	test_eval(A(A(hold(query),A(A(hold(I),hold(I)),
		A(hold(I),hold(S)))),hold(C)),1);

	test_eval(hold(Qcons),1);
	test_eval(A(hold(Qcons),hold(x)),1);
	test_eval(A(A(hold(Qcons),hold(x)),hold(y)),1);
	test_eval(A(A(A(hold(Qcons),hold(x)),hold(y)),hold(z)),1);
	test_eval(A(A(A(A(hold(Qcons),hold(x)),hold(y)),hold(z)),Qsym0(0,"G")),1);
	test_eval(A(A(A(hold(S),hold(I)),hold(I)),hold(I)),1);

	{
	value f = hold(C);
	int i;
	for (i = 0; i < 24; i++)
		f = A(f,hold(f));
	test_eval(f,0);
	}

	{
	value f = hold(C);
	int i;
	for (i = 0; i < 33; i++)
		f = A(f,hold(C));
	test_eval(f,0);
	}

	{
	/* This combinatorial explosion is quadratic but runs fast. */
	value f = hold(I);
	int i;
	for (i = 0; i < 22; i++)
		f = A(f,hold(f));
	test_eval(f,0);
	}
	}

void test_str_concat(const char *xp, const char *yp)
	{
	string x = str_new_data0(xp);
	string y = str_new_data0(yp);
	string z = str_concat(x,y);
	put(": concat ");
	put_quoted(x->data);
	put(" ");
	put_quoted(y->data);
	put(" = ");
	put_quoted(z->data);
	nl();
	str_free(x);
	str_free(y);
	str_free(z);
	}

void test_str_eq(string x, string y)
	{
	put(": str_eq ");
	put_quoted(x->data);
	put(" ");
	put_quoted(y->data);
	put(" = ");
	put_long(str_eq(x,y));
	nl();
	str_free(x);
	if (y != x) str_free(y);
	}

void test_binary_str_eq(string x, string y)
	{
	put(": binary str_eq = ");
	put_long(str_eq(x,y));
	nl();
	str_free(x);
	if (y != x) str_free(y);
	}

void test_all_str(void)
	{
	{
	string hi = str_new_data0("hi");
	test_str_eq(hi,hi);
	}

	test_str_eq(str_new_data0("hi"),str_new_data0("bye"));
	test_str_eq(str_new_data0("hi"),str_new_data0("hi"));
	test_str_eq(str_new_data0("hi"),str_new_data0("hj"));
	test_str_eq(str_new_data0(""),str_new_data0(""));
	test_str_eq(str_new_data0("abcd"),str_new_data0("abcd"));
	test_str_eq(str_new_data0("abcd"),str_new_data0("abcde"));
	test_str_eq(str_new_data("hi",2),str_new_data("hj",1));
	test_binary_str_eq(str_new_data("h\000i",3),str_new_data("h\000i",3));
	test_binary_str_eq(str_new_data("h\000i",3),str_new_data("h\000j",3));

	{
	test_str_concat("","");
	test_str_concat("a","b");
	test_str_concat("abc","d");
	test_str_concat("abc","");
	test_str_concat("","def");
	}

	return;
	nl();
	}

void test_all_lam(void)
	{
	test_lam(hold(x),hold(I));
	test_lam(hold(x),hold(x));
	test_lam(hold(x),hold(y));
	test_lam(Qsym0(0,"abc"),Qsym0(0,"abc"));
	test_lam(Qsym0(0,"abc"),Qsym0(0,"ab"));
	test_lam(Qsym0(0,"abc"),Qsym0(1,"abc"));
	test_lam(Qsym0(1,"abc"),Qsym0(1,"abc"));
	test_lam(Qsym0(1,"ab"),Qsym0(1,"abc"));
	test_lam(Qsym0(1,"abc"),Qsym0(0,"abc"));
	test_lam(hold(x),app(hold(x),hold(x)));
	test_lam(hold(x),app(hold(x),hold(y)));
	test_lam(hold(x),app(hold(y),hold(x)));
	test_lam(hold(x),app(hold(y),hold(z)));
	test_lam(hold(y),app(hold(x),app(hold(y),hold(y))));
	test_lam(hold(x),lam(hold(y),app(hold(x),app(hold(y),hold(y)))));
	test_lam(hold(x),lam(hold(y),app(app(hold(x),hold(y)),
		app(hold(y),hold(x)))));
	}

void test_buf(unsigned long max)
	{
	buffer buf;
	unsigned long len;
	string x;

	put(": Buffering ");put_ulong(max);put(" bytes");nl();
	buf_start(&buf);

	for (len = 0; len < max; len++)
		{
		char ch = (char)('a' + (len % 26));
		buf_add(&buf,ch);
		}
	x = buf_finish(&buf);
	if (x == 0)
		put("  null\n");
	else
		{
		put("  length = ");
		put_ulong(x->len);
		nl();
		if (x->len <= 74)
			{
			put("  ");
			put_quoted(x->data);
			nl();
			}
		str_free(x);
		}
	nl();
	}

void test_all_buf(void)
	{
	test_buf(0);
	test_buf(26);
	test_buf(74);
	test_buf(2600000);
	}

void run_tests(void)
	{
	test_all_str();
	test_all_eval();
	test_all_lam();
	test_all_buf();
	}

int main(void)
	{
	beg_basic();
	beg_test();
	run_tests();
	end_test();
	end_basic();
	end_value();
	return 0;
	}
