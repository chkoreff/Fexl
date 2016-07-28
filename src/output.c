#include <stdio.h>
#include <format.h>
#include <output.h>
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
	fputd(fh,"\n",1);
	}
