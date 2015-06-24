/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

/*
 * led - linkage editor for ACK assemblers output format
 */

#include <stdio.h>
#include <out.h>
#include "const.h"
#include "debug.h"
#include "arch.h"
#include "memory.h"
#include "defs.h"
#include "orig.h"

extern bool	incore;
#ifndef NOSTATISTICS
int		statistics;
#endif
#ifndef NDEBUG
int			DEB = 0;
#endif
int		Verbose = 0;

static void initializations(int argc, char *argv[]);
static void first_pass(char *argv[]);
static long number(char *s);
static void setlign(int sectno, bool lign);
static void setbase(int sectno, long base);
static struct outname *makename(char *string);
static void pass1(char *file);
static void evaluate();
static void norm_commons();
static void complete_sections();
static void change_names();
static bool tstbit(int indx, char *string);
static void second_pass(char *argv[]);
static void pass2(char *file);
#ifndef NOSTATISTICS
static void do_statistics();
#endif


int main(int argc, char *argv[])
{
	initializations(argc, argv);
	first_pass(argv);
#ifndef NOSTATISTICS
	if (statistics) do_statistics();
#endif
	freeze_core();
	evaluate();
	beginoutput();
	second_pass(argv);
	endoutput();
	stop();

	return 0;
}

#ifndef NOSTATISTICS
static void do_statistics()
{
	struct memory *m = mems;

	while (m <= &mems[NMEMS-1]) {
		fprintf(stderr, "mem %d: full %lx, free %lx\n",
				(int)(m - mems),
				(long) m->mem_full,
				(long) m->mem_left);
		m++;
	}
}
#endif

char		*progname;	/* Name this program was invoked with. */
int		passnumber;	/* Pass we are in. */
struct outhead	outhead;	/* Header of final output file. */
struct outsect	outsect[MAXSECT];/* Its section table. */

/* ARGSUSED */
static void initializations(int argc, char *argv[])
{
	/*
	 * Avoid malloc()s.
	 */
	setbuf(stdin, (char *)NULL);
	setbuf(stdout, (char *)NULL);
	setbuf(stderr, (char *)NULL);

	progname = argv[0];
	passnumber = FIRST;
	init_core();
	init_symboltable();
	outhead.oh_magic = O_MAGIC;
	outhead.oh_stamp = O_STAMP;
}

/* ------------------------ ROUTINES OF FIRST PASS ------------------------- */

int	flagword = 0;		/* To store command-line options. */
char	*outputname = "a.out";	/* Name of the resulting object file. */
int	exitstatus = 0;

/*
 * Scan the arguments.
 * If the argument starts with a '-', it's a flag, else it is either
 * a plain file to be loaded, or an archive.
 */
