#include <assert.h>
#include <limits.h>
#include <test/math.h>

static int add_int(int x, int y)
	{
	return x + y;
	}

static int sub_int(int x, int y)
	{
	return x - y;
	}

static long add_long(long x, long y)
	{
	return x + y;
	}

static long sub_long(long x, long y)
	{
	return x - y;
	}

static void test_int(void)
	{
	assert(add_int(INT_MAX,add_int(INT_MIN,1)) == 0);
	assert(add_int(sub_int(INT_MAX,1),1) == INT_MAX);
	assert(add_int(INT_MAX,1) == INT_MIN);
	assert(add_int(0,INT_MIN) == INT_MIN);
	assert(sub_int(0,INT_MAX) < 0);
	assert(sub_int(0,1) < 0);
	{
	int x = 4;
	int y = sub_int(sub_int(INT_MAX,x),1);
	int z = add_int(add_int(x,y),1);
	assert(z == INT_MAX);
	}
	}

static void test_long(void)
	{
	assert(add_long(LONG_MAX,add_long(LONG_MIN,1)) == 0);
	assert(add_long(sub_long(LONG_MAX,1),1) == LONG_MAX);
	assert(add_long(LONG_MAX,1) == LONG_MIN);
	assert(add_long(0,LONG_MIN) == LONG_MIN);
	assert(sub_long(0,LONG_MAX) < 0);
	assert(sub_long(0,1) < 0);
	{
	long x = 4;
	long y = sub_long(sub_long(LONG_MAX,x),1);
	long z = add_long(add_long(x,y),1);
	assert(z == LONG_MAX);
	}
	}

/*
Test essential integer operations.  The places in the Fexl code which use the
built-in "+" and "-" operators ought to be immune from problems related to
integer overflow or underflow.  These places include memory.c and str.c.  As
long as this test routine passes, we should be good.
*/
void test_math(void)
	{
	test_int();
	test_long();
	}
