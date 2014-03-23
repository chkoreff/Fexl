fexl - Function EXpression Language
===================================

Getting started
---------------

To run the main test program:

	(cd test/src && ./run)

To compare the test output against the reference output:

	(cd test/src && ./check)

The "base/src" directory
------------------------

This contains the essential code for any Fexl application.

The `eval` routine is in value.c.

The `value` structure is defined in value.h, and documented in value.c.

The memory.c file implements a disciplined approach to memory usage which
verifies that there are no memory leaks during execution.  Also see `hold`
and `drop` in value.c, which implement the reference counting mechanism.

The scripts here are:

	./build   # Build the code.
	./erase   # Erase the build results.

The "test/src" directory
------------------------

The run.c file is a test program for the base/src code.

The scripts here are:

	./build   # Build the code.
	./erase   # Erase the build results.
	./run     # Run the test program.
	./check   # Compare the test against the reference "out/run"

The "dist" directory
--------------------

The scripts here are:

	./build   # Create a distribution archive.
	./erase   # Erase the archive.

Development
-----------

To source the handy shell aliases that I use for development:

	. handy

I have also found certain short-cuts to be useful, for example:

	./erase    # Erase all build results.
	(cd test/src && ./run)
	(cd test/src && ./check)
	(cd base/src && ./build)
	(cd base/src && ./erase)
	(cd base/src && ./erase && ./build)
	(cd base/src && ./erase && verbose=2 ./build)

Credits and Licenses
--------------------
This code is copyright 2011 by Patrick Chkoreff.  See the LICENSE file.
