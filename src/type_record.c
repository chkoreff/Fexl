#include <str.h>
#include <value.h>

#include <stdio.h> // TODO

#include <basic.h>
#include <context.h>
#include <parse.h> // TODO find_item
#include <type_str.h>

#include <die.h> // TODO
#include <show.h> // TODO

#include <type_record.h>

struct type type_record = { 0, apply_void, clear_T };

value set(value key, value val, value obj)
	{
	if (obj->L == 0)
		return V(&type_record,A(key,val),obj);
	else
		{
		int cmp = str_cmp(key->v_ptr,obj->L->L->v_ptr);
		if (cmp < 0)
			return V(&type_record,A(key,val),obj);
		else if (cmp == 0)
			{
			value tail = hold(obj->R);
			drop(obj);
			return V(&type_record,A(key,val),tail);
			}
		else
			{
			value pair = hold(obj->L);
			value tail = hold(obj->R);
			drop(obj);
			return V(&type_record,pair,set(key,val,tail));
			}
		}
	}

static value apply_set(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_str);
	else if (f->L->L == 0)
		return keep(f,x);
	else
		{
		x = eval(x);
		if (x->T == &type_record)
			return set(hold(f->L->R), hold(f->R), x);
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	}

static struct type type_set = { 0, apply_set, clear_T };

static value apply_get(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_str);
	else
		{
		x = eval(x);
		if (x->T == &type_record)
			{
			// TODO Need to use the ordering property!
			f = maybe(find_item(f->R->v_ptr,x));
			drop(x); // TODO
			return f;
			}
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	}

static struct type type_get = { 0, apply_get, clear_T };

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
			return V(&type_record,new_pair,chain(new_tail,old));
			}
		else if (cmp == 0)
			{
			value new_pair = hold(new->L);
			value new_tail = hold(new->R);
			value old_tail = hold(old->R);
			drop(old);
			drop(new);
			return V(&type_record,new_pair,chain(new_tail,old_tail));
			}
		else
			{
			value old_pair = hold(old->L);
			value old_tail = hold(old->R);
			drop(old);
			return V(&type_record,old_pair,chain(new,old_tail));
			}
		}
	}

static value apply_chain(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_record);
	else
		{
		x = eval(x);
		if (x->T == &type_record)
			return chain(hold(f->R),x);
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	}

static struct type type_chain = { 0, apply_chain, clear_T };

void use_record(void)
	{
	define("empty", Q(&type_record,0));
	define("set", Q(&type_set,0));
	define("get", Q(&type_get,0));
	define("::", Q(&type_chain,0));
	}
