struct item
	{
	value key;
	value val;
	};

struct record
	{
	unsigned long count;
	unsigned long len;
	struct item item[0];
	};

extern value Qempty;
extern struct record *get_record(value x);
extern value type_record(value f);
extern value record_empty(void);
extern value type_set(value f);
extern value type_record_count(value f);
extern value type_record_item(value f);