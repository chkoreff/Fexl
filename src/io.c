#include <stdio.h>
#include <string.h> // strlen

#include <io.h>

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

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void nl(void)
	{
	put_ch('\n');
	}
