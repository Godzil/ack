/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTILS_ACK_GROWS_H
#define UTILS_ACK_GROWS_H

#ifndef NORCSID
#define RCS_GROWS "$Id$"
#endif

/* struct used to identify and do bookkeeping for growing strings */

typedef struct {
	char            *gr_string ;    /* Points to start of string */
	unsigned        gr_size ;       /* Current string size */
	unsigned        gr_max ;        /* Maximum string size */
} growstring ;

#define GR_MORE         50      /* Steps to grow */

#define gr_start(id)    (id).gr_string  /* The start of the string */

/* Routines used */

void gr_throw(growstring *id);             /* To free the core */
void gr_add(growstring *id, char c);       /* To add one character */
void gr_cat(growstring *id, char *string); /* concatenate the contents and the string */
void gr_init(growstring *id);              /* Initialize the bookkeeping */
char *gr_final(growstring *id);            /* Transform to a stable storage string */

#endif /* UTILS_ACK_GROWS_H */