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
static value find(struct record *rec, string key)
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
	value x = arg(f->R);
	if (x->T == type_str)
		{
		value val = find(f->L->v_ptr,x->v_ptr);
		if (val == 0) val = Qvoid;
		f = hold(val);
		}
	else
		f = hold(Qvoid);
	drop(x);
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

static void set_inline(value x, value y, value z)
	{
	struct record *rec = z->v_ptr;
	string key = x->v_ptr;
	unsigned long pos;

	for (pos = 0; pos < rec->count; pos++)
		{
		struct item *item = rec->vec + pos;
		int cmp = str_cmp(key,item->key->v_ptr);
		if (cmp > 0)
			;
		else if (cmp == 0)
			{
			// Update inline.
			drop(item->val);
			item->val = hold(y);
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
	item->key = hold(x);
	item->val = hold(y);
	}
	}

static value copy_record(value z)
	{
	struct record *old_rec = z->v_ptr;
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

static value set(value x, value y, value z)
	{
	if (z->N <= 2)
		hold(z);
	else
		z = copy_record(z);

	set_inline(x,y,z);
	return z;
	}

// Return an empty record.
value record_empty(void)
	{
	unsigned long count = 0;
	unsigned long size = calc_size(count);
	return Qrecord(new_record(count,size));
	}

static value op_set(value f, value op(value))
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
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

// (set key val obj) Set key to val in record obj, returning a record like obj
// but with key mapped to val.  It modifies obj inline if there are no other
// references to it; otherwise it returns a modified copy of obj.
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
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_str)
		{
		value y = arg(f->R);
		if (y->T == type_record)
			f = maybe(find(y->v_ptr,x->v_ptr));
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

// Return the number of items in the record.
value type_record_count(value f)
	{
	value x = arg(f->R);
	if (x->T == type_record)
		f = Qnum((double)((struct record *)x->v_ptr)->count);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// (record_item obj pos) Return the {key val} pair in record obj at offset pos,
// starting at zero.
value type_record_item(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_record)
		{
		value y = arg(f->R);
		if (y->T == type_num)
			{
			struct record *rec = x->v_ptr;
			unsigned long pos = get_ulong(y);
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
		drop(y);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
