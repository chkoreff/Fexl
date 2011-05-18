#include <stdio.h>
#include "value.h"
#include "parse.h"

#define TRACE 0

#if TRACE
#include "trace.h"
#endif

void run_fexl(FILE *source)
	{
	struct value *value = parse(source);

	hold(value);

	#if TRACE
	printf("beg: ");print_value(value);nl();
	#endif

	evaluate(value);

	#if TRACE
	max_steps = 1000;
	total_steps = 0;
	printf("end: ");print_value(value);nl();
	#endif

	drop(value);
	}

int main(int argc, char *argv[])
	{
	FILE *source = argc > 1 ? fopen(argv[1],"r") : stdin;
	if (!source)
		{
		fprintf(stderr,"I could not open the input file.\n");
		return 2;
		}

	run_fexl(source);

	fclose(source);
	finish();
	return 0;
	}
