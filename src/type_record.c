#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_num.h>
#include <type_record.h>
#include <type_str.h>
#include <type_tuple.h>

value Qempty;

struct record *get_record(value x)
	{
	return x->v_ptr;
	}

static unsigned long record_size(unsigned long len)
	{
	return sizeof(struct record) + sizeof(struct item[len]);
	}

static void clear_record(value f)
	{
	struct record *rec = f->v_ptr;
	unsigned long i;
	/* Drop each item in record */
	for (i = 0; i < rec->count; i++)
		{
		struct item *item = &rec->item[i];
		drop(item->key);
		drop(item->val);
		}
	free_memory(rec, record_size(rec->len));
	}

/* LATER 20221213 Perhaps use binary search. */
static value find(struct record *rec, string key, value result)
	{
	unsigned long i;
	for (i = 0; i < rec->count; i++)
		{
		struct item *item = &rec->item[i];
		int cmp = str_cmp(key,get_str(item->key));
		if (cmp > 0)
			continue;
		if (cmp == 0)
			result = item->val;
		break;
		}
	return result;
	}

value type_record(value f)
	{
	if (!f->L->N) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = hold(find(get_record(f->L),get_str(x),Qvoid));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static value Qrecord(struct record *rec)
	{
	static struct value clear = {{.N=0}, {.clear=clear_record}};
	return V(type_record,&clear,(value)rec);
	}

static struct record *new_record(unsigned long count, unsigned long extra)
	{
	unsigned long len = count + extra;
	struct record *rec = new_memory(record_size(len));
	rec->count = count;
	rec->len = len;
	return rec;
	}

static struct record *new_record_bump(unsigned long count)
	{
	unsigned long extra = count >> 1;
	const unsigned long min = 5;
	if (extra < min) extra = min;
	return new_record(count,extra);
	}

static void insert_inline(struct record *old_rec, unsigned long pos,
	value x, value y)
	{
	unsigned long i;
	for (i = old_rec->count; i > pos; i--)
		{
		struct item *old_item = &old_rec->item[i-1];
		struct item *new_item = &old_rec->item[i];

		new_item->key = old_item->key;
		new_item->val = old_item->val;
		}
	{
	struct item *new_item = &old_rec->item[pos];
	new_item->key = hold(x);
	new_item->val = hold(y);
	}

	old_rec->count++;
	}

static void copy(struct item *old_vec, struct item *new_vec, unsigned long n)
	{
	unsigned long i;
	for (i = 0; i < n; i++)
		{
		struct item *old_item = &old_vec[i];
		struct item *new_item = &new_vec[i];
		new_item->key = hold(old_item->key);
		new_item->val = hold(old_item->val);
		}
	}

static value insert_copy(struct record *old_rec, unsigned long pos,
	value x, value y)
	{
	struct record *new_rec = new_record_bump(old_rec->count+1);

	copy(old_rec->item, new_rec->item, pos);
	{
	struct item *new_item = &new_rec->item[pos];
	new_item->key = hold(x);
	new_item->val = hold(y);
	}
	copy(old_rec->item+pos, new_rec->item+pos+1, old_rec->count-pos);

	return Qrecord(new_rec);
	}

static value update_copy(struct record *old_rec, unsigned long pos, value y)
	{
	struct record *new_rec = new_record(old_rec->count,0);

	copy(old_rec->item, new_rec->item, pos);
	{
	struct item *old_item = &old_rec->item[pos];
	struct item *new_item = &new_rec->item[pos];
	new_item->key = hold(old_item->key);
	new_item->val = hold(y);
	}
	copy(old_rec->item+pos+1, new_rec->item+pos+1, old_rec->count-pos-1);

	return Qrecord(new_rec);
	}

static value set(value x, value y, value z)
	{
	string key = get_str(x);
	struct record *old_rec = get_record(z);
	unsigned long pos;
	struct item *item;
	int cmp = -1;

	for (pos = 0; pos < old_rec->count; pos++)
		{
		item = &old_rec->item[pos];
		cmp = str_cmp(key,get_str(item->key));
		if (cmp <= 0)
			break;
		}

	if (cmp == 0)
		{
		if (z->N <= 2)
			{
			/* Update inline */
			drop(item->val);
			item->val = hold(y);
			return hold(z);
			}
		else
			return update_copy(old_rec,pos,y);
		}
	else
		{
		if (z->N <= 2 && old_rec->count < old_rec->len)
			{
			insert_inline(old_rec,pos,x,y);
			return hold(z);
			}
		else
			return insert_copy(old_rec,pos,x,y);
		}
	}

/* Return an empty record. */
value record_empty(void)
	{
	return Qrecord(new_record_bump(0));
	}

static value op_set(value f, value op(value))
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	if (x->T == type_str)
		{
		value z = arg(f->R);
		if (z->T == type_record)
			{
			value y = op(f->L->R);
			f = set(x,y,z);
			drop(y);
			}
		else
			f = hold(Qvoid);
		drop(z);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (set key val obj) Set key to val in record obj, returning a record like obj
but with key mapped to val.  It modifies obj inline if there are no other
references to it; otherwise it returns a modified copy of obj. */
value type_set(value f)
	{
	return op_set(f,arg);
	}

// Set the value without evaluating it.
value type_setf(value f)
	{
	return op_set(f,hold);
	}

// Look up key in record and return either no or (yes val).
value type_get(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_str)
		{
		value y = arg(f->R);
		if (y->T == type_record)
			f = maybe(find(get_record(y),get_str(x),0));
		else
			f = hold(Qvoid);
		drop(y);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Return the number of items in the record. */
value type_record_count(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_record)
		f = Qnum((double)get_record(x)->count);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (record_item obj pos) Return the {key val} pair in record obj at offset pos,
starting at zero. */
value type_record_item(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_record)
		{
		value y = arg(f->R);
		if (y->T == type_num)
			{
			struct record *rec = get_record(x);
			unsigned long pos = get_ulong(y);
			if (pos < rec->count)
				{
				struct item *item = &rec->item[pos];
				f = pair(hold(item->key),hold(item->val));
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(y);
		}
	else
		f = hold(Qvoid);
	drop(x);
	}
	return f;
	}

void beg_record(void)
	{
	Qempty = record_empty();
	}

void end_record(void)
	{
	drop(Qempty);
	}
