#include <buf.h>
#include <die.h>
#include <stdio.h>
#include <str.h>
#include <string.h>
#include <sys/resource.h>

#include <value.h>
#include <parse.h>
#include <test/math.h>
#include <test/rlimit.h>
#include <test/show.h>
#include <type.h>
#include <type_str.h>
#include <type_sym.h>

/*LATER tests involving fork */
#if 0
string.h /* strlen */
stdlib.h /* exit */
sys/types.h /* pid_t */
sys/wait.h /* wait */
unistd.h /* fork */
#endif

value type_halt(value f, value g)
	{
	(void)g;
	return f;
	}

value type_ping(value f, value g)
	{
	(void)f;
	printf("ping!\n");
	return g;
	}

int show_atom_test(value f)
	{
	if (show_atom_base(f))
		;
	else if (f->T == type_halt)
		printf("halt");
	else if (f->T == type_ping)
		printf("ping");
	else
		return 0;

	return 1;
	}

void show(value f)
	{
	show_value(f, show_atom_test);
	}

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
	for (int i = 0; i < 5; i++)
		f = A(A(Q(type_concat),f),f);
	test_eval(f,0);
	}

	test_eval(Qstr0("x"),1);
	test_eval(A(A(C,Qstr0("x")),I),1);
	test_eval(A(y,Qstr0("x")),1);
	test_eval(A(A(x,I),C),1);
	test_eval(A(A(Q(type_halt),C),I),1);
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

	{
	value f = C;
	for (int i = 0; i < 24; i++)
		f = A(f,f);
	test_eval(f,0);
	}

	{
	value f = C;
	for (int i = 0; i < 33; i++)
		f = A(f,C);
	test_eval(f,0);
	}

	{
	/* This combinatorial explosion is quadratic but runs fast. */
	value f = I;
	for (int i = 0; i < 22; i++)
		f = A(f,f);
	test_eval(f,0);
	}

	if (0)
	{
	set_limit(RLIMIT_CPU, 2);
	/* Runs in constant space. */
	test_eval(A(Y,I),1); /* CPU time limit exceeded */
	}

	if (0)
	{
	test_eval(A(Y,Y),1); /* Segmentation fault */
	}

	if (0)
	{
	/*
	: \x\y x (y y)
	= \x R x (S I I)
	= L R (S I I)
	*/
	value y0 = A(A(L,R),A(A(S,I),I));
	value Y = A(A(S,y0),y0);
	test_eval(A(Y,I),1); /* Segmentation fault */
	}

	if (0)
	{
	set_limit(RLIMIT_CPU, 5);
	set_limit(RLIMIT_AS, 4000000);
	value I = A(A(S,C),C);
	value y0 = A(A(S,A(A(S,A(C,S)),C)),A(C,A(A(S,I),I)));
	value Y = A(A(S,y0),y0);
	value f = A(Y,I);
	test_eval(f,1); /* Your program ran out of memory. */
	}

	if (0)
	{
	/* Test using a data type incorrectly. */
	test_eval(A(Qstr0("x"),I),1);
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

	/*LATER test str_common, str_index, etc.*/
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

/*** Here are some different types of streams for testing. */

FILE *curr_fh = 0;
int get_file_ch(void)
	{
	return fgetc(curr_fh);
	}

const char *curr_data = 0;
int curr_pos = 0;
int curr_max = 0;

/*TODO not used, perhaps go ahead and make a parse string version */
int get_str_ch(void)
	{
	if (curr_pos >= curr_max)
		return -1;
	return curr_data[curr_pos++];
	}

int repeat_ch = 'a';
int get_test_ch(void)
	{
	if (curr_pos >= curr_max)
		return -1;
	curr_pos++;
	return repeat_ch;
	}

/***/

void show_parse(int get(void), const char *label)
	{
	int line = 1;
	value f = parse(get,&line,label);
	hold(f);
	printf("= ");show(f);nl();
	printf("  line %d\n", line);
	nl();
	drop(f);
	}

void test_parse_file(const char *name)
	{
	/* LATER test with stdio */
	curr_fh = name[0] ? fopen(name,"r") : stdin;
	if (curr_fh == 0) die("Could not open file %s", name);

	printf(": parse file %s\n", name);
	show_parse(get_file_ch,name);

	if (curr_fh != stdin)
		fclose(curr_fh);
	}

void test_parse_repeat(int ch, int max)
	{
	curr_pos = 0;
	curr_max = max;
	repeat_ch = ch;

	printf(": parse repeat '%c' %d\n", repeat_ch, curr_max);
	show_parse(get_test_ch,"test");
	}

void test_all_parse()
	{
	test_parse_file("test/in/a1");
	test_parse_file("test/in/a2");

	test_parse_repeat('a',60);
	test_parse_repeat('a',1);
	test_parse_repeat('a',0);

	/* LATER Test these pathological cases with fork.  Use -1 for no limit. */
	if (0)
	{
	set_limit(RLIMIT_CPU, 2);
	set_limit(RLIMIT_AS, 2000000);
	test_parse_repeat('a',600000);
	}
	if (0)
	{
	set_limit(RLIMIT_CPU, 1);
	set_limit(RLIMIT_AS, 2000000);
	test_parse_repeat(' ',300000000);
	}
	if (0)
	{
	set_limit(RLIMIT_CPU, 1);
	set_limit(RLIMIT_AS, 2000000);
	test_parse_repeat('(',300000000);
	}

	}

void test_buf(int max)
	{
	printf(": Buffering %d bytes\n", max);
	struct buf buf;
	buf_start(&buf);

	for (int len = 0; len < max; len++)
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
	struct sym *sym = atom_sym(x);
	if (sym->line < 0)
		return sym->name; /* literal string */

	const char *name = atom_str(sym->name)->data;
	if (strcmp(name,"ping") == 0)
		return ping;

	printf("  Undefined symbol %s on line %d\n", name, sym->line);
	return x;
	}

void test_resolve(value f)
	{
	hold(f);
	printf(": resolve ");show(f);nl();
	value g = resolve(f,context);
	hold(g);
	if (g->T == type_sym)
		printf("  The form has undefined symbols.\n");
	printf("= ");show(g);nl();
	#if 0
	/* resolve is a fixpoint */
	value h = resolve(g,context);
	hold(h);
	printf("= ");show(h);nl();
	drop(h);
	#endif
	nl();
	drop(f);
	drop(g);
	}

void run_tests(void)
	{
	test_math();
	test_all_str();
	test_all_eval();
	test_all_lam();
	test_all_parse();
	test_all_buf();

	test_resolve(app(x,y));
	test_resolve(app(y,x));
	test_resolve(app(Qsym0("x",-1),Qsym0("y",-2)));
	test_resolve(Qsym0("ping",2));
	test_resolve(app(Qsym0("ping",2),Qsym0("ping",4)));
	}

int main(void)
	{
	beg_base();
	beg_test();
	run_tests();
	end_test();
	end_base();
	end_value();
	return 0;
	}
