#include <die.h>
#include <stdio.h>
#include <stdlib.h> // malloc free

#include <memory.h>

// Track the amount of memory used so I can detect memory leaks.  Normally this
// is impossible but I check it anyway in case of software error.
unsigned long cur_blocks = 0;
unsigned long cur_bytes = 0;

// Return a new span of memory of size num_bytes, or die if not possible.
void *new_memory(unsigned long num_bytes)
	{
	void *data = malloc(num_bytes);
	if (data == 0)
		die("The program ran out of memory.");

	cur_blocks++;
	cur_bytes += num_bytes;
	return data;
	}

// Free a previously allocated span of memory.
void free_memory(void *data, unsigned long num_bytes)
	{
	if (cur_blocks == 0 || cur_bytes < num_bytes)
		die("XFREE");

	free(data);
	cur_blocks--;
	cur_bytes -= num_bytes;
	}

void end_memory(void)
	{
	// Detect any final memory leak, which should never happen.
	if (cur_blocks || cur_bytes)
		{
		fprintf(stderr, "LEAK %lu %lu\n", cur_blocks, cur_bytes);
		die(0);
		}
	}
