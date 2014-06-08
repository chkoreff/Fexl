/* NOTE: On some machines, when compiling in -ansi mode, I can't simply include
math.h to get certain built-in functions, because they're declared somewhere
else as returning type int.  So instead I do my own declarations. */
extern double round(double);
extern double trunc(double);
extern double fabs(double);
extern double pow(double,double);