static void first_pass(char *argv[])
{
	char		*argp;
	int			sectno;
	int			h;
	extern int		atoi();
	extern char		*strchr();
	extern int		hash();
	extern struct outname	*searchname();

	while (*++argv) {
		argp = *argv;
		if (*argp != '-') {
			pass1(argp);
			continue;
		}
		/* It's a flag. */
		switch (*++argp) {
		case 'a':
			/*
			 * The rest of the argument must be of the form
			 * `<section number>:<alignment>', where
			 * <section number> and <alignment> are numbers.
			 * <alignment> will be the alignment in the machine of
			 * section <section number>.
			 */
			sectno = atoi(++argp);
			if ((argp = strchr(argp, ':')) == (char *)0)
				fatal("usage: -a<section number>:<alignment>");
			setlign(sectno, number(++argp));
			break;
		case 'b':
			/*
			 * The rest of the argument must be of the form
			 * `<section number>:<base>', where <section number>
			 * and base are decimal numbers. <base> will be
			 * the base address in the machine of section
			 * <section number>.
			 */
			sectno = atoi(++argp);
			if ((argp = strchr(argp, ':')) == (char *)0)
				fatal("usage: -b<section number>:<base>");
			setbase(sectno, number(++argp));
			break;
		case 'c':
			/*
			 * Leave relocation information in the output, so that
			 * a next pass can see where relocation was done. The 
			 * resulting output however is no longer relocatable.
			 */
			flagword &= ~RFLAG;
			flagword |= CFLAG;
			break;
#ifndef NDEBUG
		case 'd':
			DEB = 1;
			break;
#endif
		case 'n':
			/* In the resulting name list, leave offsets with
			   respect to the beginning of the section instead
			   of absolute addresses.
			*/
			flagword |= NFLAG;
			break;

		case 'o':
			/*
			 * The `name' argument after -o is used as name
			 * of the led output file, instead of "a.out".
			 */
			if ((outputname = *++argv) == (char *)0)
				fatal("-o needs filename");
			break;
		case 'r':
			/*
			 * Generate relocation information in the output file
			 * so that it can be the subject of another led run.
			 * This flag also prevents final definitions from being
			 * given to common symbols, and suppresses the
			 * `Undefined:' diagnostic.
			 */
			if (flagword & CFLAG) break;
			if (flagword & SFLAG)
				warning("-r contradicts -s: -s ignored");
			flagword |= RFLAG;
			break;
		case 's':
			/*
			 * `Strip' the output, that is, remove the symbol table
			 * and relocation table to save space (but impair the
			 * usefullness of the debuggers). This information can
			 * also be removed by astrip(1).
			 */
			if (flagword & RFLAG)
				warning("-s contradicts -r: -s ignored");
			else
				flagword |= SFLAG;
			break;
		case 'u':
			/*
			 * Take the following argument as a symbol and enter it
			 * as undefined in the symbol table. This is useful for
			 * loading wholly from a library, since initially the
			 * symbol table is empty and an unresolved reference is
			 * needed to force the loading of the first routine.
			 */
			if (*++argv == (char *)0)
				fatal("-u needs symbol name");
			h = hash(*argv);
			if (searchname(*argv, h) == (struct outname *)0)
				entername(makename(*argv), h);
			break;
		case 'v':
			Verbose = 1;
			break;
		case 'S':
			statistics = 1;
			break;
		default:
			warning("bad flag letter %c", *argp);
			break;
		}
	}
}

/*
 * If `s' starts with 0x/0X, it's hexadecimal,
 * else if it starts with 0b/0B, it's binary,
 * else if it starts with 0, it's octal,
 * else it's decimal.
 */
static long number(char *s)
{
	int	digit;
	long	value = 0;
	int	radix = 10;

	if (*s == '0') {
		radix = 8;
		s++;
		if (*s == 'x' || *s == 'X') {
			radix = 16;
			s++;
		} else if (*s == 'b' || *s == 'B') {
			radix = 2;
			s++;
		}
	}
	while ((digit = *s++)) {
		if (digit >= 'A' && digit <= 'F')
			digit = digit - 'A' + 10;
		else if (digit >= 'a' && digit <= 'f')
			digit = digit - 'a' + 10;
		else if (digit >= '0' && digit <= '9')
			digit = digit - '0';
		else
			fatal("wrong digit %c", digit);
		if (digit >= radix)
			fatal("digit %c exceeds radix %d", digit, radix);
		value = radix * value + digit;
	}
	return value;
}

/*
 * We use one bit per section to indicate whether a base was already given or
 * not. Only one base may be given. The same applies for alignments.
 */
static char	basemap[MAXSECT / WIDTH];
static long	sect_base[MAXSECT];
static char	lignmap[MAXSECT / WIDTH];
static long	sect_lign[MAXSECT];

/*
 * Set the alignment of section `sectno' to `lign', if this doesn't
 * conflict with earlier alignment.
 */
static void setlign(int sectno, bool lign)
{
	if (setbit(sectno, lignmap) && sect_lign[sectno] != lign)
		fatal("section has different alignments");
	if (lign == (long)0)
		fatal("alignment cannot be zero");
	sect_lign[sectno] = lign;
}

