/*
Track the amount of memory used so we can detect memory leaks.  Normally this
is impossible but we check it anyway in case of software error.
*/

static long total_blocks = 0;
static long total_bytes = 0;

/* Return a new unused span of memory, or die if not possible. */
void *new_memory(long num_bytes)
	{
	assert(num_bytes > 0);

	void *data = malloc(num_bytes);
	if (data == 0)
		die("Your program ran out of memory.");

	total_blocks++;
	assert(total_blocks > 0);

	total_bytes += num_bytes;
	assert(total_bytes > 0);

	return data;
	}

/* Free a previously allocated span of memory of the given size. */
void free_memory(void *data, long num_bytes)
	{
	assert(num_bytes > 0);

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
	if (total_blocks != 0 || total_bytes != 0)
		die("Memory leak!\n"
			"The system did not free precisely the memory it allocated.\n"
			"  total_blocks = %ld\n"
			"  total_bytes  = %ld",
			total_blocks, total_bytes);
	}
