/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Manoel Trapier
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include "data.h"

#include "gencode.h"

#include "utils.h"

#ifdef TABLEDEBUG
static void ruletrace();
#endif

/*VARARGS1*/
void error(char *s, ...)
{
	va_list ap;
	fprintf(stderr,"Error: ");
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr,"\n");
#ifdef TABLEDEBUG
	ruletrace();
#endif
	out_finish();
	exit(-1);
}

/*VARARGS1*/
void fatal(char *s, ...)
{
	va_list ap;
	fprintf(stderr,"Fatal: ");
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr,"\n");
#ifdef TABLEDEBUG
	ruletrace();
#endif
	out_finish();
	abort();
	exit(-1);
}

#ifdef TABLEDEBUG
static void ruletrace()
{
	int i;
	extern int tablelines[MAXTDBUG];
	extern int ntableline;
	extern char *tablename;

	fprintf(stderr,"Last code rules used\n");
	i=ntableline-1;
	while(i!=ntableline) {
		if (i<0)
			i += MAXTDBUG;
		if (tablelines[i]!=0)
			fprintf(stderr,"\"%d: \"%s\", line %d\n",i,tablename,tablelines[i]);
		i--;
	}
}
#endif

#ifndef NDEBUG
void badassertion(char *asstr, char *file, int line)
{
	fatal("\"%s\", line %d:Assertion \"%s\" failed", file, line, asstr);
}
#endif
