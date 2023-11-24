#include <stdint.h>
#include <stdio.h>

#include <file.h>
#include <format.h>
#include <string.h> // strlen

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

void fput_long(FILE *fh, long x)
	{
	fput(fh,format_long(x));
	}

void fput_ulong(FILE *fh, unsigned long x)
	{
	fput(fh,format_ulong(x));
	}

void fput_double(FILE *fh, double x)
	{
	fput(fh,format_double(x));
	}

void fnl(FILE *fh)
	{
	fput_ch(fh,'\n');
	}

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void put_long(long x)
	{
	fput_long(stdout,x);
	}

void put_ulong(unsigned long x)
	{
	fput_ulong(stdout,x);
	}

void put_double(double x)
	{
	fput_double(stdout,x);
	}

void nl(void)
	{
	fnl(stdout);
	}
