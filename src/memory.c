#include <stdio.h>

#include <die.h>
#include <file.h>
#include <memory.h>
#include <stdlib.h> // malloc realloc free

// Track the amount of memory used so I can detect memory leaks.  Normally this
// is impossible but I check it anyway in case of software error.
unsigned long cur_blocks = 0;
unsigned long cur_bytes = 0;

static void out_of_memory(void)
	{
	die("The program ran out of memory.");
	}

// Return a new span of memory of size num_bytes, or die if not possible.
void *new_memory(unsigned long num_bytes)
	{
	if (num_bytes == 0) die("NEW0");
	{
	void *data = malloc(num_bytes);
	if (data == 0) out_of_memory();
	cur_blocks++;
	cur_bytes += num_bytes;
	return data;
	}
	}

// Change a span of memory from o_size bytes to n_size bytes.
void *resize_memory(void *data, unsigned long o_size, unsigned long n_size)
	{
	if (data == 0) cur_blocks++;
	data = realloc(data, n_size);
	if (data == 0) out_of_memory();
	cur_bytes -= o_size;
	cur_bytes += n_size;
	return data;
	}

// Free a previously allocated span of memory.
void free_memory(void *data, unsigned long num_bytes)
	{
	if (!data) die("NFREE");
	if (cur_blocks == 0 || cur_bytes < num_bytes)
		die("XFREE");

	free(data);
	cur_blocks--;
	cur_bytes -= num_bytes;
	}

// Detect any final memory leak, which should never happen.
void end_memory(void)
	{
	if (cur_blocks || cur_bytes)
		{
		fput(stderr,"LEAK");
		fput(stderr," ");fput_ulong(stderr,cur_blocks);
		fput(stderr," ");fput_ulong(stderr,cur_bytes);
		die("");
		}
	}
