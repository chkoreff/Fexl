#include <stdio.h>
#include <sys/resource.h>
#include "value.h"
#include "basic.h"
#include "double.h"
#include "eval.h"
#include "io.h"
#include "long.h"
#include "memory.h"
#include "run.h"
#include "show.h"
#include "string.h"
#include "sym.h"

value C;
value S;
value I;
value Y;
value R;
value L;
value query;

value long_add;
value long_sub;
value long_mul;
value long_div;

value double_add;
value double_sub;
value double_mul;
value double_div;

value tower(int height, value f)
	{
	int i;
	for (i = 0; i < height; i++)
		f = A(f,f);

	return f;
	}

long depth(value f)
	{
	if (f && f->L)
		return 1 + depth(f->L);
	else
		return 0;
	}

void test_stack_depth(void)
	{
	#if 0
	long test_depth = 1000000000; /* too much memory */
	long test_depth = 120000000; /* 5.63g */
	long test_depth = 100000000; /* 4.69g */
	long test_depth = 10000000; /* 469m */
	long test_depth = 1000000; /* 47m */
	long test_depth = 100000; /* 5m */
	#endif

	long test_depth = 10000000; /* 469m */

	printf("using test_depth = %ld\n", test_depth);
	value f = tower(test_depth,I);

	hold(f);
	long n = depth(f);
	printf("calculated depth = %ld\n", n);

	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	printf("ru_utime = %ld.%06lds\n",
		ru.ru_utime.tv_sec,
		ru.ru_utime.tv_usec);
	printf("ru_maxrss = %ldk\n", ru.ru_maxrss);
	printf("ru_majflt = %ld\n", ru.ru_majflt);

	drop(f);
	}

/*LATER use fork for tests that use too much memory or run too long */
void test_eval_plain(value f)
	{
	cur_steps = 0;

	hold(f);
	eval(f);

	if (max_steps >= 0) printf("That took %ld steps.\n", cur_steps);
	print("= "); show(f);nl();

	drop(f);
	nl();
	}

void test_eval(value f)
	{
	print(": ");show(f);nl();
	test_eval_plain(f);
	}

#if 0
void test_eval_max_steps(value f, long new_max_steps)
	{
	long save = max_steps;
	max_steps = new_max_steps;
	printf("Run with max_steps %ld ...\n", max_steps);
	test_eval(f);
	max_steps = save;
	}
#endif

void test_eval_tower(int height, value f)
	{
	printf(": (tower %d ", height); show(f); print(")\n");
	f = tower(height,f);
	if (height < 8)
		{
		print(": ");show(f);nl();
		}
	test_eval_plain(f);
	}

void test_overflow(long x, long y)
	{
	long z = x + y;
	int ok = z > x && z > y;

	printf("check %ld + %ld = %ld : %s\n", x, y, z,
		(ok ? "OK" : "OVERFLOW"));
	}

