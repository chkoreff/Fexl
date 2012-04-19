/*
This is the base path of the Fexl executable program.  Its actual value is not
specified anywhere in the src directory, but is created by special logic in
the build script, depending on whether we're doing a local or install build.
*/
extern char *base;
