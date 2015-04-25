#include <value.h>
#include <basic.h>
#include <num.h>
#include <stdio.h>
#include <str.h>
#include <type_file.h>
#include <type_num.h>
#include <type_str.h>

value type_file(value f)
	{
	if (!f->L) return 0;
	replace_void(f);
	return 0;
	}

static void file_free(FILE *fh)
	{
	if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
		fclose(fh);
	}

value Qfile(FILE *fh)
	{
	return D(type_file,fh,(type)file_free);
	}

/* (fopen path mode) Open a file and return {fh}, or void if not possible. */
value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	if (x->T == type_str && y->T == type_str)
		{
		string path = data(x);
		string mode = data(y);
		FILE *fh = fopen(path->data,mode->data);
		if (fh)
			f = A(Q(type_single),Qfile(fh));
		else
			f = Q(type_void);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}

/* (fgetc fh) = {ch}, where ch is the next byte from fh, or void if no more
characters. */
value type_fgetc(value f)
	{
	if (!f->L) return 0;
	value x = eval(hold(f->R));
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		int ch = fgetc(fh);
		string s = 0;
		if (ch >= 0)
			{
			char c = ch;
			s = str_new_data(&c,1);
			}
		f = A(Q(type_single), s ? Qstr(s) : Q(type_void));
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}

/* (fwrite fh str) Write string to file. */
value type_fwrite(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	if (x->T == type_file && y->T == type_str)
		{
		FILE *fh = data(x);
		string s = data(y);
		size_t count = fwrite(s->data, 1, s->len, fh);
		(void)count; /* ignore */
		f = hold(I);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}

/* (remove path) = {code} Remove path from file system.  The code is 0 if
successful or -1 otherwise. */
value type_remove(value f)
	{
	if (!f->L) return 0;
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		string path = data(x);
		int code = remove(path->data);
		f = A(Q(type_single), Qnum(num_new_double(code)));
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}

/* LATER strerror */
/* LATER fdopen fflush */
/* LATER pipe dup2 close fork wait */
/* LATER limit_time limit_stack limit_memory */
