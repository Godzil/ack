/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* 
 * sbrk(), brk() emulation based on calloc() 
 * Based on version from D A Gwyn
 * 02-Mar-1990     D A Gwyn 
 * http://www.linuxmisc.com/10-unix-questions/0875f91c36e18724.htm
 */
#include <stdio.h>
#include <errno.h>                      /* for errno, ENOMEM */
#if __STDC__
#include <stdlib.h>                     /* for calloc */
#else 
extern char *calloc();
#endif

/* Allocate 64MB to brk/sbrk should be enough for such application */
#ifndef HEAP_SIZE                       /* with 32-bit ints, 0x200000 is recommended */
#define HEAP_SIZE       0x4000000       /* size of simulated heap, in bytes */
#endif
#define BRK_OK          0
#define BRK_ERR         (-1)
#define SBRK_ERR        ((void *)-1)    /* horrible interface design */

static void *bottom = NULL;             /* bottom of calloc()ed pseudo-heap */
static void *brkval = NULL;             /* current value of simulated break */

void *brk_emu(const void *endds )
{
	int offset;
	if ( bottom == NULL )
	{
		if ( (bottom = calloc( HEAP_SIZE, 1 )) == 0 )
		{
			return (void *)BRK_ERR; /* unable to set up pseudo-heap */
		}
		else
		{
			brkval = bottom;
		}
	}

	if ( (offset = endds - bottom) < 0 || offset > HEAP_SIZE )
	{
		errno = ENOMEM;
		return (void *)BRK_ERR; /* attempt to set break out of heap */
	}
	else
	{
		brkval = (void *)endds;
		return (void *)BRK_OK;
	}
}

void *sbrk_emu(int incr)
{
	int offset;
	if ( bottom == 0 )
	{
		if ( (bottom = (char *)calloc( HEAP_SIZE, 1 )) == 0 )
		{
			return SBRK_ERR; /* unable to set up heap */
		}
		else
		{
			brkval = bottom;
		}
	}

	if ( (offset = (brkval - bottom) + incr) < 0 || offset > HEAP_SIZE )
	{
		errno = ENOMEM;
		return SBRK_ERR; /* attempt to set break out of heap */
	}
	else
	{
		char *oldbrk = brkval;
		brkval += incr;
		return oldbrk;
	}
}