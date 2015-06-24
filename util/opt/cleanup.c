#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <stdio.h>
#include "param.h"
#include "types.h"
#include "assert.h"
#include <em_pseu.h>
#include <em_spec.h>
#include <em_mes.h>
#include "lookup.h"
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */


void cleanup()
{
	FILE *infile;
	int c;
	sym_p *spp,sp;

 	for (spp=symhash;spp< &symhash[NSYMHASH];spp++)
 		for (sp = *spp; sp != (sym_p) 0; sp = sp->s_next)
 			if ((sp->s_flags & SYMOUT) == 0)
 				outdef(sp);
	if(!Lflag)
		return;
	c=fclose(outfile);
	assert(c != EOF);
	outfile = stdout;
	infile = fopen(template,"r");
	if (infile == NULL)
		error("temp file disappeared");
	outshort(sp_magic);
	/* Attempt to first output the word_size message */
	while ((c = getc(infile)) != sp_cend && c != EOF) {
		putc(c, outfile);
	}
	if (c == sp_cend) putc(c, outfile);
	outinst(ps_mes);
	outint(ms_ext);
	for (spp=symhash;spp< &symhash[NSYMHASH];spp++)
		for (sp = *spp; sp != (sym_p) 0; sp = sp->s_next)
			if ((sp->s_flags&(SYMDEF|SYMGLOBAL)) == (SYMDEF|SYMGLOBAL))
				outsym(sp);
	putc(sp_cend,outfile);
	while ( (c=getc(infile)) != EOF)
		putc(c,outfile);
	c=fclose(infile);
	assert(c != EOF);
	c=unlink(template);
	assert(c == 0);
}
