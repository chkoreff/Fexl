#include <num.h>
#include <stdio.h>
#include <str.h>

#include <file2.h>
#include <output2.h>

void put_str(string x)
	{
	fput_str(stdout,x);
	}

void put_num(number x)
	{
	fput_num(stdout,x);
	}
