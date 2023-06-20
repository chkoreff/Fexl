#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_str.h>

#include <type_record.h>

value Qempty;

static value get(string name, value obj)
	{
	while (obj->L)
		{
		int cmp = str_cmp(name, obj->L->L->v_ptr);
		if (cmp < 0)
			break;
		if (cmp == 0)
			return hold(obj->L->R);
		obj = obj->R;
		}
	return 0;
	}

value type_record(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_str)
		{
		value val = get(arg->v_ptr,fun);
		if (val)
			{
			drop(fun);
			drop(arg);
			return val;
			}
		else
			return type_void(fun,arg);
		}
	else
		return type_void(fun,arg);
	}

static value Qrecord(value pair, value obj)
	{
	return V(type_record,pair,obj);
	}

value def(value key, value val, value obj)
	{
	if (obj->L == 0)
		return Qrecord(Qpair(key,val),obj);
	else
		{
		int cmp = str_cmp(key->v_ptr,obj->L->L->v_ptr);
		if (cmp < 0)
			return Qrecord(Qpair(key,val),obj);
		else if (cmp == 0)
			{
			value tail = hold(obj->R);
			drop(obj);
			return Qrecord(Qpair(key,val),tail);
			}
		else
			{
			value pair = hold(obj->L);
			value tail = hold(obj->R);
			drop(obj);
			return Qrecord(pair,def(key,val,tail));
			}
		}
	}

static value set(value key, value val, value obj)
	{
	return def(key,eval(val),obj);
	}

static value op_def(value fun, value arg, value op(value,value,value))
	{
	if (fun->L == 0)
		return need(fun,arg,type_str);
	else if (fun->L->L == 0)
		return keep(fun,arg);
	else
		{
		arg = eval(arg);
		if (arg->T == type_record)
			{
			value key = hold(fun->L->R);
			value val = hold(fun->R);
			value obj = arg;
			drop(fun);
			return op(key,val,obj);
			}
		else
			return type_void(fun,arg);
		}
	}

value type_def(value fun, value arg) { return op_def(fun,arg,def); }
value type_set(value fun, value arg) { return op_def(fun,arg,set); }

value type_get(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_str);
	else
		{
		arg = eval(arg);
		if (arg->T == type_record)
			{
			value g = maybe(get(fun->R->v_ptr,arg));
			drop(fun);
			drop(arg);
			return g;
			}
		else
			return type_void(fun,arg);
		}
	}

value chain(value new, value old)
	{
	if (new->L == 0)
		{
		drop(new);
		return old;
		}
	else if (old->L == 0)
		{
		drop(old);
		return new;
		}
	else
		{
		int cmp = str_cmp(new->L->L->v_ptr, old->L->L->v_ptr);
		if (cmp < 0)
			{
			value new_pair = hold(new->L);
			value new_tail = hold(new->R);
			drop(new);
			return Qrecord(new_pair,chain(new_tail,old));
			}
		else if (cmp == 0)
			{
			value new_pair = hold(new->L);
			value new_tail = hold(new->R);
			value old_tail = hold(old->R);
			drop(old);
			drop(new);
			return Qrecord(new_pair,chain(new_tail,old_tail));
			}
		else
			{
			value old_pair = hold(old->L);
			value old_tail = hold(old->R);
			drop(old);
			return Qrecord(old_pair,chain(new,old_tail));
			}
		}
	}

value type_chain(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_record);
	else
		{
		arg = eval(arg);
		if (arg->T == type_record)
			{
			value new = hold(fun->R);
			value old = arg;
			drop(fun);
			return chain(new,old);
			}
		else
			return type_void(fun,arg);
		}
	}

// Get the list of pairs from a record (lazily).
value type_record_pairs(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_record)
		{
		if (arg->L == 0)
			{
			drop(fun);
			drop(arg);
			return hold(Qnull);
			}
		else
			{
			value item = hold(arg->L);
			value tail = A(fun,hold(arg->R));
			value g = V(type_list,item,tail);
			drop(arg);
			return g;
			}
		}
	else
		return type_void(fun,arg);
	}

void beg_record(void)
	{
	Qempty = Q(type_record);
	}

void end_record(void)
	{
	drop(Qempty);
	}
