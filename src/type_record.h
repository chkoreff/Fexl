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
extern value record_copy(value obj);
extern value record_empty(void);
extern void record_fill(value obj, value list);
extern value type_load_record(value f);
