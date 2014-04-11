fexl - Function EXpression Language
===================================

Getting started
---------------

Go into the source directory:

	cd src

To run the main test program:

	test/run

To compare the test output against the reference output:

	test/check

To build the code without doing anything else:

	./build

To erase the build output files:

	./clean

To build quietly:

	verbose=0 ./build

To build loudly:

	verbose=2 ./build

To force a clean build:

	./clean && ./build

To source the handy shell aliases that I use for development:

	. handy

Guide to source code
--------------------

The `eval` routine is in value.c.

The `value` structure is defined in value.h, and documented in value.c.

The memory.c file implements a disciplined approach to memory usage which
verifies that there are no memory leaks during execution.  Also see `hold`
and `drop` in value.c, which implement the reference counting mechanism.

The test/run.c file is a test program for the basic Fexl code.

Credits and Licenses
--------------------
This code is copyright 2011 by Patrick Chkoreff.  See the LICENSE file.
