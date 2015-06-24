/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Memory manager. Memory is divided into NMEMS pieces. There is a struct
 * for each piece telling where it is, how many bytes are used, and how may
 * are left. If a request for core doesn't fit in the left bytes, an sbrk()
 * is done and pieces after the one that requested the growth are moved up.
 *
 * Unfortunately, we cannot use sbrk to request more memory, because its
 * result cannot be trusted. More specifically, it does not work properly
 * on 2.9 BSD, and probably does not work properly on 2.8 BSD and V7 either.
 * The problem is that "sbrk" adds the increment to the current "break"
 * WITHOUT testing the carry bit. So, if your break is at 40000, and
 * you "sbrk(30000)", it will succeed, but your break will be at 4464
 * (70000 - 65536).
 */

#include <stdio.h>
#include <unistd.h>
#include <out.h>
#include "const.h"
#include "assert.h"
#include "debug.h"
#include "arch.h"
#include "memory.h"
#include "defs.h"
#include "object.h"

#include <missing_proto.h>

static ind_t move_up(int piece, ind_t incr);
static bool compact(int piece, ind_t incr, int flag);
static void copy_down(struct memory *mem, ind_t dist);
static void copy_up(struct memory *mem, ind_t dist);
static void free_saved_moduls();

struct memory	mems[NMEMS];

bool	incore = TRUE;	/* TRUE while everything can be kept in core. */
ind_t	core_position = (ind_t)0;	/* Index of current module. */

#define GRANULE		64	/* power of 2 */

static char *BASE;
static ind_t refused;

int sbreak(ind_t incr)
{
	unsigned int inc;

	incr = (incr + (GRANULE - 1)) & ~(GRANULE - 1);

	inc = incr;
	if ((refused && refused < incr) ||
	    (sizeof(char *) < sizeof(long) &&
	     (inc != incr || BASE + inc < BASE)) ||
	    brk(BASE + incr) == (void *)-1) {
		if (!refused || refused > incr)
			refused = incr;
		return -1;
	}
	BASE += incr;
	return 0;
}

/*
 * Initialize some pieces of core. We hope that this will be our last
 * real allocation, meaning we've made the right choices.
 */
void init_core()
{
	char		*base;
	ind_t		total_size;
	struct memory	*mem;

#include "mach.c"
#define ALIGN 8			/* minimum alignment for pieces */
#define AT_LEAST	(ind_t)2*ALIGN	/* See comment about string areas. */

	total_size = (ind_t)0;	/* Will accumulate the sizes. */
	BASE = base = (char *)sbrk(0);		/* First free. */
	if ((int)base % ALIGN) {
		base  = (char *)sbrk(ALIGN - (int)base % ALIGN);
		BASE = base = (char *)sbrk(0);
	}
	/*
	 * String areas are special-cased. The first byte is unused as a way to
	 * distinguish a name without string from a name which has the first
	 * string in the string area.
	 */
	for (mem = mems; mem < &mems[NMEMS]; mem++) {
		mem->mem_base = base;
		mem->mem_full = (ind_t)0;
		if (mem == &mems[ALLOLCHR] || mem == &mems[ALLOGCHR]) {
			if (mem->mem_left == 0) {
				mem->mem_left = ALIGN;
				total_size += ALIGN;
				base += ALIGN;
			}
			base += mem->mem_left;
			total_size += mem->mem_left;
			mem->mem_left--;
			mem->mem_full++;
		}
		else {
			base += mem->mem_left;	/* Each piece will start after prev. */
			total_size += mem->mem_left;
		}
	}

	if (sbreak(total_size) == -1) {
		incore = FALSE;	/* In core strategy failed. */
		if (sbreak(AT_LEAST) == -1)
			fatal("no core at all");
		
		base = BASE;
		for (mem = mems; mem < &mems[NMEMS]; mem++) {
			mem->mem_base = base;
			if (mem == &mems[ALLOLCHR] || mem == &mems[ALLOGCHR]) {
				base += ALIGN;
				mem->mem_left = ALIGN - 1;
				mem->mem_full = 1;
			}
			else {
				mem->mem_full = (ind_t)0;
				mem->mem_left = 0;
			}
		}
	}
}

/*
 * Allocate an extra block of `incr' bytes and move all pieces with index
 * higher than `piece' up with the size of the block.
 * Move up as much as possible, if "incr" fails.
 */
