#include <stdio.h>
#include <stdlib.h>
#include "die.h"
#include "memory.h"

/*
Track the amount of memory used and impose an upper limit.  That way if you run
a dangerous function like (Y Y), which runs forever and uses unbounded amounts
of memory, your operating system won't frantically thrash around with virtual
memory and hang your machine.  I chose a max_bytes value which seems fairly
safe in practice.  This can be changed at run time.
*/

long total_blocks = 0;
long total_bytes = 0;
long max_bytes = 1000000000;

/*
Return a new unused span of memory of the given size, or die if not possible.
We ensure that the new total byte count is strictly greater than the old total.
This prevents errors such as allocating zero bytes, negative bytes, or so many
bytes that it causes an integer overflow.
*/
void *new_memory(long num_bytes)
	{
	long new_total_bytes = total_bytes + num_bytes;

	void *data =
		(new_total_bytes > total_bytes && new_total_bytes <= max_bytes)
		? malloc(num_bytes)
		: 0;

	if (data == 0)
		die("Your program ran out of memory.");

	total_bytes = new_total_bytes;
	total_blocks++;

	return data;
	}

/* Free a previously allocated span of memory of the given size. */
void free_memory(void *data, long num_bytes)
	{
	if (!data)
		die("The system tried to free a null pointer.");

	free(data);

	total_blocks--;
	total_bytes -= num_bytes;

	if (total_blocks < 0 || total_bytes < 0)
		die("The system tried to free more memory than it allocated.");
	}

/* Detect any final memory leak, which should never happen. */
void end_memory(void)
	{
	if (total_blocks || total_bytes)
		die("Memory leak!\n"
			"The system did not free precisely the memory it allocated.\n"
			"  total_blocks = %ld\n"
			"  total_bytes  = %ld",
			total_blocks, total_bytes);
	}
