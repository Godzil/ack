/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* MAIN PROGRAM */

#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<system.h>
#include	<string.h>
#include 	"arith.h"
#include	"file_info.h"
#include	"idfsize.h"
#include	"idf.h"
#include	"macro.h"

extern char *symbol2str();
extern char *getwdir();
extern int err_occurred;
extern int do_dependencies;
extern char *dep_file;
int idfsize = IDFSIZE;
extern char options[];
static File *dep_fd = STDOUT;

arith ifval;

char *prog_name;

extern char **inctable;
extern int inc_max, inc_total;


void compile(int argc, char *argv[]);
void list_dependencies(char *source);
void dependency(char *s, char *source);


int main(int argc, char *argv[])
{
	/* parse and interpret the command line options	*/
	prog_name = argv[0];

	init_idf();

	inctable = (char **) Malloc(10 * sizeof(char *));
	inc_max = 10;
	inc_total = 3;
	inctable[0] = ".";
	inctable[1] = 0;
	init_pp();	/* initialise the preprocessor macros	*/

	/*	Note: source file "-" indicates that the source is supplied
		as standard input.  This is only allowed if INP_READ_IN_ONE is
		not defined!
	*/
	while (argc > 1 && *argv[1] == '-' && argv[1][1] != '\0')	{
		char *par = &argv[1][1];

		if (*par == '-')
			par++;
		do_option(par);
		argc--, argv++;
	}
	compile(argc - 1, &argv[1]);
	sys_stop(err_occurred ? S_EXIT : S_END);

	/*NOTREACHED*/
	return -1;
}

void compile(int argc, char *argv[])
{
	register char *source = 0;
	char *dummy;

	switch (argc) {
	case 1:
		source = argv[0];
		FileName = source;
		break;
	case 0:
		FileName = "";
		WorkingDir = "";
		break;
	default:
		FileName = argv[0];
		fatal("use: %s [options] [source]", prog_name);
		break;
	}

	if (!InsertFile(source, (char **) 0, &dummy)) /* read the source file	*/
		fatal("%s: no source file %s\n", prog_name, 
			source ? source : "stdin");
	if (source) WorkingDir = getwdir(dummy);
	preprocess(source);
	if (do_dependencies) list_dependencies(source);
}

struct idf	*file_head;

void list_dependencies(char *source)
{
	register struct idf *p = file_head;

	if (source) {
		register char *s = strrchr(source, '.');

		if (s && *(s+1)) {
			s++;
			*s++ = 'o';
			*s = '\0';
                        /* the source may be in another directory than the
                         * object generated, so don't include the pathname
                         * leading to it.
                         */
                        if ( (s = strrchr(source, '/')) ) {
                                source = s + 1;
                        }
		}
		else source = 0; 
	}
	if (dep_file && !sys_open(dep_file, OP_WRITE, &dep_fd)) {
		fatal("could not open %s", dep_file);
	}
	while (p) {
		assert(p->id_resmac == K_FILE);
		dependency(p->id_text, source);
		p = p->id_file;
	}
}

void add_dependency(char *s)
{
	register struct idf *p = str2idf(s, 0);

	if (! p->id_resmac) {
		p->id_resmac = K_FILE;
		p->id_file = file_head;
		file_head = p;
	}
}

void dependency(char *s, char *source)
{
	if (options['i'] && !strncmp(s, "/usr/include/", 13)) {
		return;
	}
	if (options['m'] && source) {
		fprint(dep_fd, "%s: %s\n", source, s);
	}
	else	fprint(dep_fd, "%s\n", s);
}

void No_Mem()				/* called by alloc package */
{
	fatal("out of memory");
}