void test_fexl(void)
	{
	test_overflow(9223372036854775806,1);
	test_overflow(9223372036854775807,1);
	test_overflow(9223372036854775000,807);
	test_overflow(9223372036854775000,808);

	test_overflow(1,9223372036854775806);
	test_overflow(1,9223372036854775807);
	test_overflow(807,9223372036854775000);
	test_overflow(808,9223372036854775000);

	#if 0
	/* This one runs forever. */
	test_eval_max_steps(A(Y,I),-1);
	#endif

	test_eval(C);
	test_eval(S);
	test_eval(A(C,C));
	test_eval(A(C,S));
	test_eval(A(A(C,C),C));
	test_eval(A(A(C,I),C));
	test_eval(A(A(C,S),C));
	test_eval(A(A(C,S),C));
	test_eval(A(I,S));
	test_eval(A(I,A(I,S)));
	test_eval(A(I,Y));
	test_eval(A(R,I));
	test_eval(A(L,I));
	test_eval(A(R,R));

	test_eval(A(A(C,I),S)); /* I */
	test_eval(A(A(A(C,I),S),A(I,Y)));  /* Y */
	test_eval(
		A(A(A(C,A(A(C,I),C)),S),A(I,
		A(A(A(C,I),S),A(A(A(C,I),S),Y))
		))); /* Y */

	{
	value x = Qname(Qcopy_string("x"));
	value y = Qname(Qcopy_string("y"));
	value z = Qname(Qcopy_string("z"));

	hold(x);
	hold(y);
	hold(z);

	test_eval(x);
	test_eval(A(R,x));
	test_eval(A(A(R,x),y));
	test_eval(A(A(A(R,x),y),z));
	test_eval(A(L,x));
	test_eval(A(A(L,x),y));
	test_eval(A(A(A(L,x),y),z));

	test_eval(A(Y,x));
	test_eval(A(Y,A(C,x)));

	drop(x);
	drop(y);
	drop(z);
	}

	test_eval(A(A(query,A(A(long_mul,Qlong(3)),Qlong(5))),
		A(long_sub,Qlong(-23))));

	test_eval(A(A(query,A(A(long_div,Qlong(12)),Qlong(4))),
		A(long_mul,Qlong(7))));

	test_eval(A(A(query,A(A(long_mul,Qlong(3)),Qlong(5))),
		A(long_add,Qlong(7))));

	test_eval(A(A(long_sub,Qlong(12)),Qlong(23)));

	test_eval(A(A(query,A(A(long_div,Qlong(12)),Qlong(0))),
		A(long_mul,Qlong(7))));

	test_eval(A(long_div,Qlong(12)));

	test_eval(A(A(long_div,Qlong(12)),Qlong(4)));

	/* push operand onto stack */
	test_eval(A(A(query,A(A(long_add,Qlong(3)),Qlong(5))),
		A(long_add,Qlong(7))));

	/* already a normal form */
	test_eval(A(A(query,Qlong(8)),
		A(long_add,Qlong(7))));

	test_eval(A(A(query,A(I,C)),C));
	test_eval(A(A(query,C),C));
	test_eval(A(query,C));
	test_eval(query);

	{
	value outer_long_sub =
		A(A(L,query),A(A(R,A(L,query)),long_sub));

	test_eval(A(A(outer_long_sub,
		A(A(I,I),Qlong(3))),
		A(A(long_add,Qlong(4)),Qlong(5))
		));
	}

	test_eval(A(A(long_sub,A(I,Qlong(3))),Qlong(4)));
	test_eval(A(A(long_add,Qlong(3)),Qlong(5)));
	test_eval(A(A(long_sub,Qlong(3)),Qlong(4)));
	test_eval(A(A(long_add,Qlong(3)),Qlong(4)));
	test_eval(A(A(long_add,Qlong(3)),A(I,Qlong(4))));
	test_eval(A(A(long_add,A(I,Qlong(3))),A(I,Qlong(4))));
	test_eval(A(A(long_add,A(I,Qlong(3))),Qlong(4)));
	test_eval(A(A(long_add,Qlong(3)),Qlong(4)));
	test_eval(A(Qlong(3),Qlong(4)));
	test_eval(Qlong(3));

	/* even numbers = C, odd numbers = C C */
	test_eval_tower(0,C);
	test_eval_tower(1,C);
	test_eval_tower(2,C);
	test_eval_tower(3,C);
	test_eval_tower(4,C);
	test_eval_tower(5,C);
	test_eval_tower(6,C);
	test_eval_tower(10,C);
	test_eval_tower(11,C);
	test_eval_tower(12,C);
	test_eval_tower(13,C);
	test_eval_tower(14,C);
	test_eval_tower(15,C);
	test_eval_tower(16,C);
	test_eval_tower(20,C);
	test_eval_tower(57,C);
	test_eval_tower(500,C);
	test_eval_tower(501,C);
	test_eval_tower(200000,C); /* 0.053s */
	test_eval_tower(200001,C); /* 0.053s */
	#if 0
	test_eval_tower(500000,C); /* 0.115s */
	test_eval_tower(500001,C); /* 0.115s */
	test_eval_tower(5000000,C);  /* 1.073s */
	test_eval_tower(10000001,C);  /* 2.018s */
	test_eval_tower(20000001,C);  /* 4.288s */
	test_eval_tower(50000000,C); /* 10.468s */
	test_eval_tower(50000001,C); /* 10.550s */
	#endif

	test_eval_tower(0,I);
	test_eval_tower(1,I);
	test_eval_tower(2,I);
	test_eval_tower(3,I);
	test_eval_tower(4,I);
	test_eval_tower(5,I);
	test_eval_tower(6,I);
	test_eval_tower(7,I);
	test_eval_tower(8,I);
	test_eval_tower(9,I);
	test_eval_tower(16,I);
	test_eval_tower(32,I);
	test_eval_tower(64,I);
	test_eval_tower(2048,I);
	test_eval_tower(4096,I);
	test_eval_tower(8192,I);
	test_eval_tower(16384,I);
	test_eval_tower(32768,I);
	test_eval_tower(65536,I);

	{
	/* This evaluates instantly (0.065s) in spite of its enormous exponential
	complexity.  That's because the evaluator replaces values inline.
	*/
	value f = tower(65536,tower(65536,tower(65536,tower(65536,I))));
	print(": (tower 65536; tower 65536; tower 65536; tower 65536; I)\n");
	test_eval_plain(f);
	}

	test_eval(Qcopy_string(""));
	test_eval(Qcopy_string("hello"));
	test_eval(Qcopy_chars("hello", 2));
	test_eval(Qcopy_chars("he\tllo", 6));
	test_eval(Qcopy_chars("he\0llo", 6)); /* embedded NUL */
	test_eval(Qcopy_chars("he\012llo", 6)); /* embedded LF */
	test_eval(Qcopy_string("heeeeeee\015llo")); /* embedded CR */

	test_eval(Qdouble(4.5));
	test_eval(A(long_add,Qdouble(4.5)));
	test_eval(A(double_add,Qdouble(4.5)));
	test_eval(A(double_div,Qdouble(4.5)));
	test_eval(A(A(double_add,Qdouble(4.5)),Qdouble(-7.2)));
	test_eval(A(A(double_sub,Qdouble(4.5)),Qdouble(-7.2)));
	test_eval(A(A(double_mul,Qdouble(4.5)),Qdouble(-7.2)));
	test_eval(A(A(double_div,Qdouble(4.5)),Qdouble(-7.2)));
	test_eval(A(A(double_div,Qdouble(4.5)),Qdouble(0.0)));
	test_eval(A(A(double_div,Qdouble(0.0)),Qdouble(0.0)));
	test_eval(A(C,double_add));
	test_eval(A(double_div,Qdouble(4.5)));
	test_eval(A(A(double_sub,Qdouble(4.5)),Qdouble(3.0)));
	test_eval(A(double_mul,A(A(double_mul,Qdouble(4.5)),Qdouble(3.0))));

	{
	value x = A(A(double_add,Qdouble(4.2)),Qdouble(2.4));
	value y = Qdouble(5.2);
	test_eval(A(A(double_mul,x),y));
	}

	/* Test some type errors */
	{
	test_eval(A(double_mul,A(A(double_mul,Qlong(4)),Qdouble(3.0))));
	test_eval(A(A(double_mul,Qdouble(3.2)),Qlong(4)));

	{
	value x = A(A(double_add,Qdouble(4.2)),Qlong(4));
	value y = A(A(double_mul,Qdouble(3.7)),Qdouble(5.2));
	test_eval(A(A(double_mul,x),y));
	}

	test_eval(
		A(
		A(double_mul,
			A(A(double_mul,Qdouble(4.5)),Qdouble(3.0))),
			A(A(double_mul,Qlong(7)),Qdouble(3.0))
		)
		);
	}

	if (0)
	{
	/* Test too much memory */
	/*LATER We'll test this by forking a separate fexl process.  We can't test
	it in this process because we already have a large free list from previous
	tests. */
	long save = max_bytes;
	max_bytes = 400;
	printf("Evaluate (tower 8 I) with %ld bytes of memory.\n", max_bytes);
	value f = tower(8,I);
	test_eval(f);
	max_bytes = save;
	}

	#if 0
	/*LATER test with fork */
	test_eval_max_steps(A(Y,I),50);
	test_eval_max_steps(A(Y,I),200000000); /* 3.837s */
	test_eval_max_steps(A(Y, A(A(S,C),C)), 1000000);
	#endif

	#if 0
	/*LATER test with fork */
	{
	/* Test too much memory */
	long save = max_bytes;
	max_bytes = 400000000;
	value f = A(Y,Y);
	printf("Evaluate with %ld bytes of memory ...\n", max_bytes);
	test_eval_plain(f);
	max_bytes = save;
	}
	#endif
	}

