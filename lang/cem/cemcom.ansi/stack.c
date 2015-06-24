/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S T A C K / U N S T A C K  R O U T I N E S	*/

#include	"lint.h"
#include	<system.h>
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include	"debug.h"
#include	"botch_free.h"
#include	<alloc.h>
#include	"Lpars.h"
#include	"arith.h"
#include	"stack.h"
#include	"stack_loc.h"
#include	"type.h"
#include	"idf.h"
#include	"idf_loc.h"
#include	"def.h"
#include	"struct.h"
#include	"level.h"
#include	"mes.h"
#include	"code_c.h"
#include	"error.h"
#include	"label.h"
#include	"util_loc.h"

/* #include	<em_reg.h> */

extern char options[];

static struct stack_level UniversalLevel;
struct stack_level *local_level = &UniversalLevel;
/*	The main reason for having this secondary stacking
	mechanism besides the linked lists pointed to by the idf's
	is efficiency.
	To remove the idf's of a given level, one could scan the
	hash table and chase down the idf chains; with a hash
	table size of 100 this is feasible, but with a size of say
	100000 this becomes painful. Therefore all idf's are also
	kept in a stack of sets, one set for each level.
*/

int level;	/* Always equal to local_level->sl_level. */

void stack_level()
{
	/*	A new level is added on top of the identifier stack.
	*/
	struct stack_level *stl = new_stack_level();
	struct stack_level *loclev = local_level;
	
	loclev->sl_next = stl;
	stl->sl_previous = loclev;
	stl->sl_level = ++level;
	stl->sl_max_block = loclev->sl_max_block;
	local_level = stl;
#ifdef	LINT
	lint_start_local();
#endif	/* LINT */
}

void stack_idf(struct idf *idf, struct stack_level *stl)
{
	/*	The identifier idf is inserted in the stack on level stl,
		but only if it is not already present at this level.
	*/
	struct stack_entry *se;
	
	se = stl->sl_entry;
	while (se) {
		if (se->se_idf == idf) return;
		se = se->next;
	}

	se = new_stack_entry();

	/* link it into the stack level */
	se->next = stl->sl_entry;
	se->se_idf = idf;
	stl->sl_entry = se;
}

struct stack_level *stack_level_of(int lvl)
{
	/*	The stack_level corresponding to level lvl is returned.
		The stack should probably be an array, to be extended with
		realloc where needed.
	*/
	struct stack_level *stl;

	if (lvl == level)
		return local_level;
	stl = &UniversalLevel;
		
	while (stl->sl_level != lvl) {
		stl = stl->sl_next;
	}
	return stl;
}

void unstack_level()
{
	/*	The top level of the identifier stack is removed.
	*/
	struct stack_level *lastlvl;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("before unstackidfs", 0);
#endif	/* DEBUG */

#ifdef	LINT
	lint_end_local(local_level);
#endif	/* LINT */

	/*	The implementation below is more careful than strictly
		necessary. Optimists may optimize it afterwards.
	*/
	while (local_level->sl_entry)	{
		struct stack_entry *se = local_level->sl_entry;
		struct idf *idf = se->se_idf;
		struct def *def;
		struct sdef *sdef;
		struct tag *tag;

		/* unlink it from the local stack level */
		local_level->sl_entry = se->next;
		free_stack_entry(se);

		if (level == L_LOCAL && (def = idf->id_label)) {
			unstack_label(idf);
			free_def(def);
			idf->id_label = 0;
		}
		while ((def = idf->id_def) && def->df_level >= level)	{
			/* unlink it from the def list under the idf block */
			if (def->df_sc == REGISTER || def->df_sc == AUTO)
				FreeLocal(def->df_address);
			idf->id_def = def->next;
			free_def(def);
			update_ahead(idf);
		}
		while (	(sdef = idf->id_sdef)
		&&	sdef->sd_level >= level
		)	{
			/* unlink it from the sdef list under the idf block */
			idf->id_sdef = sdef->next;
			free_sdef(sdef);
		}
		while (	(tag = idf->id_tag)
		&&	tag->tg_level >= level
		)	{
			/* unlink it from the struct list under the idf block */
			idf->id_tag = tag->next;
			free_tag(tag);
		}
	}
	/*	Unlink the local stack level from the stack.
	*/
	lastlvl = local_level;
	local_level = local_level->sl_previous;
	if (level >= L_LOCAL)	{
		local_level->sl_max_block = lastlvl->sl_max_block;
	}
	free_stack_level(lastlvl);
	local_level->sl_next = (struct stack_level *) 0;
	level = local_level->sl_level;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("after unstackidfs", 0);
#endif	/* DEBUG */
}

