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
	return (struct record *)x->R;
	}

static unsigned long record_size(unsigned long count)
	{
	return sizeof(struct record) + count*sizeof(struct item);
	}

static void record_free(struct record *rec)
	{
	unsigned long i;
	/* Drop each item in record */
	for (i = 0; i < rec->count; i++)
		{
		struct item *item = &rec->item[i];
		drop(item->key);
		drop(item->val);
		}
	free_memory(rec, record_size(rec->count));
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
	static struct value atom = {0, (type)record_free};
	return V(type_record,&atom,(value)rec);
	}

static struct record *new_record(unsigned long count)
	{
	struct record *rec = new_memory(record_size(count));
	rec->count = count;
	return rec;
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

static value insert(struct record *old_rec, unsigned long pos,
	value x, value y)
	{
	struct record *new_rec = new_record(old_rec->count+1);

	copy(old_rec->item, new_rec->item, pos);
	{
	struct item *new_item = &new_rec->item[pos];
	new_item->key = hold(x);
	new_item->val = hold(y);
	}
	copy(old_rec->item+pos, new_rec->item+pos+1, old_rec->count-pos);

	return Qrecord(new_rec);
	}

static value update(struct record *old_rec, unsigned long pos, value y)
	{
	struct record *new_rec = new_record(old_rec->count);

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
		return update(old_rec,pos,y);
	else
		return insert(old_rec,pos,x,y);
	}

/* Return an empty record. */
value record_empty(void)
	{
	return Qrecord(new_record(0));
	}

/* (set key val obj) Set key to val in record obj, returning a record like obj
but with key mapped to val.  It modifies obj inline if there are no other
references to it; otherwise it returns a modified copy of obj. */
value type_set(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	if (x->T == type_str)
		{
		value y = f->L->R;
		value z = arg(f->R);
		if (z->T == type_record)
			f = set(x,y,z);
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
