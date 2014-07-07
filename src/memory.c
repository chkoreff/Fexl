#include <die.h>
#include <limits.h>
#include <memory.h>
#include <output.h>
#include <stdlib.h> /* malloc free */

/*
Track the amount of memory used so we can detect memory leaks.  Normally this
is impossible but we check it anyway in case of software error.
*/

static unsigned long total_blocks = 0;
static unsigned long total_bytes = 0;

/* Return a new unused span of memory, or die if not possible. */
void *new_memory(unsigned long num_bytes)
	{
	void *data = 0;
	if (total_blocks <= ULONG_MAX - 1 && total_bytes <= ULONG_MAX - num_bytes)
		data = malloc(num_bytes);

	if (data == 0)
		die("Your program ran out of memory.");

	total_blocks++;
	total_bytes += num_bytes;

	return data;
	}

/* Free a previously allocated span of memory of the given size. */
void free_memory(void *data, unsigned long num_bytes)
	{
	if (!data)
		die("The system tried to free a null pointer.");

	if (total_blocks == 0 || total_bytes < num_bytes)
		die("The system tried to free more memory than it allocated.");

	free(data);

	total_blocks--;
	total_bytes -= num_bytes;
	}

/* Detect any final memory leak, which should never happen. */
void end_memory(void)
	{
	if (total_blocks != 0 || total_bytes != 0)
		{
		putd = putd_err;
		put("Memory leak!\n");
		put("The system did not free precisely the memory it allocated.\n");
		put("  total_blocks = "); put_ulong(total_blocks); nl();
		put("  total_bytes  = "); put_ulong(total_bytes); nl();
		die("");
		}
	}
