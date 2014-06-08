/* NOTE: On some machines, when compiling in -ansi mode, I can't simply include
stdio.h to get the definition of snprintf, so instead I do my own declaration
here. */
extern int snprintf(char *str, size_t size, const char *format, ...);
