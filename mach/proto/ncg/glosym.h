/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef MACH_PROTO_NCG_GLOSYM_H
#define MACH_PROTO_NCG_GLOSYM_H

typedef struct glosym {
	struct glosym *gl_next;
	string	       gl_name;
	word	       gl_rom[MAXROM+1];
} glosym_t,*glosym_p;

void enterglo(string name, word *romp);
glosym_p lookglo(string name);

#endif /* MACH_PROTO_NCG_GLOSYM_H */