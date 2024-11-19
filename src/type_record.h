struct item
	{
	value key;
	value val;
	};

struct record
	{
	unsigned long count;
	unsigned long size;
	struct item *vec;
	};

extern value type_record(value f);
extern value record_empty(void);
extern value type_set(value f);
extern value type_setf(value f);
extern value type_get(value f);
extern value type_record_count(value f);
extern value type_record_item(value f);
