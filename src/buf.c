#include "buf.h"
#include "memory.h"
#include <string.h>  /* memcpy */

void buf_add(struct buf **top, char ch)
	{
	struct buf *buf = *top;

	if (buf == 0)
		{
		buf = new_memory(sizeof(struct buf));
		*top = buf;
		buf->size = 0;
		buf->len = 0;
		buf->data = 0;
		buf->prev = 0;
		}

	if (buf->len >= buf->size)
		{
		long new_size = buf->size == 0 ? 32 : buf->size << 1;
		if (new_size > 1048576) new_size = 1048576;

		char *new_data = new_memory(new_size);

		if (buf->data)
			{
			struct buf *prev = new_memory(sizeof(struct buf));

			prev->size = buf->size;
			prev->len = buf->len;
			prev->data = buf->data;
			prev->prev = buf->prev;

			buf->len = 0;
			buf->prev = prev;
			}

		buf->data = new_data;
		buf->size = new_size;
		}

	buf->data[buf->len++] = ch;
	}

char *buf_clear(struct buf **top, long *final_len)
	{
	struct buf *buf = *top;

	struct buf *curr = buf;
	long len = 0;
	while (1)
		{
		if (curr == 0) break;
		len += curr->len;
		curr = curr->prev;
		}

	char *string = new_memory(len + 1);

	long offset = len;
	curr = buf;
	while (1)
		{
		if (curr == 0) break;
		offset -= curr->len;

		memcpy(string + offset, curr->data, curr->len);
		free_memory(curr->data, curr->size);

		struct buf *prev = curr->prev;
		free_memory(curr, sizeof(struct buf));
		curr = prev;
		}

	string[len] = '\000'; /* add trailing NUL byte */

	*final_len = len;
	return string;
	}