/*
 * Set the base of section `sectno' to `base', if no other base has been
 * given yet.
 */
static void setbase(int sectno, long base)
{
	extern bool	setbit();

	if (setbit(sectno, basemap) && sect_base[sectno] != base)
		fatal("section has different bases");
	sect_base[sectno] = base;
}

static struct outname *makename(char *string)
{
	static struct outname	namebuf;

	namebuf.on_mptr = string;
	namebuf.on_type = S_UND + S_EXT;
	namebuf.on_valu = (long)0;

	return &namebuf;
}

/*
 * If `file' is a plain file, symboltable information and section sizes are
 * extracted. If it is an archive it is examined to see if it defines any
 * undefined symbols.
 */
static void pass1(char *file)
{
	if (getfile(file) == PLAIN) {
		debug("%s: plain file\n", file);
		extract();
	} else {
		/* It must be an archive. */
		debug("%s: archive\n", file);
		arch();
	}
	closefile(file);
}

/* ---------------- ROUTINES BETWEEN FIRST AND SECOND PASS ----------------- */

/*
 * After pass 1 we know the sizes of all commons so we can give each common
 * name an address within its section and we can compute the sizes of all
 * sections in the machine. After this we can compute the bases of all
 * sections. We then add the section bases to the values of names in
 * corresponding sections.
 */
static void evaluate()
{
	norm_commons();
	complete_sections();
	if (!(flagword&(RFLAG|NFLAG)))
		change_names();
}

extern unsigned short	NGlobals, NLocals;

/*
 * Sect_comm[N] is the number of common bytes in section N.
 * It is computed after pass 1.
 */
long	sect_comm[MAXSECT];

/*
 * If there are undefined names, we print them and we set a flag so that
 * the output can be subject to another led run and we return.
 * We now know how much space each common name needs. We change the value
 * of the common name from the size to the address within its section,
 * just like "normal" names. We also count the total size of common names
 * within each section to be able to compute the final size in the machine.
 */
static void norm_commons()
{
	struct outname	*name;
	int		cnt;
	int		und = FALSE;

	name = (struct outname *)address(ALLOGLOB, (ind_t)0);
	cnt = NGlobals;
	while (cnt-- > 0) {
		if (ISUNDEFINED(name)) {
			if (!und) {
				und = TRUE;
				if (!(flagword & RFLAG)) {
					exitstatus = 1;
					fprintf(stderr, "Undefined:\n");
				}
				outhead.oh_flags |= HF_LINK;
				if (flagword & RFLAG) break;
				flagword = (flagword & ~SFLAG) | RFLAG;
			}
			fprintf(stderr, "\t%s\n",
				address(ALLOGCHR, (ind_t)name->on_foff)
			);
		}
		name++;
	}
	if (flagword & RFLAG) return;

	/*
	 * RFLAG is off, so we need not produce relocatable output.
	 * We can now assign an address to common names.
	 * It also means that there are no undefined names.
	 */
	name = (struct outname *)address(ALLOGLOB, (ind_t)0);
	cnt = NGlobals;
	while (cnt-- > 0) {
		if (!ISABSOLUTE(name) && ISCOMMON(name)) {
			long	size;
			int	sectindex;

			size = name->on_valu;	/* XXX rounding? */
			sectindex = (name->on_type & S_TYP) - S_MIN;
			name->on_valu =
				outsect[sectindex].os_size +
				sect_comm[sectindex];
			sect_comm[sectindex] += size;
			name->on_type &= ~S_COM;
		}
		name++;
	}
}

struct orig	relorig[MAXSECT];

/*
 * Compute the offsets in file and machine that the sections will have.
 * Also set the origins to 0.
 */
