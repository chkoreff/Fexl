#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "abstract.h"
#include "apply.h"
#include "I.h"
#include "parse.h"
#include "resolve.h"
#include "string.h"
#include "var.h"
#include "Y.h"
#include "Q.h"

/*
Grammar:

exp  => [empty]
exp  => term exp
exp  => \ sym exp
exp  => \ sym = term exp
exp  => \ sym == term exp
exp  => ; exp

term => sym
term => ( exp )

The Fexl parser reads an expression from the input until it reaches EOF (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file, allowing the Fexl function to read any remaining
input after the \\ token.
*/

static FILE *curr_source = 0;
static int curr_ch = 0;
int line_no = 1;

static void next_char(void)
	{
	curr_ch = fgetc(curr_source);
	if (curr_ch == '\n') line_no++;
	}

void skip_space(void)
	{
	while (1)
		{
		if (curr_ch == EOF)
			break;
		else if (curr_ch == '#')
			{
			while (curr_ch != '\n' && curr_ch != EOF)
				next_char();
			}
		else if (isspace(curr_ch))
			{
			while (isspace(curr_ch))
				next_char();
			}
		else
			break;
		}
	}

static int buf_max = 0;
static int buf_len = 0;
static char *buf = 0;

void buf_clear(void)
	{
	if (buf) free_memory(buf,buf_max);
	buf_max = 0;
	buf_len = 0;
	buf = 0;
	}

void buf_add(int ch)
	{
	if (buf_len >= buf_max)
		{
		/* Start at 10 bytes and raise it 20% each time. */
		int new_max = buf_max == 0 ? 10 : buf_max + buf_max / 5;
		if (new_max <= buf_len) new_max = buf_len + 10; /* just in case */

		char *new_buf = new_memory(new_max);
		if (buf)
			{
			memcpy(new_buf, buf, buf_len);
			free_memory(buf, buf_max);
			}
		buf = new_buf;
		buf_max = new_max;
		}

	buf[buf_len++] = ch;
	}

/*
Parse a symbol.

IDENTIFIERS

If we're starting with any character other than a quote mark (") or tilde (~),
then collect the characters up to the next white space character.  White space
includes space, tab, newline, and anything else which the isspace function
considers as white space.  Examples:

  a
  x
  abc
  a:b
  a#b

Note that you can use *any* characters (except white space) in a symbol like
this, including so-called "binary" characters, even including NUL.  I suppose
this will come in handy with Unicode characters, though I am only testing the
system with plain ASCII.

SIMPLE STRINGS

If we're starting with a quote mark ("), then collect the characters up to and
including the closing quote mark.  Examples:

  "hello"
  "a"
  ""

COMPLEX STRINGS

If we're starting with a tilde (~), then skip the tilde and then collect the
characters up to the next white space character.  Those characters will serve
as the terminator of the subsequent string.  So we skip that white space
character, and then keep collecting characters until we see the terminator that
we read at the beginning.  Examples:

  ~@ This is a "complex" string.@
  ~"" This too is a "complex" string.""
  ~@1 Here is a "complex" string with an @ sign inside it.@1
  ~END Note that we can use anything as a terminator.END
*/

struct value *parse_sym(void)
	{
	buf_clear();

	if (curr_ch == EOF)
		{
		fprintf(stderr, "Expected a symbol but reached end of file\n");
		exit(1);
		}

	if (curr_ch == '"')
		{
		while (1)
			{
			buf_add(curr_ch);
			next_char();
			if (curr_ch == EOF)
				{
				fprintf(stderr, "Missing closing quote on string.\n");
				exit(1);
				}
			if (curr_ch == '"')
				{
				buf_add(curr_ch);
				next_char();
				break;
				}
			}
		}
	else if (curr_ch == '~')
		{
		while (1)
			{
			buf_add(curr_ch);
			next_char();
			if (curr_ch == EOF)
				{
				fprintf(stderr,
				"Opening terminator on string did not end with white space.\n");
				exit(1);
				}

			if (isspace(curr_ch))
				{
				buf_add(curr_ch);
				break;
				}
			}

		int term_len = buf_len - 2;

		if (term_len <= 0)
			{
			fprintf(stderr, "Missing opening terminator on string, line %d.\n",
				line_no);
			exit(1);
			}

		while (1)
			{
			next_char();
			if (curr_ch == EOF)
				{
				fprintf(stderr, "Missing closing terminator on string.\n");
				exit(1);
				}

			buf_add(curr_ch);

			if (memcmp(buf + buf_len - term_len, buf + 1, term_len) == 0)
				{
				next_char();
				break;
				}
			}
		}
	else
		{
		while (1)
			{
			if (curr_ch == EOF || isspace(curr_ch)
				|| curr_ch == '\\'
				|| curr_ch == '('
				|| curr_ch == ')'
				|| curr_ch == ';'
				|| curr_ch == '='
				|| curr_ch == '"'
				|| curr_ch == '~'
				)
				break;

			buf_add(curr_ch);
			next_char();
			}
		}

	if (buf_len == 0)
		{
		fprintf(stderr, "Missing symbol on line %d\n", line_no);
		exit(1);
		}

	struct value *sym = new_chars(buf,buf_len);
	sym->T = &type_var;
	buf_clear();
	return sym;
	}