static ind_t move_up(int piece, ind_t incr)
{
	register struct memory	*mem;
#ifndef NOSTATISTICS
	extern int statistics;
#endif

	debug("move_up(%d, %d)\n", piece, (int)incr);
	while (incr > 0 && sbreak(incr) == -1)
		incr -= INCRSIZE;

	if (incr <= 0) {
		incr = 0;
		return (ind_t) 0;
	}
#ifndef NOSTATISTICS
	if (statistics) fprintf(stderr,"moving up %lx\n", (long) incr);
#endif
	for (mem = &mems[NMEMS - 1]; mem > &mems[piece]; mem--)
		copy_up(mem, incr);

	mems[piece].mem_left += incr;
	return incr;
}

extern int	passnumber;

/*
 * This routine is called if `piece' needs `incr' bytes and the system won't
 * give them. We first steal the free bytes of all lower pieces and move them
 * and `piece' down. If that doesn't give us enough bytes, we steal the free
 * bytes of all higher pieces and move them up. We return whether we have
 * enough bytes, the first or the second time.
 */
#define NORMAL 0
#define FREEZE 1
#define FORCED 2
static bool compact(int piece, ind_t incr, int flag)
{
	ind_t		gain, size;
	struct memory	*mem;
	int min = piece, max = piece;
#define SHIFT_COUNT 2		/* let pieces only contribute if their free
				   memory is more than 1/2**SHIFT_COUNT * 100 %
				   of its occupied memory
				*/

	debug("compact(%d, %d, %d)\n", piece, (int)incr, flag);
	for (mem = &mems[0]; mem < &mems[NMEMS - 1]; mem++) {
		assert(mem->mem_base + mem->mem_full + mem->mem_left == (mem+1)->mem_base);
	}

	mem = &mems[piece];
	if (flag == NORMAL) {
		/* try and gain a bit more than needed */
		gain = (mem->mem_full + incr) >> SHIFT_COUNT;
		if (incr < gain) incr = gain;
	}
	
	/*
	 * First, check that moving will result in enough space
	 */
	if (flag != FREEZE) {
		gain = mem->mem_left;
		for (mem = &mems[piece-1]; mem >= &mems[0]; mem--) {
			/* 
			 * Don't give it all away! 
			 * If this does not give us enough, bad luck
			 */
			if (flag == FORCED)
				size = 0;
			else {
				size = mem->mem_full >> SHIFT_COUNT;
				if (size == 0) size = mem->mem_left >> 1;
			}
			if (mem->mem_left >= size)
				gain += (mem->mem_left - size) & ~(ALIGN - 1);
			if (gain >= incr) {
				min = mem - &mems[0];
				break;
			}
		}
		if (min == piece)
		    for (mem = &mems[piece+1]; mem <= &mems[NMEMS - 1]; mem++) {
			/* 
			 * Don't give it all away! 
			 * If this does not give us enough, bad luck
			 */
			if (flag == FORCED)
				size = 0;
			else {
				size = mem->mem_full >> SHIFT_COUNT;
				if (size == 0) size = mem->mem_left >> 1;
			}
			if (mem->mem_left >= size)
				gain += (mem->mem_left - size) & ~(ALIGN - 1);
			if (gain >= incr) {
				max = mem - &mems[0];
				break;
			}
		}
		if (min == piece) {
			min = 0;
			if (max == piece) max = 0;
		}
		if (gain < incr) return 0;
	}
	else {
		min = 0;
		max = NMEMS - 1;
	}

	gain = 0;
	for (mem = &mems[min]; mem != &mems[piece]; mem++) {
		/* Here memory is inserted before a piece. */
		assert(passnumber == FIRST || gain == (ind_t)0);
		if (gain) copy_down(mem, gain);
		if (flag == FREEZE || gain < incr) {
			if (flag != NORMAL) size = 0;
			else {
				size = mem->mem_full >> SHIFT_COUNT;
				if (size == 0) size = mem->mem_left >> 1;
			}
			if (mem->mem_left >= size) {
				size = (mem->mem_left - size) & ~(ALIGN - 1);
				gain += size;
				mem->mem_left -= size;
			}
		}
	}
	/*
	 * Now mems[piece]:
	 */
	if (gain) copy_down(mem, gain);
	gain += mem->mem_left;
	mem->mem_left = 0;

	if (gain < incr) {
		ind_t	up = (ind_t)0;

		for (mem = &mems[max]; mem > &mems[piece]; mem--) {
			/* Here memory is appended after a piece. */
			if (flag == FREEZE || gain + up < incr) {
				if (flag != NORMAL) size = 0;
				else {
					size = mem->mem_full >> SHIFT_COUNT;
					if (size == 0) size = mem->mem_left >> 1;
				}
				if (mem->mem_left >= size) {
					size = (mem->mem_left - size) & ~(ALIGN - 1);
					up += size;
					mem->mem_left -= size;
				}
			}
			if (up) copy_up(mem, up);
		}
		gain += up;
	}
	mems[piece].mem_left += gain;
	assert(flag == FREEZE || gain >= incr);
	for (mem = &mems[0]; mem < &mems[NMEMS - 1]; mem++) {
		assert(mem->mem_base + mem->mem_full + mem->mem_left == (mem+1)->mem_base);
	}
	return gain >= incr;
}

