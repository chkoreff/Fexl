#include <stddef.h>

#include <buf.h>
#include <memory.h>
#include <stdlib.h> // realloc
#include <string.h> // memcpy
#include <die.h>

// Grow buffer by delta bytes.
static void buf_grow(buffer buf, size_t delta)
	{
	size_t size = buf->size + delta;
	char *data = realloc(buf->data, size);

	if (data == NULL) die("Out of memory");
	if (buf->data == 0) cur_blocks++;
	cur_bytes += delta;

	buf->size = size;
	buf->data = data;
	}

// Ensure that the buffer has at least the needed capacity.
static void buf_need(buffer buf, size_t need)
	{
	size_t room = buf->size - buf->len; // current room in buffer
	if (room < need)
		{
		const size_t base = 64;
		const size_t mark = 1048576; // 2^20

		// If size == 0 set the size to base.  Otherwise increase the size by
		// 100% if size <= mark, or 50% if size > mark.

		size_t min_delta = need - room;
		size_t delta = buf->size;

		if (delta == 0)
			delta = base;
		else if (delta > mark)
			delta >>= 1;

		// In any case ensure at least the minimum needed.
		if (delta < min_delta)
			delta = min_delta;

		buf_grow(buf,delta);
		}
	}

// Add a char to the buffer.
void buf_add(buffer buf, char ch)
	{
	buf_need(buf,1);
	buf->data[buf->len++] = ch;
	}

// Add chars to the buffer.
void buf_addn(buffer buf, const char *str, size_t len)
	{
	buf_need(buf,len);
	memcpy(buf->data + buf->len,str,len);
	buf->len += len;
	}

// Discard buffer contents.
void buf_discard(buffer buf)
	{
	if (buf->data != 0) cur_blocks--;
	free(buf->data);
	cur_bytes -= buf->size;

	buf->len = 0;
	buf->size = 0;
	buf->data = 0;
	}
