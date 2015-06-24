/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	This is the main program for the stand-alone version of the
	peephole optimizer.
*/
#include	<stdarg.h>
#include	"nopt.h"

char *filename;			/* Name of input file */
int errors;			/* Number of errors */

int main(int argc, char *argv[])
{
	static struct e_instr buff;
	p_instr p = &buff;

	if (argc >= 2) {
		filename = argv[1];
	}
	else	filename = 0;
	if (!EM_open(filename)) {
		fatal(EM_error);
	}
	EM_getinstr(p);
	O_init((arith) EM_wordsize, (arith) EM_pointersize);
	if (argc >= 3) {
		if (!O_open(argv[2])) {
			fatal("O_open failed");
		}
	}
	else	if (!O_open( (char *) 0)) fatal("O_open failed");
	O_magic();
	C_out(p);

	for(;;) {
		EM_getinstr(p=GETNXTPATT());
		switch(p->em_type) {
		case EM_DEFILB:
			p->em_opcode=op_lab;
			break;
		case EM_MNEM:
			switch(p->em_argtype) {
			case sof_ptyp:
				p->em_dnam = OO_freestr(p->em_dnam);
				break;
			case pro_ptyp:
				p->em_pnam = OO_freestr(p->em_pnam);
				break;
			case str_ptyp:
			case ico_ptyp:
			case uco_ptyp:
			case fco_ptyp:
				p->em_string = OO_freestr(p->em_string);
				break;
			}
			break;
		case EM_PSEU:
			switch(p->em_opcode) {
			case ps_pro:
			case ps_end:
				break;
			default:
				C_out(p);
				OO_nxtpatt--;
				continue;
			}
			break;
		default:
			C_out(p);
			OO_nxtpatt--;
			continue;
		case EM_EOF:
			goto got_eof;
		case EM_ERROR:
			error("%s", EM_error);
			continue;
		case EM_FATAL:
			fatal("%s", EM_error);
		}
		OO_dfa(p->em_opcode);
	}
 got_eof:
	O_close();
	EM_close();
	exit(errors);
}

/*VARARGS1*/
static int verror(char *s, va_list ap)
{
	fprintf(stderr,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	errors++;
	return 0;
}

int error(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	verror(s, ap);
	va_end(ap);
	return 0;
}

/*VARARGS1*/
int fatal(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	verror(s, ap);
	va_end(ap);
	exit(1);
	return 0;
}
