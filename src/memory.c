#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

/* Track the amount of memory used and impose an upper limit. */
long total_blocks = 0;
long total_bytes = 0;
long max_bytes = -1;  /* no limit initially */

/*
Return a new unused span of memory of the given size, or exit if not possible.
*/
void *new_memory(long num_bytes)
	{
	total_blocks++;
	total_bytes += num_bytes;

	if (max_bytes >= 0 && total_bytes > max_bytes)
		{
		fprintf(stderr,
			"Your program tried to use more than %ld bytes of memory.\n",
			max_bytes);
		exit(1);
		}

	void *data = malloc(num_bytes);

	if (data == 0)
		{
		fprintf(stderr, "Your program ran out of memory.\n");
		exit(1);
		}

	return data;
	}

/* Free a previously allocated span of memory of the given size. */
void free_memory(void *data, long num_bytes)
	{
	if (!data)
		{
		fprintf(stderr, "The system tried to free a null pointer.\n");
		exit(1);
		}

	free(data);

	total_blocks--;
	total_bytes -= num_bytes;

	if (total_blocks < 0 || total_bytes < 0)
		{
		fprintf(stderr,
			"The system tried to free more memory than it allocated.\n");
		exit(1);
		}
	}

/* Detect any final memory leak when the program ends. */
void finish_memory(void)
	{
	if (total_blocks || total_bytes)
		{
		fprintf(stderr, "Memory leak!  "
		"The system did not free precisely the memory it allocated.\n");
		fprintf(stderr, "  total_blocks = %ld\n", total_blocks);
		fprintf(stderr, "  total_bytes  = %ld\n", total_bytes);
		exit(1);
		}
	}
