/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ P A C K . H
 */

void pack(alloc_p alloclist, bool time_opt, alloc_p *packed_out, alloc_p *not_packed_out, proc_p p);
			/* This is the packing system. It decides which 
			 * allations  to grant a register.
			 */
