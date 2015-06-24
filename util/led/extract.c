/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdlib.h>
#include <stdio.h>
#include "arch.h"
#include "out.h"
#include "const.h"
#include "debug.h"
#include "memory.h"
#include "orig.h"
#include "scan.h"
#include "defs.h"

static void get_names(struct outhead	*head);
static void process(struct outhead *head);
static void getexternal(struct outname *name);
static void redefine(struct outname *new, struct outname *old);
static void transfer(struct outname	*src, struct outname	*dst);

/*
 * Get section sizes and symboltable information from present module.
 */
void extract()
{
	struct outhead	head;

	get_modul();
	/*
	 * Copy head because we need it so often but it can change place,
	 * so we can't trust a pointer to it.
	 */
	head = *(struct outhead *)modulptr(IND_HEAD);
	get_names(&head);
	process(&head);
	skip_modul(&head);
}

unsigned short	NLocals = 0;	/* Number of local names to be saved. */
unsigned short	NGlobals = 0;	/* Number of global names. */

/*
 * Walk through the nametable of this module, counting the locals that must
 * appear in the final output file if this module is linked.
 * That number will be returned.
 */
static void get_names(struct outhead	*head)
{
	register int	nnames;
	register ind_t	nameindex, charindex;
	register ind_t	charoff;
	extern int	flagword;

	nnames = head->oh_nname;
	nameindex = IND_NAME(*head);
	charindex = IND_CHAR(*head);
	charoff = OFF_CHAR(*head);
	while (nnames--) {
		struct outname		name;	/* A local copy. */
		/*
		 * Because savelocal/getexternal might relocate the modules
		 * we have to compute the core addresses again.
		 */
		name = *(struct outname *)modulptr(nameindex);
		/*
		 * Change the offset in file into an offset in the memory area.
		 * There will always be at least a header before the string
		 * area, so we don't have to be afraid to confuse "no name"
		 * with "the first name".
		 */
		if (name.on_foff) {
			if (name.on_foff < charoff ||
			    name.on_foff >= charoff+head->oh_nchar) {
				fatal("illegal offset in name");
			}
			name.on_foff += charindex - charoff;
		}
		namerelocate(&name);
		if ((name.on_type & S_TYP) == S_CRS) {
			name.on_valu += charindex - charoff;
			name.on_valu = savechar(ALLOGCHR, (ind_t)name.on_valu);
		}
		if (name.on_type & S_EXT) {
			getexternal(&name);
		} else {
			/*
			 * The only thing we want to know about locals is
			 * whether they must appear in the output file.
			 */
			if (!(flagword & SFLAG) && mustsavelocal(&name)) {
				NLocals++;
				savelocal(&name);
			}
		}
		nameindex += sizeof(struct outname);
	}
}

extern struct orig	relorig[];

static void process(struct outhead *head)
{
	struct outsect	*sects;
	struct outsect	*outsp;
	int		nsect;
	struct orig	*orig = relorig;
	extern struct outhead	outhead;
	extern struct outsect	outsect[];

	outhead.oh_nrelo += head->oh_nrelo;
	outhead.oh_nemit += head->oh_nemit;
	if (head->oh_nsect > outhead.oh_nsect)
		outhead.oh_nsect = head->oh_nsect;
	sects = (struct outsect *)modulptr(IND_SECT(*head));
	nsect = head->oh_nsect;
	outsp = outsect;
	while (nsect--) {
		if (sects->os_flen) {
			/* contains non-zero stuff */
			outhead.oh_nemit += outsp->os_size - outsp->os_flen;
			outsp->os_flen = outsp->os_size + sects->os_flen;
		}
		else {
			outsp->os_flen += sects->os_flen;
		}
		outsp->os_size += sects->os_size;
		/*
		 * Add all flen's and all (size - flen == zero)'s of
		 * preceding sections with the same number.
		 */
		orig->org_size = outsp->os_size;
		orig++; outsp++; sects++;
	}
}

/*
 * Add relocation constant for names in user defined sections.
 * The value of a common name indicates a size instead of an offset,
 * and hence shouldn't be relocated.
 * Otherwise we just add the accumulated size of all normal parts in preceding
 * sections with the same size.
 */
void namerelocate(struct outname *name)
{
	int	type = name->on_type;
	int	sct = type & S_TYP;

	if (sct == S_UND || sct == S_ABS || sct == S_CRS)
		return;
	if (type & S_COM) {
		if ( ! (type&S_EXT) ) fatal("local commons should be handled by the assembler") ;
		return;
	}

	name->on_valu += relorig[(type & S_TYP) - S_MIN].org_size;
}

/*
 * If we see this name for the first time, we must remember it for
 * we might need it later on. Otherwise it must confirm to what we already
 * know about it, and eventually add to that knowledge.
 */
static void getexternal(struct outname *name)
{
	register char		*string;
	register int		h;
	register struct outname	*old;
	extern int		hash();
	extern struct outname	*searchname();

	string = modulptr((ind_t)name->on_foff);
	h = hash(string);
	old = searchname(string, h);
	if (old == (struct outname *)0) {
		NGlobals++;
		entername(name, h);
		if (ISUNDEFINED(name)) {
			verbose("requires %s", string, 0, 0, 0);
		}
	} else if (!ISUNDEFINED(name)) {
		if (ISUNDEFINED(old)) {
			name->on_mptr = string;	/* Just for convenience. */
			transfer(name, old);
		} else {
			name->on_mptr = string;	/* Just for convenience. */
			redefine(name, old);
		}
	}
}

/*
 * Handle the redefinition of `new' in the current module.
 * A name can be defined in three ways, in increasing priority:
 *	undefined,
 *	common,
 *	defined in a section.
 * A name may become "higher" when defined, but not "lower".
 * A redefinition as common is allowed. It is ignored, but a warning is given
 * when the desired section of `new' doesn't correspond with the section of
 * `old'. If a common definition is given again for a name, we take the
 * greatest value so that the common declared name always has enough space.
 * If a common is defined as a not-common, the old definition is ignored.
 */
static void redefine(struct outname *new, struct outname *old)
{
	if (!ISCOMMON(old)) {
		if (!ISCOMMON(new))
			error("%s: multiply defined", new->on_mptr);
		/*
		else if ((new->on_type & S_TYP) != (old->on_type & S_TYP))
			warning("%s: sections differ", new->on_mptr);
		*/
	} else {
		/* `Old' is common. */
		if (ISCOMMON(new)) {
			if ((new->on_type & S_TYP) != (old->on_type & S_TYP))
				warning("%s: sections differ", new->on_mptr);

			if (new->on_valu > old->on_valu)
				old->on_valu = new->on_valu;
		} else {
			transfer(new, old);
		}
	}
}

/*
 * Transfer things we want to know from `src' to `dst'.
 */
static void transfer(struct outname	*src, struct outname	*dst)
{
	debug("%s defined here\n", src->on_mptr);
	dst->on_valu = src->on_valu;
	dst->on_type = src->on_type;
	dst->on_desc = src->on_desc;
}
