#include <str.h>

#include <buf.h>
#include <memory.h>
#include <string.h> // memcpy

buffer buf_new(void)
	{
	buffer buf = new_memory(sizeof(struct buffer));
	buf->top = 0;
	return buf;
	}

static struct chunk *new_chunk(struct chunk *next, const unsigned long size)
	{
	struct chunk *new = new_memory(sizeof(struct chunk));
	new->pos = 0;
	new->str = str_new(size);
	new->next = next;
	return new;
	}

// Add a single char to the buffer.
void buf_add(buffer buf, char ch)
	{
	struct chunk *top = buf->top;
	if (!top || top->pos >= top->str->len)
		{
		unsigned long size = top ? top->pos : 16;
		if (size < 1048576) size <<= 1;
		top = new_chunk(top,size);
		buf->top = top;
		}
	top->str->data[top->pos++] = ch;
	}

// To add a whole string I simply add the characters one at a time.  I could
// optimize it later, but it's fine for now.
void buf_addn(buffer buf, const char *str, unsigned long len)
	{
	unsigned long pos;
	for (pos = 0; pos < len; pos++)
		buf_add(buf, str[pos]);
	}

// Add a string to the buffer.
void buf_put(buffer buf, string str)
	{
	buf_addn(buf,str->data,str->len);
	}

// Clear the buffer, discarding its contents.
void buf_discard(buffer buf)
	{
	struct chunk *chunk = buf->top;
	while (chunk)
		{
		struct chunk *next = chunk->next;
		str_free(chunk->str);
		free_memory(chunk,sizeof(struct chunk));
		chunk = next;
		}
	buf->top = 0;
	}

unsigned long buf_length(buffer buf)
	{
	unsigned long len = 0;
	struct chunk *chunk = buf->top;
	while (chunk)
		{
		len += chunk->pos;
		chunk = chunk->next;
		}
	return len;
	}

// Clear the buffer and return its content in a string.
string buf_clear(buffer buf)
	{
	struct chunk *chunk = buf->top;
	unsigned long offset = buf_length(buf);
	string result = str_new(offset);

	while (chunk)
		{
		struct chunk *next = chunk->next;
		offset -= chunk->pos;
		memcpy(result->data + offset, chunk->str->data, chunk->pos);
		chunk = next;
		}

	buf_discard(buf);
	return result;
	}

// Free a dynamically allocated buffer.
void buf_free(buffer buf)
	{
	buf_discard(buf);
	free_memory(buf,sizeof(struct buffer));
	}
