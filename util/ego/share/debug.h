/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* D E B U G G I N G   T O O L S */

#include "types.h"

/* TEMPORARY: */
/* #define DEBUG */

extern int		linecount;	/* # lines in this file */
extern bool		verbose_flag;  /* generate verbose output ? */

#ifdef __STDC__

/* VARARGS 1 */
void error(char *fmt, ...);

# ifdef TRACE
void OUTTRACE(char *s, ...);
# else /* TRACE */
#  define OUTTRACE(s,n)
# endif /* TRACE */

# ifdef VERBOSE
void OUTVERBOSE(char *s, ...);
# else /* VERBOSE */
#  define OUTVERBOSE(s,n1,n2)
# endif /* VERBOSE */

#else /* __STDC__ */

/* VARARGS 1 */
extern error();

# ifdef TRACE
extern OUTTRACE();
# else /* TRACE */
#  define OUTTRACE(s,n)
# endif /* TRACE */

# ifdef VERBOSE
extern OUTVERBOSE();
# else /* VERBOSE */
#  define OUTVERBOSE(s,n1,n2)
# endif /* VERBOSE */

#endif /* __STDC__ */

#ifdef DEBUG

/* Some (all?) Unix debuggers don't particularly like
 * static procedures and variables. Therefor we make everything
 * global when debugging.
 */

#define STATIC

#define assert(x) if(!(x)) badassertion(__FILE__,__LINE__)

extern VI();
extern VL();
extern VD();
extern VA();
extern VO();
extern VP();



#else /*DEBUG*/

#define assert(b)

#define VI(i)
#define VL(l)
#define VD(d)
#define VA(a)
#define VO(o)
#define VP(p)


#define STATIC static
#endif
