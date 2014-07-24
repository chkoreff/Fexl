#include <str.h>
#include <buffer.h>
#include <memory.h>
#include <string.h> /* memcpy */

static void buf_free(buffer buf)
	{
	while (buf)
		{
		buffer next = buf->next;
		str_free(buf->str);
		free_memory(buf,sizeof(struct buffer),3);
		buf = next;
		}
	}

static buffer buf_push(buffer buf, const unsigned long size)
	{
	string top = str_new(size);
	buffer new = top ? new_memory(sizeof(struct buffer),3) : 0;

	if (!new)
		{
		if (top) str_free(top);
		if (buf) buf_free(buf);
		return 0;
		}

	new->pos = 0;
	new->str = top;
	new->next = buf;
	return new;
	}

static unsigned long buf_length(buffer buf)
	{
	unsigned long len = 0;
	while (buf)
		{
		len += buf->pos;
		buf = buf->next;
		}
	return len;
	}

/* Return the buffer content in a string. */
static string buf_string(buffer buf)
	{
	unsigned long offset = buf_length(buf);
	string result = str_new(offset);
	if (!result) return 0;

	result->data[offset] = '\000'; /* Add trailing NUL byte. */

	while (buf)
		{
		offset -= buf->pos;
		memcpy(result->data + offset, buf->str->data, buf->pos);
		buf = buf->next;
		}

	return result;
	}

buffer buf_add(buffer buf, char ch)
	{
	if (!buf || buf->pos >= buf->str->len)
		{
		unsigned long size = buf ? buf->pos : 16;
		if (size < 1048576) size <<= 1;
		buf = buf_push(buf,size);
		if (!buf) return 0;
		}

	buf->str->data[buf->pos++] = ch;
	return buf;
	}

/* Clear the buffer and return its content. */
string buf_finish(buffer buf)
	{
	string result = buf_string(buf);
	buf_free(buf);
	return result;
	}
