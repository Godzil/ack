/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#define ALLOEMIT	0			/* Section contents. */
#define ALLORELO	(ALLOEMIT + MAXSECT)	/* Relocation table. */
#define ALLOLOCL	(ALLORELO + 1)		/* Saved local names. */
#define ALLOGLOB	(ALLOLOCL + 1)		/* Saved global names. */
#define ALLOLCHR	(ALLOGLOB + 1)		/* Strings of local names. */
#define ALLOGCHR	(ALLOLCHR + 1)		/* Strings of global names. */
#ifdef SYMDEBUG
#define ALLODBUG	(ALLOGCHR + 1)		/* Symbolic debugging info. */
#else /* SYMDEBUG */
#define ALLODBUG	ALLOGCHR
#endif /* SYMDEBUG */
#define ALLOSYMB	(ALLODBUG + 1)		/* Symbol table. */
#define ALLOARCH	(ALLOSYMB + 1)		/* Archive positions. */
#define ALLOMODL	(ALLOARCH + 1)		/* Modules. */
#define ALLORANL	(ALLOMODL + 1)		/* Ranlib information. */
#define NMEMS		(ALLORANL + 1)

#define BADOFF		((ind_t)-1)

typedef long		ind_t;

struct memory {
	char	*mem_base;
	ind_t	mem_full;
	ind_t	mem_left;
};
extern struct memory	mems[];

#define address(piece,offset)	(mems[(piece)].mem_base+(offset))
#define modulptr(offset)	(mems[ALLOMODL].mem_base+core_position+(offset))

#define int_align(sz)		(((sz)+(sizeof(int)-1))&~(int)(sizeof(int)-1))

extern ind_t		core_position;

void init_core();
ind_t hard_alloc(int piece, long size);
ind_t alloc(int piece, long size);
void dealloc(int piece);
char *core_alloc(int piece, long size);
void core_free(int piece, char *p);
void freeze_core();
void write_bytes();
void namecpy(struct outname	*name, unsigned int nname, long offchar);
