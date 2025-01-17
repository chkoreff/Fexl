#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_num.h>
#include <type_record.h>
#include <type_str.h>

static void clear_record(value f)
	{
	struct record *rec = f->v_ptr;
	unsigned long i;
	// Drop each item in record
	for (i = 0; i < rec->count; i++)
		{
		struct item *item = rec->vec + i;
		drop(item->key);
		drop(item->val);
		}
	free_memory(rec->vec, sizeof(struct item[rec->size]));
	free_memory(rec,sizeof(struct record));
	}

// LATER 20221213 Perhaps use binary search.
value record_find(struct record *rec, string key)
	{
	unsigned long pos;

	for (pos = 0; pos < rec->count; pos++)
		{
		struct item *item = rec->vec + pos;
		int cmp = str_cmp(key,item->key->v_ptr);
		if (cmp > 0)
			;
		else if (cmp == 0)
			return item->val;
		else
			break;
		}
	return 0;
	}

value type_record(value f)
	{
	value key = arg(f->R);
	if (key->T == type_str)
		{
		value val = record_find(f->L->v_ptr,key->v_ptr);
		if (val == 0) val = Qvoid;
		f = hold(val);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}

static value Qrecord(struct record *rec)
	{
	static struct value clear = {{.N=0}, {.clear=clear_record}};
	return V(type_record,&clear,(value)rec);
	}

static struct record *new_record(unsigned long count, unsigned long size)
	{
	struct record *rec = new_memory(sizeof(struct record));
	rec->count = count;
	rec->size = size;
	rec->vec = new_memory(sizeof(struct item[size]));
	return rec;
	}

static unsigned long calc_size(unsigned long count)
	{
	unsigned long extra = count >> 1;
	const unsigned long min = 5;
	if (extra < min) extra = min;
	return count + extra;
	}

static void bump(struct record *old_rec)
	{
	// LATER 20241119 Use realloc
	unsigned long count = old_rec->count;
	unsigned long size = calc_size(count+1);
	struct item *new_vec = new_memory(sizeof(struct item[size]));
	struct item *old_vec = old_rec->vec;
	unsigned long i;

	for (i = 0; i < count; i++)
		new_vec[i] = old_vec[i];

	free_memory(old_vec, sizeof(struct item[old_rec->size]));

	old_rec->size = size;
	old_rec->vec = new_vec;
	}

static void shift_items(struct item *vec, unsigned long n, unsigned long pos)
	{
	unsigned long i;
	for (i = n; i > pos; i--)
		vec[i] = vec[i-1];
	}

void record_set(value obj, value key, value val)
	{
	struct record *rec = obj->v_ptr;
	string s_key = key->v_ptr;
	unsigned long pos;

	for (pos = 0; pos < rec->count; pos++)
		{
		struct item *item = rec->vec + pos;
		int cmp = str_cmp(s_key,item->key->v_ptr);
		if (cmp > 0)
			;
		else if (cmp == 0)
			{
			// Change the value.
			drop(item->val);
			item->val = val;
			return;
			}
		else
			break;
		}

	// Insert new item.
	if (rec->count >= rec->size)
		bump(rec);

	shift_items(rec->vec,rec->count,pos);
	rec->count++;

	{
	struct item *item = rec->vec + pos;
	item->key = hold(key);
	item->val = val;
	}
	}

static value record_copy(value obj)
	{
	struct record *old_rec = obj->v_ptr;
	unsigned long count = old_rec->count;
	struct record *new_rec = new_record(count,old_rec->size);
	struct item *old_vec = old_rec->vec;
	struct item *new_vec = new_rec->vec;
	unsigned long i;

	for (i = 0; i < count; i++)
		{
		struct item *old_item = old_vec+i;
		struct item *new_item = new_vec+i;
		new_item->key = hold(old_item->key);
		new_item->val = hold(old_item->val);
		}

	return Qrecord(new_rec);
	}

// Return an empty record.
value record_empty(void)
	{
	unsigned long count = 0;
	unsigned long size = calc_size(count);
	return Qrecord(new_record(count,size));
	}

value type_empty(value f)
	{
	return record_empty();
	(void)f;
	}

// LATER 20241120 In a new version I am eliminating the automatic copy based on
// reference count.  Instead all records will be mutable and you can make an
// explicit copy if you like.
static value record_set_copy(value obj, value key, value val)
	{
	if (obj->N <= 2)
		hold(obj);
	else
		obj = record_copy(obj);

	record_set(obj,key,val);
	return obj;
	}

static value op_set(value f, value op(value))
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value key = arg(f->L->L->R);
	if (key->T == type_str)
		{
		value obj = arg(f->R);
		if (obj->T == type_record)
			f = record_set_copy(obj,key,op(f->L->R));
		else
			f = hold(Qvoid);
		drop(obj);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}
	}

// (set key val obj) Set key to val in obj, after evaluating val.
// This returns a record like obj but with key mapped to val.  It modifies obj
// inline if there are no other references to it; otherwise it returns a
// modified copy of obj.
value type_set(value f)
	{
	return op_set(f,arg);
	}

// (setf key val obj) Set key to val in obj, without evaluating val.
value type_setf(value f)
	{
	return op_set(f,hold);
	}

static value op_SET(value f, value op(value))
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value obj = arg(f->L->L->R);
	if (obj->T == type_record)
		{
		value key = arg(f->L->R);
		if (key->T == type_str)
			{
			record_set(obj,key,op(f->R));
			f = hold(QI);
			}
		else
			f = hold(Qvoid);
		drop(key);
		}
	else
		f = hold(Qvoid);
	drop(obj);
	return f;
	}
	}

// (SET obj key val) Set key to val in obj, after evaluating val.
value type_SET(value f)
	{
	return op_SET(f,arg);
	}

// (SETF obj key val) Set key to val in obj, without evaluating val.
value type_SETF(value f)
	{
	return op_SET(f,hold);
	}

// Look up key in record and return either no or (yes val).
value type_get(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value key = arg(f->L->R);
	if (key->T == type_str)
		{
		value obj = arg(f->R);
		if (obj->T == type_record)
			f = maybe(record_find(obj->v_ptr,key->v_ptr));
		else
			f = hold(Qvoid);
		drop(obj);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}
	}

value type_record_copy(value f)
	{
	value obj = arg(f->R);
	if (obj->T == type_record)
		f = record_copy(obj);
	else
		f = hold(Qvoid);
	drop(obj);
	return f;
	}

// Return the number of items in the record.
value type_record_count(value f)
	{
	value obj = arg(f->R);
	if (obj->T == type_record)
		f = Qnum((double)((struct record *)obj->v_ptr)->count);
	else
		f = hold(Qvoid);
	drop(obj);
	return f;
	}

// (record_item obj pos) Return the {key val} pair in record obj at offset pos,
// starting at zero.
value type_record_item(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value obj = arg(f->L->R);
	if (obj->T == type_record)
		{
		value vpos = arg(f->R);
		if (vpos->T == type_num)
			{
			struct record *rec = obj->v_ptr;
			unsigned long pos = get_ulong(vpos);
			if (pos < rec->count)
				{
				struct item *item = rec->vec+pos;
				f = pair(hold(item->key),hold(item->val));
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(vpos);
		}
	else
		f = hold(Qvoid);
	drop(obj);
	return f;
	}
	}
