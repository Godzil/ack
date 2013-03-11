/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#if __STDC__
#include	<stdarg.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#if __STDC__
void error(char *fmt, ...);
#else
void error();
#endif

int nerrors=0;

void yyerror(char *s)
{
	error("Parser gives %s",s);
}

void goodbye()
{

	error("This was fatal, goodbye!");
#ifndef NDEBUG
	abort();
#endif
}

#if __STDC__
/*VARARGS1*/
void error(char *fmt, ...)
{
	extern int lineno;
	extern char *filename;
	va_list ap;
	fprintf(stderr,"\"%s\", line %d:",filename,lineno);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	nerrors++;
}
/*VARARGS1*/
void fatal(char *fmt, ...)
{
	extern int lineno;
	extern char *filename;
	
	va_list ap;
	fprintf(stderr,"\"%s\", line %d:",filename,lineno);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	nerrors++;

	errorexit();
	goodbye();
	exit(-1);
}
#else /* __STDC__ */
/*VARARGS1*/
fatal(s,a,b,c,d) char *s; {

	error(s,a,b,c,d);
	errorexit();
	goodbye();
	exit(-1);
}

/*VARARGS1*/
error(s,a,b,c,d) char *s; {
	extern int lineno;
	extern char *filename;

	fprintf(stderr,"\"%s\", line %d:",filename,lineno);
	fprintf(stderr,s,a,b,c,d);
	fprintf(stderr,"\n");
	nerrors++;
}
#endif

#ifndef NDEBUG
void badassertion(char *string, char *file, int line)
{
	fprintf(stderr,"\"%s\", line %d: Assertion failed \"%s\"\n",
		file,line,string);
	goodbye();
}
#endif

void tabovf(char *string)
{
	fatal("%s overflow", string);
}
