/*
 * show - make the contents of an ACK object file human readable.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <out.h>
#include "object.h"

#define OK	0	/* Return value of gethead if Orl Korekt. */
#define BMASK	0xFF	/* To extract least significant 8 bits from an int. */


void show(struct outhead *headp);
void showflags(unsigned int flagword);
void showsect();
void showname(struct outname *namep);
void error(char *format, ...);
void showrelo();


/* ARGSUSED */
#define prog	argv[0]

int main(int argc, char *argv[])
{
	char	**arg = argv;
	struct outhead	header;

	while (*++arg) {
		if (! rd_open(*arg)) {
			error("%s: cannot read %s\n", prog, *arg);
			continue;
		}
		rd_ohead(&header);
		if (BADMAGIC(header)) {
			error("%s: %s not an ACK object file\n", prog, *arg);
		} else {
			printf("%s:\n", *arg);
			show(&header);
		}
		rd_close();
	}
}

/*
 * Read an ACK object file from `fp' and show it in a human readable way.
 * NB. The header has already been read and is in the struct outhead `headp'
 * points to.
 */
void show(struct outhead *headp)
{
	int		i;
	struct outname	*np;
	struct outname	*name;	/* Dynamically allocated name-array. */
	char		*string;/* Base of string area. */
	extern char		*myalloc();

	printf("Version %d\n", headp->oh_stamp);
	showflags((unsigned) headp->oh_flags);
	/*
	 * Show all sections.
	 */
	for (i = 0; i < headp->oh_nsect; i++) {
		printf("Section %d:\n", i);
		showsect();
	}
	/*
	 * Show relocation information.
	 */
	for (i = 0; i < headp->oh_nrelo; i++) {
		printf("Relocation record %d:\n", i);
		showrelo();
	}
	/*
	 * We get all struct outname's and the strings in core first.
	 */
	name = (struct outname *) myalloc(headp->oh_nname * SZ_NAME);
	string = myalloc((unsigned) headp->oh_nchar);
	rd_name(name, headp->oh_nname);
	for (np = &name[0]; np < &name[headp->oh_nname]; np++) {
		if (np->on_foff != 0) {
			np->on_foff -= OFF_CHAR(*headp);
			if (np->on_foff >= headp->oh_nchar || np->on_foff < 0) {
				np->on_mptr = "????";
			}
			else np->on_mptr = string + np->on_foff;
		}
	}
	/*
	 * Transfer strings from file to core.
	 */
	rd_string(string, headp->oh_nchar);
	/*
	 * Now we can show all names.
	 */
	for (np = &name[0]; np < &name[headp->oh_nname]; np++) {
		printf("Name %ld:\n",(long)(np - name));
		showname(np);
	}
}

/*
 * Show flags from header.
 */
void showflags(unsigned int flagword)
{
	if (flagword & HF_LINK) printf("unresolved references left\n");
}

/*
 * Show a section.
 */
void showsect()
{
	struct outsect	section;

	rd_sect(&section, 1);
	printf("\tstartaddress in machine\t%ld\n",	section.os_base);
	printf("\tsection size in machine\t%ld\n",	section.os_size);
	printf("\tstartaddress in file\t%ld\n",		section.os_foff);
	printf("\tsection size in file\t%ld\n",		section.os_flen);
	printf("\tsection alignment\t%ld\n",		section.os_lign);
}

/*
 * Show a relocation record.
 */
void showrelo()
{
	struct outrelo	relrec;

	rd_relo(&relrec, 1);
	switch (relrec.or_type & RELSZ) {
	case RELO1:
		printf("\t1 byte\n");
		break;
	case RELO2:
		printf("\t2 bytes\n");
		break;
	case RELO4:
		printf("\t4 bytes\n");
		break;
	default:
		error("\tunexpected relocation length\n");
		break;
	}
	if (relrec.or_type & RELPC) printf("\tpc relative\n");
	if (relrec.or_type & RELBR) printf("\tbytes reversed\n");
	if (relrec.or_type & RELWR) printf("\twords reversed\n");
	printf("\treferencing section\t%d\n", (relrec.or_sect & BMASK) - S_MIN);
	printf("\treferenced symbol index\t%d\n", relrec.or_nami);
	printf("\treferencing address\t%ld\n", relrec.or_addr);
}

/*
 * Show the name in the struct `namep' points to.
 */
void showname(struct outname *namep)
{
	if (namep->on_mptr)
		printf("\t%s\n", namep->on_mptr);
	else
		printf("\tno name\n");
	switch (namep->on_type & S_TYP) {
	case S_UND:
		printf("\tundefined\n");
		break;
	case S_ABS:
		printf("\tabsolute\n");
		break;
	case S_CRS:
		printf("\tcross reference\n");
	default:
		printf("\tin section %d\n", (namep->on_type & S_TYP) - S_MIN);
		break;
	}
	if (namep->on_type & S_EXT) printf("\texternal\n");
	if (namep->on_type & S_STB) {
		printf("\tstab 0x%x\n", namep->on_type >> 8);
		printf("\tdesc 0x%x\n", namep->on_desc);
	}
	else switch (namep->on_type & S_ETC) {
	case S_SCT:
		printf("\tsection name\n"); break;
	case S_LIN:
		printf("\thll source line item\n"); break;
	case S_FIL:
		printf("\thll source file item\n"); break;
	case S_MOD:
		printf("\tass src file item\n"); break;
	case S_COM:
		printf("\tcommon\n"); break;
	case 0:
		break;
	default:
		printf("\tstab 0x%x\n", namep->on_type >> 8);
		printf("\tdesc 0x%x\n", namep->on_desc);
	}
	printf("\tvalue %ld\n", namep->on_valu);
}

/*
 * Core allocation via malloc() but fatal if no core.
 */
char *myalloc(unsigned int u)
{
	register char	*rcp;

	rcp = malloc(u);
	if (rcp == (char *) 0) {
		error("Out of core\n");
		exit(1);
	}
	return rcp;
}

/* VARARGS1 */
void error(char *format, ...)
{
	va_list ap;
	fflush(stdout);
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

void rd_fatal()
{
	error("Error in reading the object file\n");
	exit(1);
}
