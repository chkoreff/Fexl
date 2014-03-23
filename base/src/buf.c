void buf_start(struct buf *buf)
	{
	buf->str = 0;
	buf->pos = 0;
	buf->next = 0;
	}

void buf_add(struct buf *buf, char ch)
	{
	const int first = 8;
	const int max = 1048576;

	if (buf->str == 0)
		buf->str = str_new(first);
	else if (buf->pos == buf->str->len)
		{
		int size = 2 * buf->pos;
		if (size > max) size = max;

		struct buf *new = new_memory(sizeof(struct buf));
		new->pos = 0; /* not used */
		new->str = buf->str;
		new->next = buf->next;

		buf->pos = 0;
		buf->str = str_new(size);
		buf->next = new;
		}

	assert(buf->pos <= buf->str->len);
	buf->str->data[buf->pos++] = ch;
	}

/* Clear the buffer and return its contents in a string, or 0 if empty. */
struct str *buf_finish(struct buf *buf)
	{
	if (buf->str == 0)
		return 0;

	struct buf *list = buf->next;

	/* Calculate total length. */
	int len = buf->pos;
	while (list)
		{
		len += list->str->len;
		assert(len > 0);
		list = list->next;
		}

	/* Copy chunks into result string. */
	struct str *result = str_new(len);

	int offset = len - buf->pos;
	memcpy(result->data + offset, buf->str->data, buf->pos);
	str_free(buf->str);

	list = buf->next;
	while (list)
		{
		buf->str = list->str;
		offset -= buf->str->len;
		memcpy(result->data + offset, buf->str->data, buf->str->len);
		str_free(buf->str);
		struct buf *old = list;
		list = list->next;
		free_memory(old, sizeof(struct buf));
		}

	result->data[len] = '\000'; /* Add trailing NUL byte. */
	return result;
	}