/*
 * The bytes of `mem' must be moved `dist' down in the address space.
 * We copy the bytes from low to high, because the tail of the new area may
 * overlap with the old area, but we do not want to overwrite them before they
 * are copied.
 */
static void copy_down(struct memory *mem, ind_t dist)
{
	char		*old;
	char		*new;
	ind_t		size;

	size = mem->mem_full;
	old = mem->mem_base;
	new = old - dist;
	mem->mem_base = new;
	while (size--)
		*new++ = *old++;
}

/*
 * The bytes of `mem' must be moved `dist' up in the address space.
 * We copy the bytes from high to low, because the tail of the new area may
 * overlap with the old area, but we do not want to overwrite them before they
 * are copied.
 */
static void copy_up(struct memory *mem, ind_t dist)
{
	char		*old;
	char		*new;
	ind_t		size;

	size = mem->mem_full;
	old = mem->mem_base + size;
	new = old + dist;
	while (size--)
		*--new = *--old;
	mem->mem_base = new;
}

static int alloctype = NORMAL;

/*
 * Add `size' bytes to the bytes already allocated for `piece'. If it has no
 * free bytes left, ask them from memory or, if that fails, from the free
 * bytes of other pieces. The offset of the new area is returned. No matter
 * how many times the area is moved, because of another allocate, this offset
 * remains valid.
 */
ind_t alloc(int piece, long size)
{
	ind_t		incr = 0;
	ind_t		left = mems[piece].mem_left;
	ind_t		full = mems[piece].mem_full;

	assert(passnumber == FIRST || (!incore && piece == ALLOMODL));
	if (size == (long)0)
		return full;
	if (size != (ind_t)size)
		return BADOFF;
	switch(piece) {
	case ALLOMODL:
	case ALLORANL:
		size = int_align(size);
	}

	if (size - left > 0)
		incr = ((size - left + (INCRSIZE - 1)) / INCRSIZE) * INCRSIZE;

	if (incr == 0 ||
	    (incr < left + full && (incr -= move_up(piece, left + full)) <= 0) ||
	    move_up(piece, incr) == incr ||
	    compact(piece, size, alloctype)) {
		mems[piece].mem_full += size;
		mems[piece].mem_left -= size;
		return full;
	} else {
		incore = FALSE;
		return BADOFF;
	}
}

/*
 * Same as alloc() but for a piece which really needs it. If the first
 * attempt fails, release the space occupied by other pieces and try again.
 */
ind_t hard_alloc(int piece, long size)
{
	ind_t	ret;
	int	i;

	if (size != (ind_t)size)
		return BADOFF;
	if ((ret = alloc(piece, size)) != BADOFF) {
		return ret;
	}

	/*
	 * Deallocate what we don't need.
	 */
	for (i = 0; i < NMEMS; i++) {
		switch (i) {
		case ALLOGLOB:
		case ALLOGCHR:
		case ALLOSYMB:
		case ALLOARCH:
		case ALLOMODL:
		case ALLORANL:
			break;	/* Do not try to deallocate this. */
		default:
			dealloc(i);
			break;
		}
	}
	free_saved_moduls();

	if ((ret = alloc(piece, size)) != BADOFF) {
		return ret;
	}

	alloctype = FORCED;
	ret = alloc(piece, size);
	alloctype = NORMAL;
	return ret;
}

/*
 * We don't need the previous modules, so we put the current module
 * at the start of the piece allocated for module contents, thereby
 * overwriting the saved modules, and release its space.
 */
static void free_saved_moduls()
{
	ind_t		size;
	char		*old, *new;
	struct memory	*mem = &mems[ALLOMODL];

	size = mem->mem_full - core_position;
	new = mem->mem_base;
	old = new + core_position;
	while (size--)
		*new++ = *old++;
	mem->mem_full -= core_position;
	mem->mem_left += core_position;
	core_position = (ind_t)0;
}

