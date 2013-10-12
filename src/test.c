#include <stdio.h>
#include "buf.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "form.h"
#include "qstr.h"

value type_ping(value f)
	{
	printf("Ping!\n");
	return I;
	}

value tower(value f, int n) /*LATER do in fexl*/
	{
	value dual = A(A(S,I),I);
	hold(dual);
	value result = f;
	int i;
	for (i = 0; i < n; i++)
		result = A(A(query,result),dual);

	drop(dual);
	return result;
	}

void test_buf(void) /*LATER do in fexl*/
	{
	printf("== test_buf\n");
	struct buf buf;
	buf_start(&buf);
	if (1)
	{
	buf_add(&buf,'a');
	buf_add(&buf,'b');
	buf_add(&buf,'\000');
	buf_add(&buf,'c');
	}
	else
	{
	int i;
	#if 0
	int n = 400000000;
	#endif
	int n = 30000000;
	for (i = 0; i < n; i++)
		buf_add(&buf,'a');
	}

	struct str *str = buf_finish(&buf);
	#if 0
	printf("name=\"%s\" len=%ld\n", str->data, str->len);
	#endif
	printf("len=%ld\n", str->len);
	str_free(str);
	}

value type_test_pattern(value f)
	{
	if (0)
		{
		return tower(C,55);
		return tower(I,46);
		}

	if (1)
	{
	test_buf();
	return I;
	}

	return I;
	}
