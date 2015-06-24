/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef MACH_PROTO_NCG_TYPES_H
#define MACH_PROTO_NCG_TYPES_H

#ifndef TEM_WSIZE
#warning TEM_WSIZE should be defined at this point
#endif
#ifndef TEM_PSIZE
#warning TEM_PSIZE should be defined at this point
#endif
#if TEM_WSIZE>4 || TEM_PSIZE>4
#error Implementation will not be correct unless a long integer has more then 4 bytes of precision.
#endif

typedef char byte;
typedef char * string;

#if TEM_WSIZE>2 || TEM_PSIZE>2
#define full            long
#else
#define full            int
#endif

#define word long
#ifndef WRD_FMT
#define WRD_FMT "%ld"
#endif /* WRD_FMT */

#endif /* MACH_PROTO_NCG_TYPES_H */