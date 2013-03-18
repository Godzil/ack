/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTILS_ACK_ACK_H
#define UTILS_ACK_ACK_H

#include "trans.h"
#include "grows.h"

#ifndef NORCSID
#define RCS_ACK "$Id$"
#endif

/****************************************************************************/
/*                      User settable options                               */
/****************************************************************************/

#define FRONTENDS       "fe"    /* The front-end definitions */
#define TMPNAME         "Ack_%x"     /* Naming of temp. files */

/****************************************************************************/
/*         Internal mnemonics, should not be tinkered with                  */
/****************************************************************************/

/* The names of some string variables */

#define HOME    "EM"
#define RTS     "RTS"
#define HEAD    "HEAD"
#define TAIL    "TAIL"
#define SRC     "SOURCE"
#define LIBVAR	"LNAME"
#define SUFFIX	"SUFFIX"

/* Intended for flags, possibly in bit fields */

#define YES     (1)
#define NO      (0)
#define MAYBE   (2)

#define EXTERN  extern

#define SUFCHAR '.'             /* Start of SUFFIX in file name */
#define SPACE   ' '
#define TAB     '\t'
#define EQUAL   '='
#define S_VAR   '{'             /* Start of variable */
#define C_VAR   '}'             /* End of variable */
#define A_VAR   '?'             /* Variable alternative */
#define BSLASH  '\\'            /* Backslash */
#define STAR    '*'             /* STAR */
#define C_IN    '<'             /* Token specifying input */
#define C_OUT   '>'             /* Token specifying output */
#define S_EXPR  '('             /* Start of expression */
#define C_EXPR  ')'             /* End of expression */
#define M_EXPR  ':'             /* Middle of two suffix lists */
#define T_EXPR  '='             /* Start of tail */

#define NO_SCAN 0200            /* Bit set in character to defeat recogn. */

typedef struct {
	char    *p_path;        /* points to the full pathname */
	int     p_keeps:1;      /* The string should be thrown when unused */
	int     p_keep:1;       /* The file should be thrown away after use */
} path ;

#define p_cont(elem) ((path *)l_content(elem))

/* Return values of setpath() */
enum f_path { FP_OK, FP_NOMATCH, FP_NOPATH } ;

/* Own routines */
enum f_path getpath(trf **first);
enum f_path scan_end(trf **first);
void noodstop();
char *getvar(char *name);
char *keeps(char *str);
char *basename(char *string);
char *skipblank(char *str);
char *firstblank(char *str);
char *getcore(unsigned size);
char *changecore(char *ptr, unsigned int size);
#define freecore(area)  free(area)
growstring scanb(char *line);
growstring scanvars(char *line);
void throws(char *str);
void start_scan();
void scan_found();
void find_cpp();
void try(list_elem *f_scan, char *suffix);
void setlist(char *name);
void fuerror(const char *fmt, ...);
void transini();
int process(char *arg);
void setsvar(char *name, char *str);
void getmapflags(trf *phase);
void vprint(const char* fmt, ...);
void disc_inputs(trf *phase);
int startrf(trf *first);
void setpvar(char *name,char *(*rout)(void));
void l_add(list_head *header, char *string);
void error(const char *fmt, ...);
void do_Rflag(char *argp);
void werror(const char *fmt, ...);
void add_input(path *file, trf *phase);
void add_head(char *str);
void add_tail(char *str);
int mayprep();
int transform(trf *phase);
void fatal(const char* fmt, ...);
void l_clear(list_head *header);
int satisfy(trf *trafo, char *suffix);
int setfiles(trf *phase);
void disc_files(trf *phase);
int runphase(trf *phase);
void rmfile(path *file);
int mapflag(list_head *maplist, char *cflag);
void clr_noscan(char *str);
void l_throw(list_head *header);
int mapexpand(char *mapentry, char *cflag);
void prns(char *s);
void set_Rflag(char *argp);
void condit(growstring *line, list_head *fsuff, list_head *lsuff, char *tailval);
void doassign(char *line, char *star, int length);
void getcallargs(trf *phase);
void discardargs(trf *phase);
void quit(int code);
void rmtemps();
void intrf();
void open_in(char *name);
void close_in();
int getinchar();
int getln();
void keephead(char *suffix);
void varinit();
void vieuwargs(int argc, char *argv[]);
void firstarg(char *argp);
void block(trf *first);
void keeptail(char *suffix);
void scanneeds();
void setneeds(char *suffix, int tail);
int run_exec(trf *phase, char *prog);
void do_flush();
void file_final(path *file);


#define DEBUG	1	/* Allow debugging of Ack */

#ifndef DEBUG
#  define debug 0       /* To surprise all these 'if ( debug ) 's */
#else
extern  int debug ;
#endif

#endif /* UTILS_ACK_ACK_H */