/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LLlex.h"
#include "file_info.h"
#include "input.h"
#define INP_TYPE	struct file_info
#define INP_VAR		finfo
struct file_info	finfo;
#include <inp_pkg.body>
#include <alloc.h>

char *getwdir(char *fn)
{
	char *p;

	p = strrchr(fn, '/');
	while (p && *(p + 1) == '\0') {	/* remove trailing /'s */
		*p = '\0';
		p = strrchr(fn, '/');
	}

	if (fn[0] == '\0' || (fn[0] == '/' && p == &fn[0])) /* absolute path */
		return "";
	else
	if (p) {
		*p = '\0';
		fn = Salloc(fn, p - &fn[0] + 1);
		*p = '/';
		return fn;
	}
	else return "";
}

int	NoUnstack;
int	Unstacked;
int	InputLevel;

int AtEoIT()
{
	/* if (NoUnstack) warning("unexpected EOF"); ??? */
	/* This is wrong; in an #elif, NoUnstack can be set, but you
	   can still get calls to AtEoIT().
	*/
	InputLevel--;
	DoUnstack();
	return 0;
}

int AtEoIF()
{
	extern int nestlevel;
	extern int nestcount;
	extern int svnestlevel[];

	if (nestlevel > svnestlevel[nestcount]) warning("missing #endif");
	else if (NoUnstack) warning("unexpected EOF");
	nestlevel = svnestlevel[nestcount--];
	return 0;
}
