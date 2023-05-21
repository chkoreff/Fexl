#include <stdio.h>
#include <str.h>
#include <string.h> // strlen

#include <output.h>

void fputd(FILE *fh, const char *data, unsigned long len)
	{
	size_t n = fwrite(data,1,len,fh);
	(void)n;
	}

void fput(FILE *fh, const char *data)
	{
	fputd(fh,data,strlen(data));
	}

void fput_ch(FILE *fh, char ch)
	{
	fputd(fh,&ch,1);
	}

void fput_str(FILE *fh, string x)
	{
	fputd(fh,x->data,x->len);
	}

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void put_double(double x)
	{
	printf("%.15g",x);
	}

void put_str(string x)
	{
	fput_str(stdout,x);
	}

void nl(void)
	{
	put_ch('\n');
	}
