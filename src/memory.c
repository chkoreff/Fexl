#include <die.h>
#include <memory.h>
#include <output.h>
#include <stdlib.h> /* malloc free */

/*
Track the amount of memory used so we can detect memory leaks.  Normally this
is impossible but we check it anyway in case of software error.
*/
unsigned long cur_blocks = 0;
unsigned long cur_bytes = 0;

unsigned long max_words = 8000000;
unsigned long cur_words = 0;

/*
Return a new span of memory of size num_bytes and cost num_words, or 0 if not
possible.  The cost is a portable measure of memory usage that is independent
of machine word size.  Here is the recommended cost schedule:

	unsigned long   1
	pointer         1
	double          1
	N bytes         (N >> 3)
*/
void *new_memory(unsigned long num_bytes, unsigned long num_words)
	{
	if (num_words > max_words - cur_words) return 0;
	if (num_bytes == 0) return 0;

	{
	void *data = malloc(num_bytes);
	if (!data) return 0;
	cur_blocks++;
	cur_bytes += num_bytes;
	cur_words += num_words;
	return data;
	}
	}

/* Free a previously allocated span of memory. */
void free_memory(void *data, unsigned long num_bytes, unsigned long num_words)
	{
	if (!data) die("NFREE");
	if (cur_blocks == 0 || cur_bytes < num_bytes || cur_words < num_words)
		die("XFREE");

	free(data);
	cur_blocks--;
	cur_bytes -= num_bytes;
	cur_words -= num_words;
	}

/* Detect any final memory leak, which should never happen. */
void end_memory(void)
	{
	if (cur_blocks || cur_bytes || cur_words)
		{
		put_to_error();
		put("LEAK");
		put(" ");put_ulong(cur_blocks);
		put(" ");put_ulong(cur_bytes);
		put(" ");put_ulong(cur_words);
		die("");
		}
	}
