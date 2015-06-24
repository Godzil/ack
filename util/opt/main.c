/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "alloc.h"
#include "util.h"
#include "getline.h"
#include "cleanup.h"
#include "util.h"
#include "putline.h"
#include <em_spec.h>
#include "ext.h"

#include <missing_proto.h>

/*
 * Main program for EM optimizer
 */
void fileinit();
void flags(char *s);

int main(int argc, char *argv[])
{
	int somespace[STACKROOM];

	progname = argv[0];
	while (argc-->1 && **++argv == '-')
		flags(*argv);
	if (argc>1) {
		fprintf(stderr,"Usage: %s [-Ln] [-m<num>] [name]\n",progname);
		exit(-1);
	}
	if (argc)
		if (freopen(*argv,"r",stdin) == NULL)
			error("Cannot open %s",*argv);
	fileinit();
	coreinit((short *)somespace,(short *)(somespace+STACKROOM));
	getlines();
	cleanup();
	return(0);
}

void flags(char *s)
{
	for (s++;*s;s++)
		switch(*s) {
		case 'L':	Lflag = TRUE; break;
		case 'n':	nflag = TRUE; break;
		case 'm':	if (*(s+1) == 'l') {
					s++;
					repl_longmuls = TRUE;
				}
				repl_muls = atoi(s+1); break;
		}
}

void fileinit()
{
	if (readshort() != (short) sp_magic)
		error("wrong input file");
	if (Lflag) {
		mktemp(template);
		outfile = fopen(template, "w");
		if (outfile == NULL)
			error("can't create %s",template);
	} else {
		outfile = stdout;
		outshort(sp_magic);
	}
}
