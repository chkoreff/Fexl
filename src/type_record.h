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

extern value record_find(value obj, value key);
extern value type_record(value f);
extern void record_set(value obj, value key, value val);
extern value record_empty(void);
extern value type_empty(value f);
extern value type_set(value f);
extern value type_setf(value f);
extern value type_del(value f);
extern value type_get(value f);
extern value type_record_copy(value f);
extern value type_record_count(value f);
extern value type_record_item(value f);
