#include <str.h>
#include <buffer.h>
#include <memory.h>
#include <string.h> /* memcpy */

void buf_free(buffer buf)
	{
	while (buf)
		{
		buffer next = buf->next;
		str_free(buf->str);
		free_memory(buf,sizeof(struct buffer));
		buf = next;
		}
	}

static buffer buf_push(buffer buf, const unsigned long size)
	{
	string top = str_new(size);
	buffer new = new_memory(sizeof(struct buffer));
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
