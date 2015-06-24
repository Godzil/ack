/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

/*
 * If everything is kept in core, we must save some things for the second pass.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arch.h"
#include "out.h"
#include "const.h"
#include "assert.h"
#include "memory.h"
#include "defs.h"

extern bool	incore;

void savemagic()
{
	char	*p;

	if (!incore)
		return;

	if ((p = core_alloc(ALLOMODL, (long)sizeof(int))) != (char *)0) {
		*(unsigned short *)p = AALMAG;
		core_position += sizeof(int);
	}
}

void savehdr(struct ar_hdr * hdr)
{
	char	*p;

	if (!incore)
		return;

	if ((p=core_alloc(ALLOMODL,(long)sizeof(struct ar_hdr)))!=(char *)0) {
		*(struct ar_hdr *)p = *hdr;
		core_position += int_align(sizeof(struct ar_hdr));
	}
}

long	NLChars = 0;	/* Size of string area for local names. */
long	NGChars = 0;	/* Idem for global names. */

/*
 * Put the string in cp into the block allocated for the string area.
 * Return its offset in this area. We don't use the first char of the string
 * area, so that empty strings can be distinguished from the first string.
 */
ind_t savechar(int piece, ind_t off)
{
	long	len;
	ind_t	newoff;

	if (off == (ind_t)0)
		return 0;

	len = strlen(modulptr(off)) + 1;
	if (piece == ALLOLCHR) {
		NLChars += len;
		if (!incore || (newoff = alloc(piece, len)) == BADOFF)
			return BADOFF;
	} else {
		assert(piece == ALLOGCHR);
		NGChars += len;
		if ((newoff = hard_alloc(piece, len)) == BADOFF)
			return BADOFF;
	}
	strcpy(address(piece, newoff), modulptr(off));
	return newoff;
}

/*
 * Put the local in `name' in the piece allocated for local names that must
 * be saved. `Name' points to a private copy, so will not become invalid after
 * allocation, but the string of which name->on_foff is the offset may be
 * destroyed, so we save that first.
 */
void savelocal(struct outname *name)
{
	ind_t		savindex;
	struct outname	*new;

	if ((savindex = savechar(ALLOLCHR, (ind_t)name->on_foff)) == BADOFF)
		return;

	new = (struct outname *)
			core_alloc(ALLOLOCL, (long)sizeof(struct outname));
	if (new != (struct outname *)0) {
		*new = *name;
		new->on_foff = savindex;
	}
}
