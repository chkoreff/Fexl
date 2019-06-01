#include <stdint.h>
#include <stdio.h>

#include <file.h>
#include <format.h>
#include <string.h> /* strlen */

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
