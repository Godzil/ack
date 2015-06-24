/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <out.h>
#include "const.h"



static short	nerrors = 0;
static void	diag(char *, char *, va_list);

void stop()
{
	extern char	*outputname;
	extern int	exitstatus;

	if (nerrors) {
		unlink(outputname);
		exit(nerrors);
	}

	exit(exitstatus);
}

/* VARARGS1 */
void fatal(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	nerrors++;
	diag("fatal", format, ap);
	va_end(ap);
	stop();
}

/* VARARGS1 */
void warning(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	diag("warning", format, ap);
	va_end(ap);
}

/* VARARGS1 */
void error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	nerrors++;
	diag("error", format, ap);
	va_end(ap);
}

/* VARARGS1 */
int do_verbose(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	diag((char *) 0, format, ap);
	va_end(ap);
	return (0==0);
}

static void diag(char *tail, char *format, va_list ap)
{
	extern char	*progname, *archname, *modulname; 

	fprintf(stderr, "%s: ", progname);
	if (archname && modulname)
		fprintf(stderr, "%s(%.14s): ", archname, modulname);
	else if (archname)
		fprintf(stderr, "%s: ", archname);
	else if (modulname)
		fprintf(stderr, "%s: ", modulname);
	vfprintf(stderr, format, ap);
	if (tail) fprintf(stderr, " (%s)\n", tail);
	else putc('\n', stderr);
}
