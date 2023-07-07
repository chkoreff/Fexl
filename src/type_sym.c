#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <string.h> // strcmp
#include <type_str.h>
#include <type_sym.h>

static unsigned long table_size(unsigned long len)
	{
	return sizeof(struct table) + sizeof(struct symbol[len]);
	}

static void table_free(struct table *table)
	{
	unsigned long i;
	// Drop each symbol in vec.
	for (i = 0; i < table->count; i++)
		{
		struct symbol *sym = &table->vec[i];
		drop(sym->name);
		drop(sym->pattern);
		}
	free_memory(table, table_size(table->len));
	}

static void clear_form(value f)
	{
	struct form *form = f->v_ptr;

	drop(form->exp);
	drop(form->label);

	if (form->table)
		table_free(form->table);

	free_memory(form,sizeof(struct form));
	}

value type_form(value f)
	{
	return type_data(f);
	}

value Qform(struct form form)
	{
	static struct value clear = {{.N=0}, {.clear=clear_form}};
	struct form *p = new_memory(sizeof(struct form));
	*p = form;
	return V(type_form,&clear,(value)p);
	}

struct form form_null(void)
	{
	return (struct form){ 0, 0, 0 };
	}

// Make a reference to a fixed value with no symbols.
struct form form_val(value exp)
	{
	return (struct form){ 0, exp, 0 };
	}

static struct table *table_ref(string name, unsigned long line)
	{
	struct table *table;
	struct symbol *sym;

	table = new_memory(table_size(1));
	table->count = 1;
	table->len = 1;

	sym = &table->vec[0];
	sym->name = Qstr(name);
	sym->pattern = hold(QT);
	sym->line = line;
	return table;
	}

// Make a reference to a symbol on a given line.
struct form form_ref(string name, unsigned long line)
	{
	struct table *table = table_ref(name,line);
	return (struct form){ table, hold(QI), 0 };
	}

// Merge the tables and combine patterns where they intersect.
static struct table *table_merge(struct table *xt, struct table *yt)
	{
	struct table *zt = 0;
	unsigned long xn = xt ? xt->count : 0;
	unsigned long yn = yt ? yt->count : 0;
	unsigned long zn = xn + yn;

	if (zn != 0)
	{
	unsigned long xi = 0;
	unsigned long yi = 0;
	unsigned long zi = 0;

	zt = new_memory(table_size(zn));
	zt->len = zn;

	while (1)
		{
		struct symbol *x = &xt->vec[xi];
		struct symbol *y = &yt->vec[yi];
		struct symbol *z = &zt->vec[zi];

		int cmp;
		if (xi < xn)
			{
			if (yi < yn)
				cmp = strcmp(str_data(x->name),str_data(y->name));
			else
				cmp = -1;
			}
		else if (yi < yn)
			cmp = 1;
		else
			break;

		if (cmp < 0)
			{
			z->name = hold(x->name);
			z->pattern = A(hold(x->pattern),hold(QF));
			z->line = x->line;
			xi++;
			}
		else if (cmp == 0)
			{
			z->name = hold(x->name);
			z->pattern = A(hold(x->pattern),hold(y->pattern));
			z->line = x->line;
			xi++;
			yi++;
			}
		else
			{
			z->name = hold(y->name);
			z->pattern = A(hold(QF),hold(y->pattern));
			z->line = y->line;
			yi++;
			}

		zi++;
		}

	zt->count = zi;
	}

	if (xt) table_free(xt);
	if (yt) table_free(yt);

	return zt;
	}

// Make an applicative form with the given type.
struct form form_join(type t, struct form fun, struct form arg)
	{
	return (struct form)
		{
		table_merge(fun.table,arg.table),
		V(t,fun.exp,arg.exp),
		0
		};
	}

// Apply function to argument, keeping the type of the function.
struct form form_appv(struct form fun, struct form arg)
	{
	return form_join(fun.exp->T,fun,arg);
	}

// Apply function to argument.
struct form form_app(struct form fun, struct form arg)
	{
	return form_join(type_A,fun,arg);
	}

// Delete the symbol with the given name and return the associated pattern.
static value table_pop(string name, struct table *xt)
	{
	value pattern = 0;
	if (xt != 0)
		{
		unsigned long xi = 0;
		unsigned long yi = 0;

		while (xi < xt->count)
			{
			struct symbol *x = &xt->vec[xi];
			int cmp = pattern ? 1 : strcmp(name->data,str_data(x->name));

			if (cmp == 0)
				{
				pattern = x->pattern;
				drop(x->name);
				}
			else
				{
				struct symbol *y = &xt->vec[yi];
				y->name = x->name;
				y->pattern = A(hold(QF),x->pattern);
				y->line = x->line;
				yi++;
				}
			xi++;
			}

		xt->count = yi;
		}

	if (pattern == 0)
		pattern = hold(QF); // not found
	return pattern;
	}

