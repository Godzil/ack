/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm1.h	1.5 */
/*
 * variable declarations
 */

#ifdef extern
#define	INIT(x)		= x
#else
#define	INIT(x)		/* empty */
#endif

extern short	pass INIT(PASS_1);
				/* PASS 1, 2 or 3 */
extern short	peekc;		/* push back symbol (PASS_1) */
extern short	unresolved;	/* number of unresolved references */
extern long	lineno;		/* input line number */
extern short	hllino;		/* high-level language line number */
extern short	nerrors;	/* terminate at end of pass if set */
extern short	sflag INIT(SYM_DEF);
				/* -s option (symbol table info) */
extern char	*progname;	/* for error messages */
extern char	*modulename;	/* for error messages */
extern common_t	*commons;	/* header of commons list */

#ifdef ASLD
extern short	archmode;	/* scanning archives */
extern long	archsize;	/* size of individual archive entries */
#else
extern short	uflag;		/* if 1 make undefineds extern */
				/* symbol table index for last S_UND */
#endif

#ifdef LISTING
extern short	dflag;		/* -d option (list mode) */
#endif

#ifdef ASLD
#ifdef RELOCATION
extern short	rflag;		/* -r option (relocation info) */
#endif /* RELOCATION */
#else
#define	rflag	1
extern valu_t	relonami;
#endif /* ASLD */

#ifdef THREE_PASS
extern short	bflag;		/* -b option (no optimizations) */
#endif

extern char	*aoutpath INIT("a.out");
extern char	temppath[50];

extern FILE	*input;
extern FILE	*tempfile;

extern char	*stringbuf;	/* contains last string value */
extern int	stringlen;	/* contains length of last string value */

extern sect_t	sect[SECTMAX];

/*
 * specials for the location counter
 */
extern sect_t	*DOTSCT;	/* &sect[DOTTYP-S_MIN] or NULL */
extern ADDR_T	DOTVAL;		/* DOTSCT->s_size + DOTSCT->s_base */
extern short	DOTTYP;		/* S_MIN..S_MAX or S_UND */

extern unsigned short	nname;		/* Counts name table index in PASS_3 */

extern item_t	*hashtab[H_TOTAL];
extern short	hashindex;	/* see item_search() */

extern item_t	*fb_ptr[4*FB_SIZE];

#ifdef THREE_PASS
#define BITCHUNK	(8 * MEMINCR)
extern int	nbits;
extern int	bitindex;	/* bitindex * MEMINCR * 8 + nbits gives
				   number of decisions so far
				*/
#endif

#ifdef LISTING
extern short	listmode;	/* -d option for current pass */
extern short	listtemp;	/* listmode if .list seen */
extern short	listflag;	/* copied from listtemp at '\n' */
extern short	listcolm;	/* column on output */
extern short	listeoln INIT(1);
				/* set by endline, tested by emit1 */
extern FILE	*listfile;	/* copy of source text */
extern char	listpath[50];
#endif

#ifndef extern
extern item_t		keytab[];
extern struct outhead	outhead;
#endif

extern int	curr_token;

/* forward function declarations */
int yyparse();

/* ========== comm4.c prototypes =========== */
void stop(int signal);
int main(int argc, char *argv[]);
int pass_1(int argc, char *argv[]);
void parse(char *s);
void pass_23(int n);
void newmodule(char *s);
void setupoutput(void);
void commfinish(void);

/* ========== comm5.c prototypes =========== */
int yylex(void);
void putval(int c);
int getval(int c);
int nextchar(void);
void readcode(int n);
int induo(int c);
int inident(int c);
int innumber(int c);
int instring(int termc);
int inescape(void);
int infbsym(char *p);
int hash(char *p);
item_t *item_search(char *p);
void item_insert(item_t *ip, int h);
item_t *item_alloc(int typ);
item_t *fb_alloc(int lab);
item_t *fb_shift(int lab);
#ifdef ASLD
char *readident(int c);
#endif

/* ========== comm6.c prototypes =========== */
void newequate(item_t *ip, int typ);
void newident(item_t *ip, int typ);
void newlabel(item_t *ip);
void newsect(item_t *ip);
void newbase(valu_t base);
void newcomm(item_t *ip, valu_t val);
void switchsect(int newtyp);
void align(valu_t bytes);
long new_string(char *s);
void newsymb(char *name, int type, int desc, valu_t valu);
void new_common(item_t *ip);
#ifdef RELOCATION
void newrelo(int s, int n);
#endif

/* ========== comm7.c prototypes =========== */
valu_t load(item_t *ip);
int store(item_t *ip, valu_t val);
char *remember(char *s);
int combine(int typ1, int typ2, int op);
int small(int fitsmall, int gain);
void emit1(int arg);
void emit2(int arg);
void emit4(long arg);
void emitx(valu_t val, int n);
void emitstr(int zero);
void ffreopen(char *s, FILE *f);
FILE *ffcreat(char *s);
FILE *fftemp(char *path, char *tail);
void yyerror(char *str);
int printx(int ndig, valu_t val);
void nosect(void);
void wr_fatal(void);
void fatal(char *s, ...);
void assert1(void);
void serror(char *s, ...);
void warning(char *s, ...);
void diag(char *tail, char *s, ...);
void nofit(void);
void listline(int textline);

/* ========== Machine dependent C declarations ========== */

#include	"mach1.c"