void beg_test(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
	query = Q(type_query);

	long_add = Q(type_long_add);
	long_sub = Q(type_long_sub);
	long_mul = Q(type_long_mul);
	long_div = Q(type_long_div);

	double_add = Q(type_double_add);
	double_sub = Q(type_double_sub);
	double_mul = Q(type_double_mul);
	double_div = Q(type_double_div);

	hold(C);
	hold(S);
	hold(I);
	hold(R);
	hold(L);
	hold(Y);
	hold(query);

	hold(long_add);
	hold(long_sub);
	hold(long_mul);
	hold(long_div);

	hold(double_add);
	hold(double_sub);
	hold(double_mul);
	hold(double_div);
	}

void end_test(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(R);
	drop(L);
	drop(Y);
	drop(query);
	drop(long_add);
	drop(long_sub);
	drop(long_mul);
	drop(long_div);

	drop(double_add);
	drop(double_sub);
	drop(double_mul);
	drop(double_div);
	}

/*
http://czyborra.com/utf/
man utf8
hexdump -C

for example:
U+00E5 => c3 a5
U+00FC => c3 bc
*/
void put_utf8(int c)
	{
	if (c < 0x80)
		putchar (c);
	else if (c < 0x800)
		{
		putchar (0xC0 | (c>>6));
		putchar (0x80 | (c & 0x3F));
		}
	else if (c < 0x10000)
		{
		putchar (0xE0 | (c>>12));
		putchar (0x80 | (c>>6 & 0x3F));
		putchar (0x80 | (c & 0x3F));
		}
	else if (c < 0x200000)
		{
		putchar (0xF0 | (c>>18));
		putchar (0x80 | (c>>12 & 0x3F));
		putchar (0x80 | (c>>6 & 0x3F));
		putchar (0x80 | (c & 0x3F));
		}
	}

/* LATER unicode support functions (see unistd.h)
LATER in the meantime, the fexl parser naturally accepts UTF-8 encoded programs,
allowing non-ASCII symbols and strings.  But what if the user has LANG set to a
UTF-16 encoding?  His source code would then have wide 16-bit chars, and the
parser would see NUL chars as we read each byte.  UTF-8 is really nice because
the parser doesn't even have to be aware that it's reading non-ASCII text.
*/
void try_utf(void)
	{
	print("åabcüdef\n");
	print("\xc3\xa5\x61\x62\x63\xc3\xbc\x64\x65\x66\x0a");
	put_utf8(0x0061); /* a */
	put_utf8(0x00FC); /* ü */
	put_utf8(0x00E5); /* å */
	put_utf8(0x2260); /* ≠ */
	put_utf8(0x000a); /* NL */
	}

int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);
	beg_test();
	test_fexl();
	end_test();
	end_value();
	return 0;
	}
