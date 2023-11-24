#include <stddef.h>
#include <str.h>

#include <buf.h>

#include <buf_str.h>
#include <string.h> // memcpy

// Add a string to the buffer.
void buf_put(buffer buf, string str)
	{
	buf_addn(buf,str->data,str->len);
	}

// Clear the buffer and return its content in a string.
string buf_clear(buffer buf)
	{
	string result = str_new(buf->len);
	memcpy(result->data,buf->data,buf->len);
	buf_discard(buf);
	return result;
	}
