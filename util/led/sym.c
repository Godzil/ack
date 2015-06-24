/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

/*
 * Symbol table management.
 */

#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "const.h"
#include "arch.h"
#include "memory.h"
#include "debug.h"
#include "defs.h"

/*
 * Symbol table types. Each hash table entry contains the offset of a symbol
 * struct. `Sy_name' contains the offset the name in the piece of global
 * names. `Sy_next' contains the offset of the next symbol of which the
 * corresponding name has the same hash value.
 */
struct symbol {
	ind_t	sy_name;
	ind_t	sy_next;
};

#define NHASH	307		/* Size of hash table. Must be odd. */

static ind_t	hashtable[NHASH];

/*
 * Initialize the symbol table. All indices should be noticeably invalid.
 */
void init_symboltable()
{
	ind_t	*rap;

	for (rap = hashtable; rap < &hashtable[NHASH]; rap++)
		*rap = BADOFF;
}

/*
 * Search for `string' in the symboltable. The hash value of `string' is in
 * `hashval'. The linked list belonging to the entry of hashval
 * in the hash table is followed. If the names match, a pointer to the outname
 * in this element of the list is returned. When a match cannot be found,
 * NIL is returned.
 */ 
struct outname *searchname(char *string, int hashval)
{
	char		*rcp;
	char		*namestring;
	ind_t		symindex;
	struct outname	*name;
	struct symbol	*sym;

	symindex = hashtable[hashval];
	debug("looking for %s %d %ld:", string, hashval, hashtable[hashval]);
	while (symindex != BADOFF) {
		sym = (struct symbol *)address(ALLOSYMB, symindex);
		name = (struct outname *)address(ALLOGLOB, sym->sy_name);
		namestring = address(ALLOGCHR, (ind_t)name->on_foff);
		rcp = string;
		debug("comp %s;", namestring);
		while (*rcp == *namestring++)
			if (*rcp++ == '\0') {
				debug("found %x, %x, %lx\n",
					name->on_type, name->on_desc, name->on_valu);
				return name;
			}	
		symindex = sym->sy_next;
	}
	/* Not found. */
	debug("not found\n");
	return (struct outname *)0;
}

/*
 * Enter a new name in the symbol table. We must copy everything to a
 * new entry. `Name' is a private copy, i.e. the pointer to it will not be
 * destroyed by allocation. However, the string of which name->on_foff is the
 * offset can be destroyed, so we save it first.
 */
void entername(struct outname *name, int hashval)
{
	ind_t		savindex;
	ind_t		symindex;
	ind_t		namindex;
	struct symbol	*sym;
	struct outname	*newname;

	debug("entername %s %d %x %x", modulptr((ind_t)name->on_foff), hashval, name->on_type, name->on_desc);
	savindex = savechar(ALLOGCHR, (ind_t)name->on_foff);
	symindex = hard_alloc(ALLOSYMB, (long)sizeof(struct symbol));
	debug("; %ld\n", symindex);
	namindex = hard_alloc(ALLOGLOB, (long)sizeof(struct outname));
	if (savindex == BADOFF || symindex == BADOFF || namindex == BADOFF)
		fatal("symbol table overflow");
	sym = (struct symbol *)address(ALLOSYMB, symindex);
	sym->sy_name = namindex;
	newname = (struct outname *)address(ALLOGLOB, namindex);
	*newname = *name;
	newname->on_foff = savindex;
	sym->sy_next = hashtable[hashval];
	hashtable[hashval] = symindex;
}

/*
 * Return the index of `name' in the symbol table in the order in which
 * it was entered. We need a REAL index, not a byte offset.
 */
unsigned int indexof(struct outname *name)
{
	return name - (struct outname *)address(ALLOGLOB, (ind_t)0);
}

/*
 * Assign an integer to the string in p.
 * 0 <= hash(p) < NHASH, so it can - and will - be used
 * as index in a hash table.
 */
int hash(char *p)
{
	unsigned short	h = 0;
	int		c;

	while ((c = *p++)) {
		h <<= 2;
		h += c;
	}
	return h % NHASH;
}