/*
LATER also once we start calling this parser from within fexl itself for
embedded environments, we'll increment total_steps and compare with max_steps
as we go.
*/

int paren_count = 0;

extern struct value *parse_exp(struct value *exp);

struct value *parse_term(void)
	{
	struct value *term;

	if (curr_ch == '(')
		{
		paren_count++;
		next_char();
		term = parse_exp(0);
		}
	else
		{
		struct value *sym = parse_sym();
		term = resolve(sym);
		drop(sym);
		}

	return term;
	}

/*
Parse an expression.  If an exp value is given, apply that to the left side of
the parsed expression.  This gives us the left-associative chaining that we
need.

Note that we check max_depth here to avoid a stack overflow and segmentation
fault which could happen from strange inputs like an infinite series of left
parentheses.
*/

struct value *parse_exp(struct value *exp)
	{
	if (++total_depth > max_depth)
		{
		fprintf(stderr,
			"Your program tried to evaluate at a depth greater than %d.\n",
			max_depth);
		exit(1);
		}

	while (1)
		{
		skip_space();

		if (curr_ch == EOF)
			break;
		else if (curr_ch == '\\')
			{
			next_char();

			if (curr_ch == '\\')
				{
				/* Stop reading the input. */
				curr_ch = EOF;
				break;
				}

			skip_space();

			struct value *sym = parse_sym();

			skip_space();

			if (curr_ch == '=')
				{
				next_char();

				int is_recursive = 0;
				if (curr_ch == '=')
					{
					is_recursive = 1;
					next_char();
					}

				skip_space();

				if (is_recursive) push_var(sym, sym);
				struct value *term = parse_term();
				hold(term);
				if (!is_recursive) push_var(sym, sym);

				struct value *def;
				if (is_recursive)
					{
					def = apply(&value_Y,abstract(sym,term));
					optimize(def);
					}
				else
					def = term;

				struct value *body = parse_exp(0);
				hold(body);

				struct value *value = abstract(sym,body);

				struct value *result;
				if (is_recursive)
					result = apply(value,def);
				else
					/* The Q forces eager evaluation of def first. */
					result = apply(apply(&value_Q,def),value);

				exp = exp ? apply(exp,result) : result;

				drop(term);
				drop(body);
				}
			else
				{
				push_var(sym, sym);
				struct value *body = parse_exp(0);
				hold(body);

				struct value *value = abstract(sym,body);
				exp = exp ? apply(exp,value) : value;

				drop(body);
				}

			pop_var();
			break;
			}
		else if (curr_ch == ')')
			{
			paren_count--;
			if (paren_count < 0)
				{
				fprintf(stderr,
					"Extra right parentheses on line %d\n", line_no);
				exit(1);
				}
			next_char();
			break;
			}
		else if (curr_ch == ';')
			{
			next_char();
			struct value *value = parse_exp(0);
			exp = exp ? apply(exp,value) : value;
			break;
			}
		else
			{
			struct value *term = parse_term();
			exp = exp ? apply(exp,term) : term;
			}
		}

	total_depth--;
	return exp ? exp : &value_I;
	}

struct value *parse(FILE *source)
	{
	start_resolve();

	curr_source = source;
	next_char();

	struct value *value = parse_exp(0);

	if (paren_count > 0)
		{
		fprintf(stderr, "Missing right parentheses\n");
		exit(1);
		}

	finish_resolve();

	return value;
	}