value type_pattern(value f)
	{
	(void)f;
	return 0;
	}

static void clear_pattern(value f)
	{
	drop(f->R);
	}

static value wrap_pattern(value pattern)
	{
	static struct value clear = {{.N=0}, {.clear=clear_pattern}};
	return V(type_pattern,&clear,pattern);
	}

// Abstract the name from the body.
struct form form_lam(type type, string name, struct form body)
	{
	value pattern = wrap_pattern(table_pop(name,body.table));
	str_free(name);
	body.exp = V(type,pattern,body.exp);
	return body;
	}

// Use pattern p to make a copy of expression e with argument x substituted in
// the places designated by the pattern.

static value subst(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return V(e->T,subst(p->L,e->L,x),subst(p->R,e->R,x));
	}

// Direct substitution
value type_D(value f)
	{
	if (f->L->T == type_pattern) return 0;
	return subst(f->L->L->R,f->L->R,f->R);
	}

// Eager substitution
value type_E(value f)
	{
	if (f->L->T == type_pattern) return 0;
	{
	value x = arg(f->R);
	f = subst(f->L->L->R,f->L->R,x);
	drop(x);
	return f;
	}
	}

static int is_closed(struct table *xt)
	{
	return (xt == 0 || xt->count == 0);
	}

// Return true if the form has no undefined symbols.
value type_is_closed(value f)
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = exp->v_ptr;
		f = boolean(is_closed(form->table));
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}

// Define key as val in a form.
static value def(const char *key, value val, value exp)
	{
	struct form *form = exp->v_ptr;
	struct table *xt = form->table;
	value pattern = 0;

	if (xt != 0)
		{
		unsigned long xi = 0;

		while (xi < xt->count)
			{
			struct symbol *x = &xt->vec[xi];
			int cmp = strcmp(key,str_data(x->name));
			if (cmp > 0)
				xi++;
			else
				{
				if (cmp == 0)
					pattern = x->pattern;
				break;
				}
			}
		}

	if (pattern == 0)
		return hold(exp);
	else
		{
		// Copy the symbols except the one with the given pattern.
		struct table *yt;
		unsigned long yn = xt->count - 1;

		if (yn == 0)
			yt = 0;
		else
			{
			unsigned long xi = 0;
			unsigned long yi = 0;

			yt = new_memory(table_size(yn));
			yt->count = yn;
			yt->len = yn;

			while (xi < xt->count)
				{
				struct symbol *x = &xt->vec[xi];
				if (x->pattern != pattern)
					{
					struct symbol *y = &yt->vec[yi];
					y->name = hold(x->name);
					y->pattern = hold(x->pattern);
					y->line = x->line;
					yi++;
					}
				xi++;
				}
			}

		return Qform((struct form){ yt,
			subst(pattern,form->exp,val),
			hold(form->label) });
		}
	}

value type_def(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value key = arg(f->L->L->R);
	if (key->T == type_str)
		{
		value exp = arg(f->R);
		if (exp->T == type_form)
			f = def(str_data(key),f->L->R,exp);
		else
			f = hold(Qvoid);
		drop(exp);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}
	}

// Evaluate the form if all symbols are defined, otherwise report the undefined
// symbols and die.
value type_value(value f)
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = exp->v_ptr;
		struct table *xt = form->table;

		if (is_closed(xt))
			f = hold(form->exp);
		else
			{
			const char *label = str_data(form->label);
			unsigned long i;
			for (i = 0; i < xt->count; i++)
				{
				struct symbol *x = &xt->vec[i];
				undefined_symbol(str_data(x->name),x->line,label);
				}
			die(0);
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}

// Like type_value, except it yields the form value to the caller without
// evaluating it.
value type_resolve(value f)
	{
	f = type_value(f);
	if (f) f = yield(f);
	return f;
	}

const char *cur_name;

int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

value op_resolve(value f, value define(void))
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = exp->v_ptr;
		struct table *xt = form->table;

		if (is_closed(xt))
			f = hold(exp);
		else
			{
			value result = hold(form->exp);
			struct table *yt;
			unsigned long xi = 0;
			unsigned long yi = 0;

			yt = new_memory(table_size(xt->count));
			yt->len = xt->count;

			while (xi < xt->count)
				{
				struct symbol *x = &xt->vec[xi];
				value val;

				cur_name = str_data(x->name);
				val = define();

				if (val)
					{
					value exp = subst(x->pattern,result,val);
					drop(val);
					drop(result);
					result = exp;
					}
				else
					{
					struct symbol *y = &yt->vec[yi];
					y->name = hold(x->name);
					y->pattern = hold(x->pattern);
					y->line = x->line;
					yi++;
					}
				xi++;
				}

			yt->count = yi;

			if (yi == 0)
				{
				table_free(yt);
				yt = 0;
				}

			f = Qform((struct form){ yt, result, hold(form->label) });
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}
