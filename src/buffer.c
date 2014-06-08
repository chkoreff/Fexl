#include <str.h>

#include <buffer.h>
#include <die.h>
#include <limits.h>
#include <memory.h>
#include <string.h>

void buf_start(buffer *buf)
	{
	buf->str = 0;
	buf->pos = 0;
	buf->next = 0;
	}

void buf_add(buffer *buf, char ch)
	{
	const unsigned long first = 8;
	const unsigned long max = 1048576;

	if (buf->str == 0)
		buf->str = str_new(first);
	else if (buf->pos == buf->str->len)
		{
		/* Safe because ULONG_MAX is guaranteed at least 2^32-1. */
		buffer *new;
		unsigned long size = 2 * buf->pos;
		if (size > max) size = max;
		new = new_memory(sizeof(buffer));
		new->pos = 0; /* not used */
		new->str = buf->str;
		new->next = buf->next;

		buf->pos = 0;
		buf->str = str_new(size);
		buf->next = new;
		}

	if (buf->pos >= buf->str->len) die("buf_add");
	buf->str->data[buf->pos++] = ch;
	}

/* Clear the buffer and return its contents in a string, or 0 if empty. */
string buf_finish(buffer *buf)
	{
	buffer *list = buf->next;
	unsigned long len = buf->pos;
	string result;

	if (buf->str == 0)
		return 0;

	/* Calculate total length. */
	while (list)
		{
		if (list->str->len > ULONG_MAX - len) die("buf_finish");
		len += list->str->len;
		list = list->next;
		}

	/* Copy chunks into result string. */
	result = str_new(len);

	{
	unsigned long offset = len - buf->pos;
	memcpy(result->data + offset, buf->str->data, buf->pos);
	str_free(buf->str);

	list = buf->next;
	while (list)
		{
		buffer *old = list;
		buf->str = list->str;
		offset -= buf->str->len;
		memcpy(result->data + offset, buf->str->data, buf->str->len);
		str_free(buf->str);
		list = list->next;
		free_memory(old, sizeof(buffer));
		}
	}

	result->data[len] = '\000'; /* Add trailing NUL byte. */
	return result;
	}
