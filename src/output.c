#include <stdio.h>
#include <file.h>
#include <output.h>

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void put_ulong(unsigned long x)
	{
	fput_ulong(stdout,x);
	}

void nl(void)
	{
	fnl(stdout);
	}
