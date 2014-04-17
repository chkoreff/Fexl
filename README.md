fexl - Function EXpression Language
===================================

[Fexl](http://fexl.com) is the simplest and most powerful language I've been
able to devise.  You can do low-level system programming in it, but you can
also create secure "sandboxes" to allow restricted high-level scripting.

The interpreter creates an initial value which represents your entire program.
It then evaluates that value, reducing it one step at a time until it finally
reaches a value that is identical to the previous one.  Each step may possibly
create side effects -- after all, the entire purpose of a computer program is
to create side effects.

A *value* is either an atom or a pair.  An *atom* represents either a built-in
function (with a pointer to a C function), or a piece of data (e.g. long,
double, string, file, etc.)  A *pair* represents the functional application of
a value on the left side to a value on the right side.

The initial value is fully resolved and contains no symbols, so the interpreter
does not have to do any symbol lookups or bindings during evaluation.

This gives you all the low and high level programming features you can imagine,
everything from bashing bits on the bare metal, all the way up to the cool
stratosphere of closures, high-order functions, and lazy evaluation.  All this,
with a very small language grammar.  My goal is to create a language with the
highest conceivable level of simplicity, power, efficiency, flexibility,
reliability, and security.

Getting started
---------------

First, get the code and go into the source code directory:

	cd src

Now build the code:

	./build

Run the hello program to see a simple example:

	../bin/fexl hello.fxl

Installation
------------

There is no "installation" as such.  You just put the Fexl code anywhere you
like, build it, and run it from there.  There is no script to copy it to /usr
or anything like that.

Helpful Shortcuts
-----------------

During development I like to use scripts that automatically do a build for me.
For example, you could run the hello program like this:

	./fexl hello.fxl

The "fexl" script automatically does a quiet build before running ../bin/fexl.
That way I can change the C code and not have to think about doing the build.

To erase the build output files:

	./clean

Normally you won't have to bother with that because the build script behaves
quite intelligently, but it's good to have.

To build quietly:

	verbose=0 ./build

To build loudly:

	verbose=2 ./build

To force a clean build:

	./clean && ./build

To source the handy shell aliases that I use for development:

	. handy

Test Suite
----------

Fexl includes a very detailed test suite which includes tests for normal
behavior, syntax errors, undefined symbols, and even pathological run-time
behavior such as exceeding limits on time, memory, and stack.

To run the main test program:

	test/run

To compare the test output against the reference output:

	test/check

Guide to source code
--------------------

The `main` routine is in fexl.c.

The `eval` routine is in value.c.

The `value` structure is defined in value.h, and documented in value.c.

The memory.c file implements a disciplined approach to memory usage which
verifies that there are no memory leaks during execution.  Also see `hold`
and `drop` in value.c, which implement the reference counting mechanism.

The "test" directory contains the Fexl test suite.

License
-------
This code is copyright 2011 by Patrick Chkoreff.  See the LICENSE file.
