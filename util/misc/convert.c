/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#if __STDC__
#include	<stdarg.h>
#endif

/*	This program converts either human-readable or compact EM
	assembly code to calls of the procedure-interface.
	It must be linked with two libraries:
	- a library to read EM code, according to read_em(3)
	- a library implementing the em_code(3) interface.
	Thus, this program could serve as an EM_encoder, an
	EM_decoder, or some code generator, depending on how it is
	linked.
*/

#include <stdio.h>
#include <stdlib.h>
#include "system.h"
#include <em_arith.h>
#include <em_label.h>
#include "em_pseu.h"
#include "em_mnem.h"
#include "em_spec.h"
#include "em_flag.h"
#include "em_ptyp.h"
#include "em_comp.h"
#include "em.h"
#include <em_code.h>

#if __STDC__
void error(char *fmt, ...);
void fatal(char *fmt, ...);
#else
#include "print.h"
void error();
void fatal();
#endif

char *filename;			/* Name of input file */
int errors;			/* Number of errors */
extern char *C_error;

int main(int argc, char *argv[])
{
	struct e_instr buf;
	struct e_instr *p = &buf;

	if (argc >= 2) {
		filename = argv[1];
	}
	else	filename = 0;
	if (!EM_open(filename)) {
		fatal(EM_error);
	}
	EM_getinstr(p);
	C_init((arith) EM_wordsize, (arith) EM_pointersize);
	if (argc >= 3) {
		if (!C_open(argv[2])) {
			fatal("C_open failed");
		}
	}
	else	if (!C_open( (char *) 0)) fatal("C_open failed");
	C_magic();
	while (p->em_type != EM_EOF) {
		if (p->em_type == EM_FATAL) {
			fatal("%s", EM_error);
		}
		if (EM_error) {
			error("%s", EM_error);
		}
		if (p->em_type != EM_ERROR && !C_out(p)) {
			error("%s", C_error);
		}
		EM_getinstr(p);
	}
	C_close();
	EM_close();
	exit(errors);
	return errors;
}


#if __STDC__
void error(char *fmt, ...)
{
	va_list ap;
	fprintf(stderr,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

void fatal(char *fmt, ...)
{
	va_list ap;

	if (C_busy()) C_close();

	fprintf(stderr,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(1);
}
#else /* __STDC__ */

/* VARARGS */
error(s,a1,a2,a3,a4)
	char *s;
{
	fprint(STDERR,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);
	fprint(STDERR,s,a1,a2,a3,a4);
	fprint(STDERR, "\n");
	errors++;
}

/* VARARGS */
fatal(s,a1,a2,a3,a4)
	char *s;
{
	if (C_busy()) C_close();
	error(s,a1,a2,a3,a4);
	exit(1);
}
#endif /* __STDC__ */
