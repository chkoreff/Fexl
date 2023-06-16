#include <str.h>
#include <value.h>

#include <stdio.h> // TODO

#include <basic.h>
#include <context.h>
#include <parse.h> // TODO find_item
#include <type_str.h>

#include <type_record.h>

value type_record(value fun, value arg)
	{
	return type_void(fun,arg);
	}

value set(value key, value val, value obj)
	{
	if (obj->L == 0)
		return V(type_record,A(key,val),obj);
	else
		{
		int cmp = str_cmp(key->v_ptr,obj->L->L->v_ptr);
		if (cmp < 0)
			return V(type_record,A(key,val),obj);
		else if (cmp == 0)
			{
			value tail = hold(obj->R);
			drop(obj);
			return V(type_record,A(key,val),tail);
			}
		else
			{
			value pair = hold(obj->L);
			value tail = hold(obj->R);
			drop(obj);
			return V(type_record,pair,set(key,val,tail));
			}
		}
	}

value type_set(value fun, value arg)
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
			return set(key,val,obj);
			}
		else
			return type_void(fun,arg);
		}
	}

value type_get(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_str);
	else
		{
		arg = eval(arg);
		if (arg->T == type_record)
			{
			// TODO Need to use the ordering property!
			value g = maybe(find_item(fun->R->v_ptr,arg));
			drop(fun);
			drop(arg);
			return g;
			}
		else
			return type_void(fun,arg);
		}
	}

static value chain(value new, value old)
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
			return V(type_record,new_pair,chain(new_tail,old));
			}
		else if (cmp == 0)
			{
			value new_pair = hold(new->L);
			value new_tail = hold(new->R);
			value old_tail = hold(old->R);
			drop(old);
			drop(new);
			return V(type_record,new_pair,chain(new_tail,old_tail));
			}
		else
			{
			value old_pair = hold(old->L);
			value old_tail = hold(old->R);
			drop(old);
			return V(type_record,old_pair,chain(new,old_tail));
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

void use_record(void)
	{
	define("empty", Q(type_record));
	define("set", Q(type_set));
	define("get", Q(type_get));
	define("::", Q(type_chain));
	}
