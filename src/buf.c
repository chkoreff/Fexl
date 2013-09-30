#include <string.h>
#include "buf.h"
#include "memory.h"
#include "str.h"

void buf_start(struct buf *buf)
	{
	buf->top = 0;
	}

/* Add a single character to a buffer.  This does very fast buffering, with no
copying needed as the buffer expands dynamically. */
void buf_add(struct buf *buf, char ch)
	{
	struct buf_chunk *chunk = buf->top;
	if (chunk == 0 || chunk->len >= chunk->size)
		{
		/* Start with block size 32, then double each time up to 1MB limit. */
		long new_size = chunk == 0 ? 32 : chunk->size << 1;
		if (new_size > 1048576) new_size = 1048576;

		chunk = new_memory(sizeof(struct buf_chunk));
		chunk->data = new_memory(new_size);
		chunk->size = new_size;
		chunk->len = 0;
		chunk->prev = buf->top;
		buf->top = chunk;
		}

	chunk->data[chunk->len++] = ch;
	}

/* Clear the buffer and return its contents in a string. */
extern struct str *buf_finish(struct buf *buf)
	{
	/* Determine total number of bytes held in buffer. */
	long length = 0;
	{
	struct buf_chunk *chunk = buf->top;
	while (chunk)
		{
		length += chunk->len;
		chunk = chunk->prev;
		}
	}

	/* Allocate a string which can hold all the bytes plus trailing NUL. */
	struct str *str = str_new(length);

	/* Copy buffer bytes into the string, freeing chunks along the way. */
	{
	char *dest = str->data;
	long offset = length;

	struct buf_chunk *chunk = buf->top;
	while (chunk)
		{
		offset -= chunk->len;

		memcpy(dest + offset, chunk->data, chunk->len);
		free_memory(chunk->data, chunk->size);

		struct buf_chunk *prev = chunk->prev;
		free_memory(chunk, sizeof(struct buf_chunk));
		chunk = prev;
		}

	dest[length] = '\000'; /* add trailing NUL byte */
	}

	return str;
	}
