/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTILS_ACK_TRF_H
#define UTILS_ACK_TRF_H

#include "list.h"

#ifndef NORCSID
#define RCS_TRANS "$Id$"
#endif

/* This structure is the center of all actions */
/* It contains the description of all phases,
   the suffices they consume and produce and various properties */

typedef struct transform trf;

struct transform {
	char    *t_in ;         /* Suffices in '.o.k' */
	char    *t_out ;        /* Result '.suffix' */
	char	*t_outfile ;	/* Resulting output file */
	char    *t_name ;       /* The name of this transformation */
	list_head t_mapf ;      /* Mapflags argument, uses varrep */
	char    *t_argd ;       /* Argument descriptor, uses varrep */
	char    *t_needed ;     /* Suffix indicating the libraries needed */
	char    *t_rts ;        /* Suffix indicating the major language used*/
	unsigned int     t_stdin:1 ;     /* The input is taken on stdin */
	unsigned int     t_stdout:1 ;    /* The output comes on stdout */
	unsigned int     t_combine:1 ;   /* Transform several files to one result */
	unsigned int     t_visited:1 ;   /* NO before setup, YES after */
	unsigned int     t_prep:2 ;      /* Needs preprocessor YES/NO/MAYBE */
	unsigned int     t_isprep:1 ;    /* Is preprocessor */
	unsigned int     t_keep:1 ;      /* Keep the output file */
	unsigned int     t_scan:1 ;      /* Used while finding path's */
	unsigned int	t_bscan:1 ;	/* Best scan so far, while finding path's */
	unsigned int	t_linker:1 ;	/* The linker usurps all unrecognized flags */
	unsigned int	t_do:1 ;	/* Is in a path to execute */
	unsigned int	t_blocked:1 ;	/* An input file could not be produced */
	short   t_optim ;       /* Is optimizer, + optimizer level */
	short	t_priority ;	/* Importance of including phase in scan */
	list_head t_inputs ;	/* The input 'path's of a combiner */
	char	*t_origname ;	/* The basename of the output file */
	trf	*t_next ;	/* The transformation to be executed next */
	char    *t_prog ;       /* Pathname for load file */
	list_head t_flags ;     /* List of flags */
	list_head t_args ;      /* List of arguments */
} ;

#define t_cont(elem) ((trf *)l_content(elem))

#endif