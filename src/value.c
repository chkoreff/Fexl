#include <stdio.h>
#include <stdlib.h>
#include "value.h"

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

/* Using this simple free list makes things about 60% faster. */

static struct value *free_list = 0;

/* Create a new value with the given type, left, and right data. */
struct value *new_value(
	struct type *type,
	struct value *left,
	struct value *right)
	{
	struct value *value;
	if (free_list)
		{
		value = free_list;
		free_list = value->R;
		}
	else
		value = (struct value *)new_memory(sizeof (struct value));

	value->N = 0;
	value->T = type;
	value->L = left;
	value->R = right;
	type->hold(value);

	return value;
	}

/* Increment the reference count. */
void hold(struct value *value)
	{
	value->N++;
	}

/* Decrement the reference count and free the memory if no longer used. */
void drop(struct value *value)
	{
	if (--value->N <= 0)
		{
		value->T->drop(value);
		value->R = free_list;
		free_list = value;
		}
	}

/* Do nothing to the value and return 0. */
int reduce_none(struct value *value)
	{
	return 0;
	}

/* Hold both sides of a value, either of which can be zero. */
void hold_both(struct value *value)
	{
	if (value->L) hold(value->L);
	if (value->R) hold(value->R);
	}

/* Drop both sides of a value, either of which can be zero. */
void drop_both(struct value *value)
	{
	if (value->L) drop(value->L);
	if (value->R) drop(value->R);
	}

/* Replace the contents of value with the contents of other. */
void replace(struct value *value, struct value *other)
	{
	if (value == other) return;

	hold(other);

	value->T->drop(value);
	value->T = other->T;
	value->L = other->L;
	value->R = other->R;
	value->T->hold(value);

	drop(other);
	}

/* Here is the evaluation system. */

/* Track the number of evaluation steps and impose an upper limit. */
long total_steps = 0;
long max_steps = -1;  /* no limit initially */

/*
Track the depth of evaluation on the left side of a value and impose an upper
limit.  This prevents a degenerate function like (Y Y) from causing a system
stack overflow and segmentation fault.  That's usually harmless, but this might
be a little safer.

On some machines you can use a higher value for max_depth, but it's pointless
because no reasonable function should ever require such a deep recursion on the
left side.
*/
int max_depth = 100000;
int total_depth = 0;

/*
Evaluate a value, reducing it to a final normal form if possible within the
limits on time and space.
*/
void evaluate(struct value *value)
	{
	if (++total_depth > max_depth)
		{
		fprintf(stderr,
			"Your program tried to evaluate at a depth greater than %d.\n",
			max_depth);
		exit(1);
		}

	while (1)
		{
		if (max_steps >= 0 && ++total_steps > max_steps)
			{
			fprintf(stderr,
				"Your program tried to run more than %ld steps.\n",
				max_steps);
			exit(1);
			}

		if (!value->T->reduce(value)) break;
		}

	total_depth--;
	}

void exit_bad_type(struct value *value)
	{
	fprintf(stderr, "Unexpected data type: %s\n", value->T->name);
	exit(1);
	}

/* Evaluate a value and ensure it's an atom of the given type. */
void evaluate_type(struct value *value, struct type *type)
	{
	evaluate(value);
	if (value->T == type && !value->L) return;
	exit_bad_type(value);
	}

/* Detect any final memory leak when the program ends. */
void finish(void)
	{
	while (free_list)
		{
		struct value *next = free_list->R;
		free_memory(free_list, sizeof(struct value));
		free_list = next;
		}

	if (total_blocks || total_bytes)
		{
		fprintf(stderr, "Memory leak!  "
		"The system did not free precisely the memory it allocated.\n");
		fprintf(stderr, "  total_blocks = %ld\n", total_blocks);
		fprintf(stderr, "  total_bytes  = %ld\n", total_bytes);
		exit(1);
		}
	}
