/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/*
 * We need the S_EXT because we leave locals alone.
 */
#define ISUNDEFINED(n)	(((n)->on_type & (S_TYP | S_EXT)) == (S_UND | S_EXT))
#define ISABSOLUTE(n)	(((n)->on_type & (S_TYP | S_EXT)) == (S_ABS | S_EXT))
#define ISCOMMON(n)	(((n)->on_type & (S_COM | S_EXT)) == (S_COM | S_EXT))

#define mustsavelocal(name)	(!((name)->on_type & S_SCT))

/* archive.c */
void arch();
void arch2();

/* extract.c */
void extract();
void namerelocate(struct outname *name);

/* finish.c */
void finish();
void do_crs(struct outname *base, unsigned int count);


/* main.c */
bool setbit(int indx, char *string);
void addbase(struct outname *name);

/* output.c */
void endoutput();
void beginoutput();

/* error.c */
void stop();
void fatal(char *format, ...);
void warning(char *format, ...);
void error(char *format, ...);
int do_verbose(char *format, ...);

/* read.c */
void rd_fatal();

/* write.c */
void wr_fatal();
void begin_write();
void end_write();
void wrt_emit(char *emit, int sectindex, long cnt);
void wrt_nulls(int sectindex, long cnt);
void wrt_name(struct outname *name, int writename);

/* sym.c */
void init_symboltable();
struct outname *searchname(char *string, int hashval);
void entername(struct outname *name, int hashval);
unsigned int indexof(struct outname *name);
int hash(char *p);

/* save.c */
void savemagic();
void savehdr(struct ar_hdr * hdr);
ind_t savechar(int piece, ind_t off);
void savelocal(struct outname *name);

/* relocate.c */
void relocate(struct outhead *head, char *emit, struct outname names[], struct outrelo *relo, long off);

/* scan.c */
int getfile(char *filename);
void closefile(char *filename);
void get_archive_header(struct ar_hdr *archive_header);
void get_modul();
void seek(long pos);
void skip_modul(struct outhead *head);
void startrelo(struct outhead *head);
struct outrelo *nextrelo();
char *getemit(struct outhead *head, struct outsect *sects, int sectindex);
char *getblk(long totalsz, long *pblksz, int sectindex);
void endemit(char *emit);

/* --- */
