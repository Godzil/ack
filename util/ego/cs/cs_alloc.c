/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "../share/types.h"
#include "../share/alloc.h"
#include "cs.h"

occur_p newoccur(line_p l1, line_p l2, bblock_p b)
{
	/* Allocate a new struct occur and initialize it. */
	occur_p rop;

	rop = (occur_p) newcore(sizeof(struct occur));
	rop->oc_lfirst = l1; rop->oc_llast = l2; rop->oc_belongs = b;
	return rop;
}

void oldoccur(occur_p ocp)
{
	oldcore((char *) ocp, sizeof(struct occur));
}

avail_p newavail()
{
	return (avail_p) newcore(sizeof(struct avail));
}

void oldavail(avail_p avp)
{
	oldcore((char *) avp, sizeof(struct avail));
}

entity_p newentity()
{
	return (entity_p) newcore(sizeof(struct entity));
}

void oldentity(entity_p enp)
{
	oldcore((char *) enp, sizeof(struct entity));
}
