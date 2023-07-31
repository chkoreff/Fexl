struct item
	{
	value key;
	value val;
	};

struct record
	{
	unsigned long count;
	unsigned long len;
	struct item item[];
	};

extern value Qempty;
extern struct record *get_record(value x);
extern value type_record(value fun, value f);
extern value record_empty(void);
extern value type_set(value fun, value f);
extern value type_setf(value fun, value f);
extern value type_get(value fun, value f);
extern value type_record_count(value fun, value f);
extern value type_record_item(value fun, value f);
extern void beg_record(void);
extern void end_record(void);
