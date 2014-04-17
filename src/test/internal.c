#include <value.h>
#include <basic.h>
#include <buf.h>
#include <die.h>
#include <show.h>
#include <stdio.h>
#include <str.h>
#include <string.h>
#include <test/math.h>
#include <type_file.h>
#include <type_str.h>
#include <type_sym.h>

void test_eval(value f, int full)
	{
	printf(": eval ");
	if (full)
		show(f);
	else
		printf("...");
	nl();

	f = eval(f);
	printf("= ");show(f);nl();
	nl();
	drop(f);
	}

void test_lam(value sym, value body)
	{
	printf(": lam ");show(sym);printf(" ");show(body);nl();
	value f = lam(sym,body);
	hold(f);
	printf("= ");show(f);nl();
	nl();
	drop(f);
	}

value type_ping(value f)
	{
	(void)f;
	printf("ping!\n");
	return I;
	}

int show_atom_test(value f)
	{
	if (show_atom_default(f))
		;
	else if (f->T == type_ping)
		printf("ping");
	else
		return 0;

	return 1;
	}

value x;
value y;
value z;
value ping;

void beg_test(void)
	{
	x = Qsym0("x",1);
	y = Qsym0("y",2);
	z = Qsym0("z",3);
	ping = Q(type_ping);
	hold(x);
	hold(y);
	hold(z);
	hold(ping);
	show_atom = show_atom_test;
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
	test_eval(A(A(C,x),y),1);
	test_eval(A(I,x),1);
	test_eval(A(A(A(S,x),y),z),1);
	test_eval(A(A(A(L,x),y),z),1);
	test_eval(A(A(A(R,x),y),z),1);
	test_eval(app(x,y),1);

	test_eval(A(A(A(S,C),C),C),1);
	test_eval(A(A(A(S,C),S),Y),1);

	/*
	Verify that type_S builds the right side lazily so it's not evaluated
	prematurely.  Otherwise the ("hi" x) on the right side yields a type error.
	: S C "hi" x
	= C x ("hi" x)
	= x
	*/
	test_eval(A(A(A(S,C),Qstr0("hi")),x),1);
	test_eval(A(A(A(A(S,S),C),x),y),1);

	{
	value f = A(A(Q(type_concat),Qstr0("x")),Qstr0("y"));
	int i;
	for (i = 0; i < 5; i++)
		f = A(A(Q(type_concat),f),f);
	test_eval(f,0);
	}

	test_eval(Qstr0("x"),1);
	test_eval(A(A(C,Qstr0("x")),I),1);
	test_eval(A(y,Qstr0("x")),1);
	test_eval(A(A(x,I),C),1);
	test_eval(Q(type_halt),1);
	test_eval(A(Q(type_halt),C),1);
	test_eval(A(A(Q(type_halt),C),I),1);
	test_eval(A(A(Q(type_halt),A(C,C)),S),1);
	test_eval(A(A(I,Q(type_halt)),C),1);

	{
	test_eval(ping,1);
	test_eval(A(ping,I),1);
	test_eval(A(ping,ping),1);
	test_eval(A(A(ping,ping),I),1);
	test_eval(A(A(ping,ping),x),1);
	}

	test_eval(A(C,A(I,S)),1);
	test_eval(A(A(query,A(A(I,I),A(I,S))),C),1);

	test_eval(Qpair,1);
	test_eval(A(Qpair,x),1);
	test_eval(A(A(Qpair,x),y),1);
	test_eval(A(A(A(Qpair,x),y),Qsym0("G",0)),1);
	test_eval(Qcons,1);
	test_eval(A(Qcons,x),1);
	test_eval(A(A(Qcons,x),y),1);
	test_eval(A(A(A(Qcons,x),y),z),1);
	test_eval(A(A(A(A(Qcons,x),y),z),Qsym0("G",0)),1);
	test_eval(A(A(A(S,I),I),I),1);

	{
	value f = C;
	int i;
	for (i = 0; i < 24; i++)
		f = A(f,f);
	test_eval(f,0);
	}

	{
	value f = C;
	int i;
	for (i = 0; i < 33; i++)
		f = A(f,C);
	test_eval(f,0);
	}

	{
	/* This combinatorial explosion is quadratic but runs fast. */
	value f = I;
	int i;
	for (i = 0; i < 22; i++)
		f = A(f,f);
	test_eval(f,0);
	}
	}