void unstack_world()
{
	/*	The global level of identifiers is scanned, and final
		decisions are taken about such issues as
		extern/static/global and un/initialized.
		Effects on the code generator: initialised variables
		have already been encoded while the uninitialised ones
		are not and have to be encoded at this moment.
	*/
	struct stack_entry *se = local_level->sl_entry;

#ifdef	LINT
	lint_end_global(local_level);
#endif	/* LINT */

#ifdef GEN_NM_LIST
	open_name_list();
#endif /* GEN_NM_LIST */

	while (se)	{
		struct idf *idf = se->se_idf;
		struct def *def = idf->id_def;
		
		if (!def)	{
			/* global selectors, etc. */
			se = se->next;
			continue;
		}
		
#ifdef DEBUG
		if (options['a']) {
			char *symbol2str();

			print("\"%s\", %s, %s, %s, %s\n",
				idf->id_text,
				(def->df_alloc == 0) ? "no alloc" :
				(def->df_alloc == ALLOC_SEEN) ? "alloc seen" :
				(def->df_alloc == ALLOC_DONE) ? "alloc done" :
				"illegal alloc info",
				symbol2str(def->df_sc),
				def->df_initialized ? "init" : "no init",
				def->df_used ? "used" : "not used");
		}
#endif /* DEBUG */
		if (def->df_sc == STATIC
		    && def->df_type->tp_fund == FUNCTION
		    && !def->df_initialized) {
			/* orphaned static function */
			warning("static function %s never defined, %s"
				    , idf->id_text
				    , "changed to extern");
			def->df_sc = EXTERN;
		}
		
		if (def->df_alloc == ALLOC_SEEN
		    && !def->df_initialized) {
			/* space must be allocated */
			bss(idf);
#ifdef GEN_NM_LIST
			if (def->df_sc != STATIC)
				namelist(idf->id_text);	/* may be common */
#endif /* GEN_NM_LIST */
			def->df_alloc = ALLOC_DONE;	/* see Note below */
		}
		se = se->next;
	}
	/*	Note:
		df_alloc must be set to ALLOC_DONE because the idf entry
		may occur several times in the list.
		The reason for this is that the same name may be used
		for different purposes on the same level, e.g.
			struct s {int s;} s;
		is a legal definition and contains 3 defining occurrences
		of s.
		Each definition has been entered into the identifier stack.
		Although only one of them concerns a variable, we meet the
		s 3 times when scanning the identifier stack.
	*/
}

#ifdef GEN_NM_LIST
/*	A list of potential common names is kept, to be fed to
	an understanding loader.  The list is written to a file
	the name of which is nmlist.  If nmlist == NULL, no name
	list is generated.
*/
extern char *nmlist;	/* BAH! -- main.c	*/
static File *nfp = 0;

void open_name_list()
{
	if (nmlist && sys_open(nmlist, OP_WRITE, &nfp) == 0)
		fatal("cannot create namelist %s", nmlist);
}

void namelist(char *nm)
{
	if (nmlist)	{
		sys_write(nfp, nm, strlen(nm));
		sys_write(nfp, "\n", 1);
	}
}
#endif /* GEN_NM_LIST */