/*
 * The piece of memory with index `piece' is no longer needed.
 * We take care that it can be used by compact() later, if needed.
 */
void dealloc(int piece)
{
	/*
	 * Some pieces need their memory throughout the program.
	 */
	assert(piece != ALLOGLOB);
	assert(piece != ALLOGCHR);
	assert(piece != ALLOSYMB);
	assert(piece != ALLOARCH);
	mems[piece].mem_left += mems[piece].mem_full;
	mems[piece].mem_full = (ind_t)0;
}

char *core_alloc(int piece, long size)
{
	register ind_t	off;

	if ((off = alloc(piece, size)) == BADOFF)
		return (char *)0;
	return address(piece, off);
}

void core_free(int piece, char *p)
{
	char	*q = address(piece, mems[piece].mem_full);

	assert(p < q);
	if (sizeof(unsigned) == sizeof(char *))
	{
		mems[piece].mem_full -= (unsigned) (q - p);
		mems[piece].mem_left += (unsigned) (q - p);
	}
	else
	{
		mems[piece].mem_full -= (ind_t) q - (ind_t) p;
		mems[piece].mem_left += (ind_t) q - (ind_t) p;
	}
}

/*
 * Reset index into piece of memory for modules and
 * take care that the allocated pieces will not be moved.
 */
void freeze_core()
{
	int i;

	core_position = (ind_t)0;

	if (incore)
		return;

	for (i = 0; i < NMEMS; i++) {
		switch (i) {
		case ALLOGLOB:
		case ALLOGCHR:
		case ALLOSYMB:
		case ALLOARCH:
			break;	/* Do not try to deallocate this. */
		default:
			dealloc(i);
			break;
		}
	}
	compact(NMEMS - 1, (ind_t)0, FREEZE);
}

/* ------------------------------------------------------------------------- */

/*
 * To transform the various pieces of the output in core to the file format,
 * we must order the bytes in the unsigned shorts and longs as ACK prescribes.
 */
void write_bytes()
{
	unsigned short		nsect;
	long			offchar;
	struct memory	*mem;
	extern unsigned short	NLocals, NGlobals;
	extern long		NLChars, NGChars;
	extern int		flagword;
	extern struct outhead	outhead;
	extern struct outsect	outsect[];
	/* extern char		*outputname; */
	int			sectionno = 0;

	nsect = outhead.oh_nsect;
	offchar = OFF_CHAR(outhead);

	/*
	 * We allocated two areas: one for local and one for global names.
	 * Also, we used another kind of on_foff than on file.
	 * At the end of the global area we have put the section names.
	 */
	if (!(flagword & SFLAG)) {
		do_crs((struct outname *)mems[ALLOLOCL].mem_base, NLocals);
		namecpy((struct outname *)mems[ALLOLOCL].mem_base,
			NLocals,
			offchar
		);
		namecpy((struct outname *)mems[ALLOGLOB].mem_base,
			NGlobals + nsect,
			offchar + NLChars
		);
	}
	/*
	 * These pieces must always be written.
	 */
	wr_ohead(&outhead);
	wr_sect(outsect, nsect);
	for (mem = &mems[ALLOEMIT]; mem < &mems[ALLORELO]; mem++)
		wrt_emit(mem->mem_base, sectionno++, mem->mem_full);
	/*
	 * The rest depends on the flags.
	 */
	if (flagword & (RFLAG|CFLAG))
		wr_relo((struct outrelo *) mems[ALLORELO].mem_base,
			outhead.oh_nrelo);
	if (!(flagword & SFLAG)) {
		wr_name((struct outname *) mems[ALLOLOCL].mem_base,
			NLocals);
		wr_name((struct outname *) mems[ALLOGLOB].mem_base,
			NGlobals+nsect);
		wr_string(mems[ALLOLCHR].mem_base + 1, (long)NLChars);
		wr_string(mems[ALLOGCHR].mem_base + 1, (long)NGChars);
#ifdef SYMDBUG
		wr_dbug(mems[ALLODBUG].mem_base, mems[ALLODBUG].mem_full);
#endif /* SYMDBUG */
	}
}

void namecpy(struct outname	*name, unsigned int nname, long offchar)
{
	while (nname--) {
		if (name->on_foff)
			name->on_foff += offchar - 1;
		name++;
	}
}