void test_str_concat(const char *xp, const char *yp)
	{
	struct str *x = str_new_data0(xp);
	struct str *y = str_new_data0(yp);
	struct str *z = str_concat(x,y);
	printf(": concat \"%s\" \"%s\" = \"%s\"\n", x->data, y->data, z->data);
	str_free(x);
	str_free(y);
	str_free(z);
	}

void test_str_eq(struct str *x, struct str *y)
	{
	printf(": str_eq \"%s\" \"%s\" = %d\n", x->data, y->data, str_eq(x,y));
	str_free(x);
	if (y != x) str_free(y);
	}

void test_binary_str_eq(struct str *x, struct str *y)
	{
	printf(": binary str_eq = %d\n", str_eq(x,y));
	str_free(x);
	if (y != x) str_free(y);
	}

void test_all_str(void)
	{
	{
	struct str *hi = str_new_data0("hi");
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

	/*TODO test str_common, str_index, etc.*/
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
	test_lam(x,I);
	test_lam(x,x);
	test_lam(x,y);
	test_lam(Qsym0("abc",1),Qsym0("abc",1));
	test_lam(Qsym0("abc",1),Qsym0("ab",1));
	test_lam(Qsym0("abc",0),Qsym0("abc",-1));
	test_lam(Qsym0("abc",-2),Qsym0("abc",-5));
	test_lam(Qsym0("ab",-2),Qsym0("abc",-5));
	test_lam(Qsym0("abc",-2),Qsym0("abc",5));
	test_lam(x,app(x,x));
	test_lam(x,app(x,y));
	test_lam(x,app(y,x));
	test_lam(x,app(y,z));
	test_lam(y,app(x,app(y,y)));
	test_lam(x,lam(y,app(x,app(y,y))));
	test_lam(x,lam(y,app(app(x,y),app(y,x))));
	}

void test_buf(int max)
	{
	printf(": Buffering %d bytes\n", max);
	struct buf buf;
	buf_start(&buf);

	int len;
	for (len = 0; len < max; len++)
		{
		char ch = 'a' + (len % 26);
		buf_add(&buf,ch);
		}
	struct str *x = buf_finish(&buf);
	if (x == 0)
		printf("  null\n");
	else
		{
		printf("  length = %d\n", x->len);
		if (x->len <= 74)
			printf("  \"%s\"\n", x->data);
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

value context(value x)
	{
	struct sym *sym = get_sym(x);
	if (sym->line < 0)
		return sym->name; /* literal string */

	const char *name = get_str(sym->name)->data;
	if (strcmp(name,"ping") == 0)
		return ping;

	printf("  Undefined symbol %s on line %d\n", name, sym->line);
	return x;
	}

void test_resolve(value f)
	{
	printf(": resolve ");show(f);nl();
	value g = resolve(f,context);
	hold(g);
	if (g->T == type_sym)
		printf("  The form has undefined symbols.\n");
	printf("= ");show(g);nl();
	nl();
	drop(g);
	}

void test_all_resolve(void)
	{
	test_resolve(app(x,y));
	test_resolve(app(y,x));
	test_resolve(app(Qsym0("x",-1),Qsym0("y",-2)));
	test_resolve(Qsym0("ping",2));
	test_resolve(app(Qsym0("ping",2),Qsym0("ping",4)));
	test_resolve(app(Qsym0("ping",2),Qsym0("pingx",4)));
	}

void run_tests(void)
	{
	test_math();
	test_all_str();
	test_all_eval();
	test_all_lam();
	test_all_buf();
	test_all_resolve();
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
