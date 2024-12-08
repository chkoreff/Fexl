# fexl - Function EXpression Language

[Fexl](http://fexl.com) is a simple but powerful scripting language.  It allows
you to manipulate numbers, strings, streams, and functions easily and reliably,
all within a secure "sandbox".

## Getting started

Go into the source code directory:
```
cd src
```

Build the code and test its validity:
```
test/check
```

That runs the test scripts, comparing their outputs with the reference files in
the out directory.  They should all succeed, however, if you run on a 32-bit
system you will see some inconsequential differences in tests which measure
memory usage or deliberately try to run out of memory.

## Installation

There is no "installation" as such.  You just put the Fexl code anywhere you
like, build it, and run it from there.  There is no script to copy it to /usr
or anything like that.

One approach is to create a ~/bin/fexl script which does this:
```
exec ~/project/fexl/bin/fexl "$@"
```

So the "fexl" script in my search path runs the real executable right where I
built it, setting argv[0] to its full path name, allowing it to resolve the
names of any files it might need that come bundled with the distribution.

## Development Tools

To build the code:
```
./build
```

To erase the output files and build the code from scratch:
```
./build clean
```

To erase the output files:
```
./build erase
```

To do a verbose build, showing all the commands:
```
verbose=1 ./build
```

To build the code and run a fexl program:
```
./fexl [FILE]
```

To build the code and check all the tests:
```
test/check
```

You can also check a specific set of tests, e.g.:
```
test/check chars b10 sat crypto
```

You can show the output of an individual test, e.g.:
```
test/show sat
```

To source the handy shell aliases that I use for development:
```
. handy
```

To see the current version number (http://semver.org):
```
cat VERSION
```

## Test sort in GNU coreutils.

```
(cd test/ls_bug; ./try)
```

## Guide to source code

The `main` routine is in fexl.c.

The `eval` routine is in value.c.

The `value` structure is defined in value.h, and documented in value.c.

The memory.c file implements a disciplined approach to memory usage which
verifies that there are no memory leaks during execution.  Also see `hold`
and `drop` in value.c, which implement the reference counting mechanism.

The "test" directory contains the Fexl test suite.

## Technical Details

The interpreter creates an initial value which represents your entire program.
It then evaluates that value, reducing it one step at a time until it reaches a
final value.  Each step may possibly create side effects -- after all, the
entire purpose of a computer program is to create side effects.  However, the
purely functional aspect of Fexl allows you to isolate those side effects as
far as you like.

A *value* is either an atom or a pair.  An *atom* represents either a built-in
function (with a pointer to a C function), or a piece of data (e.g. long,
double, string, file, etc.)  A *pair* represents the functional application of
a value on the left side to a value on the right side.

The initial value is fully resolved and contains no symbols, so the interpreter
does not have to do any symbol lookups or bindings during evaluation.

## Credits

"On the Building Blocks of Mathematical Logic", Moses Schönfinkel, 1924

"The Calculi of Lambda-Conversion", Alonzo Church, 1941

"Typed Representation of Objects by Functions", Jørgen Steensgaard-Madsen, 1989

## License
This code is copyright 2023 by Patrick Chkoreff (pc@fexl.com).
See the LICENSE file for details.
