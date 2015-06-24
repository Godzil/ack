/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <string.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include "glosym.h"
#include "salloc.h"
#include "glosym.h"

glosym_p glolist= (glosym_p) 0;

void enterglo(string name, word *romp)
{
	glosym_p gp;
	int i;

	gp = (glosym_p) myalloc(sizeof *gp);
	gp->gl_next = glolist;
	gp->gl_name = (string) myalloc(strlen(name)+1);
	strcpy(gp->gl_name,name);
	for (i=0;i<=MAXROM;i++)
		gp->gl_rom[i] = romp[i];
	glolist = gp;
}

glosym_p lookglo(string name)
{
	glosym_p gp;

	for (gp=glolist;gp != (glosym_p) 0; gp=gp->gl_next)
		if (strcmp(gp->gl_name,name)==0)
			return(gp);
	return((glosym_p) 0);
}
