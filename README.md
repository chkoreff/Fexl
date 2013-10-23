fexl - Function EXpression Language
===================================

[Fexl](http://fexl.com) is the simplest and most powerful language I've been
able to devise.  You can do low-level system programming in it, but you can
also create secure "sandboxes" to allow restricted high-level scripting.  The
interpreter is tiny, about 27K at the moment.  It is easy to call external C
routines.

The interpreter creates an initial value which represents your entire program.
It then evaluates that value, reducing it one step at a time until it finally
reaches a zero value.  Each step may possibly create side effects -- after all,
the entire purpose of a computer program is to create side effects.

A *value* is either an atom or a pair.  An *atom* represents either a built-in
function (with a pointer to a C function), or a piece of data (e.g. long,
double, string, file, etc.)  A *pair* represents the functional application of
a value on the left side to a value on the right side.

The initial value is fully resolved and contains no symbols, so the interpreter
does not have to do any symbol lookups or bindings during evaluation.  The use
of combinator functions makes this possible.

This gives you all the low and high level programming features you can imagine,
everything from bashing bits on the bare metal, all the way up to the cool
stratosphere of closures, high-order functions, and lazy evaluation.  All this,
with a very small language grammar.  My goal is to create a language with the
highest conceivable level of simplicity, power, efficiency, flexibility,
reliability, and security.

Installation
------------

First get the code:

	git clone git@github.com:chkoreff/Fexl.git
	cd Fexl

There you will see a script called "build".  You can run `./build` by itself
to see all the options, but here's a quick summary:

To install in the standard /usr directory:

	sudo ./build install

To install it somewhere else:

	./build install in ~/myfexl

You can use "uninstall" the same way to remove an installation.

To build it in the current directory without installing anywhere else:

	./build here

Fexl honours the standard installation structure (e.g. the way perl does it).
So if you install in /usr, it will create these:

	/usr/bin/fexl
	/usr/lib/fexl/*
	/usr/share/fexl/*

Testing
-------

First, source the "alias" file into your shell:

	. alias

To run "test/a1.fxl" and see the output:

	.run a1

To run "test/a1.fxl" and compare with the reference output "test/a1.out":

	.check a1

If the test passes, you should see no output, unless the test deliberately
prints something to stderr.

To do the check but also report the execution time:

	.time a1

Guide to the Source Code
------------------------

See the "src" directory for the .c and .h source files, and a very simple
config file which guides the build script.

The `main` routine is in fexl.c.

The `eval` routine is in value.c.

The `value` structure is defined in value.h.

The memory.c file implements a disciplined approach to memory usage which
verifies that there are no memory leaks during execution.  Also see `hold`
and `drop` in value.c, which implement the reference counting mechanism.

In the top directory you'll see an "alias" file with handy shell functions I
use for development and testing.

The "share" directory contains library functions written in Fexl itself.

The "test" directory contains detailed Fexl regression tests.

Examples
--------

The "hello world" program is:

	say "Hello world."

I'll have more later.

Author
------

My name is Patrick Chkoreff, and I started programming as a youngster in 1974.
In 1989 I became interested in functional programming languages after reading a
paper by Jørgen Steensgaard-Madsen, and also a 1924 paper by Moses Schönfinkel
introducing the concept of combinators.  In 1992 I sketched out a grammar for
Fexl.  Over the years I refined and simplified the concepts and implementation,
and now everything has clicked together in a satisfying way.

License
-------

Fexl is distributed under the Apache license.  See the NOTICE file for details.
