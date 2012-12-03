/*
Track the amount of memory used so we can detect memory leaks.  Normally this
is impossible but we check it anyway in case of software error.
*/

static long total_blocks = 0;
static long total_bytes = 0;

/*
Return a new unused span of memory of the given size, or die if not possible.
We ensure that the new total byte count is strictly greater than the old total.
This prevents errors such as allocating zero bytes, negative bytes, or so many
bytes that it causes an integer overflow.
*/
void *new_memory(long num_bytes)
	{
	long new_total_bytes = total_bytes + num_bytes;

	void *data = new_total_bytes > total_bytes ? malloc(num_bytes) : 0;

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
