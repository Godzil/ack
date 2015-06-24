/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include "arch.h"
#include "out.h"
#include "ranlib.h"
#include "const.h"
#include "debug.h"
#include "memory.h"
#include "defs.h"

#define ENDLIB		((long)0)

static struct ar_hdr	arhdr;

static void notelib(long pos);

/*
 * First read a long telling how many ranlib structs there are, then
 * the structs themselves. Second read a long telling how many chars there are
 * in the string table, then the string table itself.
 * We keep only one ranlib table in core, so this table always starts at offset
 * (ind_t)0 from its base.
 */
static long getsymdeftable()
{
	ind_t		off;
	struct ranlib	*ran;
	long		count;
	long		nran, nchar;
	extern long		rd_long();
	extern int		infile;

	count = nran = rd_long(infile);
	debug("%ld ranlib structs, ", nran);
	off = hard_alloc(ALLORANL, nran * sizeof(struct ranlib));
	if (off == BADOFF)
		fatal("no space for ranlib structs");
	ran = (struct ranlib *)address(ALLORANL, off);
	rd_ranlib(infile, ran, count);
	nchar = rd_long(infile);
	debug("%ld ranlib chars\n", nchar);
	if ((off = hard_alloc(ALLORANL, nchar)) == BADOFF)
		fatal("no space for ranlib strings");
	rd_bytes(infile, address(ALLORANL, off), nchar);
	ran = (struct ranlib *)address(ALLORANL, (ind_t)0);
	while (count--) {
		/*
		 * Adjust because names are now in core, not on file.
		 * Note that `ran_off' is measured from the beginning of the
		 * string area, NOT from the beginning of the file.
		 */
		if (ran->ran_off >= nchar)
			fatal("bad ranlib string offset");
		ran->ran_off += off;
		ran++;
	}
	return nran;
}

extern char	*modulname;

/*
 * Process archive with table of contents. The table of contents tells
 * of symbols in which module they are defined. We scan the table for
 * symbols that are known but not yet defined. Then we extract all necessary
 * information from the corresponding module. This module may need symbols that
 * were defined in modules located before this one in the archive, so we
 * scan the table again. We perform these actions as long as new symbols
 * are defined.
 */
void arch()
{
	long	nran;
	bool	resolved;

	nran = getsymdeftable();

	savemagic();
	do {
		register ind_t	ranindex;
		register long	count;

		debug("(re)scan ranlib table\n");
		ranindex = (ind_t)0;
		count = nran;
		resolved = FALSE;
		while (count > 0) {
			register struct ranlib	*ran;
			register char		*string;
			register struct outname	*name;
			register long		pos;
			extern int		hash();
			extern struct outname	*searchname();

			ran = (struct ranlib *)address(ALLORANL, ranindex);
			string = address(ALLORANL, (ind_t)ran->ran_off);
			name = searchname(string, hash(string));
			if (name == (struct outname *)0 || !ISUNDEFINED(name)) {
				ranindex += sizeof(struct ranlib);
				count--;
				continue;
			}
			seek(ran->ran_pos);
			get_archive_header(&arhdr);
			modulname = arhdr.ar_name;
			verbose("defines %s", string);
			resolved = TRUE;
			/*
			 * This archive member is going to be linked,
			 * so we don't need to know what else it defines.
			 * Note that we assume that all ranlib information of
			 * one archive member is contiguous.
			 */
			pos = ran->ran_pos;
			do {
				count--; ran++;
				ranindex += sizeof(struct ranlib);
			} while (count > 0 && ran->ran_pos == pos);
			notelib(pos);
			savehdr(&arhdr);
			extract();
		}
	} while (resolved);

	dealloc(ALLORANL);
	notelib(ENDLIB);
}

/*
 * An archive member that will be loaded is remembered by storing its position
 * in the archive into the table of positions.
 */
static void notelib(long pos)
{
	ind_t	off;

	if ((off = hard_alloc(ALLOARCH, (long)sizeof(long))) == BADOFF)
		fatal("no space for archive position");
	*(long *)address(ALLOARCH, off) = pos;
}

/*
 * Index of position of first archive member of next archive.
 */
static ind_t		posindex = (ind_t)0;

/*
 * Process the archive in pass 2.
 * We walk through the table of positions telling at what byte offset the
 * archive header + module is located, until this position is ENDLIB, meaning
 * that we've processed all needed modules in this archive. Each group of
 * positions of an archive is terminated with ENDLIB.
 */
void arch2()
{
	long	*pos;
	ind_t	localpos;

	localpos = posindex;
	for (	pos = (long *)address(ALLOARCH, localpos);
		*pos != ENDLIB;
		pos++, localpos += sizeof(long)
	) {
		seek(*pos);
		get_archive_header(&arhdr);
		modulname = arhdr.ar_name;
		debug("%s: archive member\n", modulname);
		finish();
	}
	localpos += sizeof(long);	/* Skip ENDLIB. */
	posindex = localpos;		/* Remember for next call. */
}