static void complete_sections()
{
	long	base = 0;
	long	foff;
	struct outsect *sc;
	int	sectindex;

	foff = SZ_HEAD + outhead.oh_nsect * SZ_SECT;
	for (sectindex = 0; sectindex < outhead.oh_nsect; sectindex++) {
		relorig[sectindex].org_size = (long)0;
		sc = &outsect[sectindex];
		sc->os_foff = foff;
		foff += sc->os_flen;

		if (flagword & RFLAG)
			continue;

		sc->os_size += sect_comm[sectindex];
		sc->os_lign =
			tstbit(sectindex, lignmap) ? sect_lign[sectindex] : 1;
		if (tstbit(sectindex, basemap)) {
			base = sect_base[sectindex];
			if (sc->os_lign && base % sc->os_lign)
				fatal("base not aligned");
		} else if (sc->os_lign) {
			base += sc->os_lign - 1;
			base -= base % sc->os_lign;
		}
		sc->os_base = base;
		base += sc->os_size;
	}
}

/*
 * For each name we add the base of its section to its value, unless
 * the output has to be able to be linked again, as indicated by RFLAG.
 */
static void change_names()
{
	int		cnt;
	struct outname	*name;

	name = (struct outname *)address(ALLOGLOB, (ind_t)0);
	cnt = NGlobals;
	while (cnt-- > 0) {
		addbase(name);
		name++;
	}
	if (!incore)
		return;
	/*
	 * Do the same with the local names.
	 */
	name = (struct outname *)address(ALLOLOCL, (ind_t)0);
	cnt = NLocals;
	while (cnt-- > 0) {
		addbase(name);
		name++;
	}
}

#define BIT	0x01

/*
 * This function sets a bit with index `indx' in string.
 * It returns whether it was already set.
 */
bool setbit(int indx, char *string)
{
	register int	byte_index, bit_index;
	register int	byte;

	byte_index = indx / WIDTH;	/* Index of byte with bit we need. */
	bit_index = indx % WIDTH;	/* Index of bit we need. */
	byte = string[byte_index];
	byte >>= bit_index;
	if (byte & BIT)	return TRUE;

	byte = BIT;
	byte <<= bit_index;
	string[byte_index] |= byte;
	return FALSE;
}

/*
 * This function returns whether the bit given by `indx' is set in `string'.
 */
static bool tstbit(int indx, char *string)
{
	register int	byte_index, bit_index;
	register int	byte;

	byte_index = indx / WIDTH;	/* Index of byte with bit we need. */
	bit_index = indx % WIDTH;	/* Index of bit we need. */
	byte = string[byte_index];
	byte >>= bit_index;

	return byte & BIT;
}

/*
 * Add the base of the section of a name to its value.
 */
void addbase(struct outname *name)
{
	register int	type = name->on_type & S_TYP;
	register int	sectindex = type - S_MIN;

	if (type == S_UND || type == S_ABS || type == S_CRS)
		return;
	if (name->on_type & S_COM)
		return;

	name->on_valu += outsect[sectindex].os_base;
	debug(	"%s: type 0x%x, value %ld\n",
		address((name->on_type & S_EXT) ? ALLOGCHR : ALLOLCHR,
			(ind_t)name->on_foff
		),
		name->on_type, name->on_valu
	);
}

/* ------------------------ ROUTINES OF SECOND PASS ------------------------ */

/*
 * Flags have already been processed, so we ignore them here.
 */
static void second_pass(char *argv[])
{
	passnumber = SECOND;
	while (*++argv) {
		if ((*argv)[0] != '-') {
			pass2(*argv);
			continue;
		}
		switch ((*argv)[1]) {
		case 'o':
		case 'u':
			++argv;
			break;
		default:
			break;
		}
	}
}

static void pass2(char *file)
{
	if (getfile(file) == PLAIN) {
		debug("%s: plain file\n", file);
		finish();
	} else {
		/* It must be an archive. */
		debug("%s: archive\n", file);
		arch2();
	}
	closefile(file);
}
