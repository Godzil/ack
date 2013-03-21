/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef MACH_PROTO_NCG_ASSERT_H
#define MACH_PROTO_NCG_ASSERT_H

#ifndef NDEBUG
#define assert(x) if(!(x)) badassertion( #x ,__FILE__,__LINE__)
#else
#define assert(x)	/* nothing */
#endif

#endif /* MACH_PROTO_NCG_ASSERT_H */