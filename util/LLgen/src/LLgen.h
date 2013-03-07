#ifndef LLGEN_H
#define LLGEN_H

/* alloc.c */
p_mem alloc(unsigned size);
p_mem ralloc(p_mem p, unsigned size);
p_mem new_mem(p_info p);

/* check.c */
void conflchecks(void);
void prline(char *s);
void printset(p_set p, char * s);
int check(p_gram p);
void moreverbose(p_set t);
void prrule(p_gram p);
void cfcheck(p_set s1, p_set s2, int flag);
void resolve(p_gram p);
void propagate(p_set set, p_gram p);
void spaces(void);

/* compute.c */
typedef struct lngth {
			/* Structure used to compute the shortest possible
			 * length of a terminal production of a rule.
			 * In case of a tie, the second field is used.
			 */
	int cnt;
	int val;
} t_length, *p_length;

void do_compute(void);
void createsets(void);
void walk(p_set u, p_gram p);
void co_trans(int (*fc)(p_nont));
int nempty(p_nont p);
int empty(p_gram p);
int nfirst(p_nont p);
int first(p_set setp, p_gram p, int flag);
int nfollow(p_nont p);
int follow(p_set setp, p_gram p);
void co_dirsymb(p_set setp, p_gram p);
void co_others(p_gram p);
int ncomplength(p_nont p);
int nc_nfirst(p_nont p);
STATIC int nc_nfollow(p_nont p);
void do_lengthcomp(void);
void complength(p_gram p, p_length le);
void add(p_length a, int c, int v);
int compare(p_length a, p_length b);
void setdefaults(p_gram p);
void do_contains(p_nont n);
void contains(p_gram p, p_set set);
int nsafes(p_nont p);
int do_safes(p_gram p, int safe, int *ch);
int t_safety(int rep, int count, int persistent, int safety);
int t_after(int rep, int count, int outsafety);

/* gencode.c */
void doclose(FILE *f);
int *mk_tokenlist(void);
void genhdr(void);
void gencode(int argc);
void opentemp(char * str);
void geninclude(void);
void genrecovery(void);
void generate(p_file f);
void prset(p_set p);
void macro(char * s, p_nont n);
void controlline(void);
void getparams(void);
void genprototypes(p_file f);
void getansiparams(int mkdef);
int gettok(void);
void rulecode(p_gram p, int safety, int mustscan, int mustpop);
void alternation(p_gram pp, int safety, int mustscan, int mustpop, int lb);
void genncrecovery(void);
int *dopush(p_gram p, int safety, int toplevel, int **pp);
void getaction(int flag);
int codeforterm(p_term q, int safety, int toplevel);
void genswhead(p_term q, int rep_kind, int rep_count, int safety, int ispushed);
void gencases(int *tokenlist, int caseno, int compacted);
char * genname(char * s);
void genpush(int d);
void genincrdecr(char * s, int d);
void genpop(int d);
int analyze_switch(int *tokenlist);
void add_cases(p_set s, int *tokenlist, int caseno);
void out_list(int *tokenlist, int listno, int casecnt);
void genextname(int d, char *s, FILE *f);
void correct_prefix(void);

/* LLgen.c */
void newnorder(int index);
void newtorder(int index);
void p_init(void);
void LL0_spec(void);
void mkalt(p_gram prod, int condition, int lc, p_gram res);
void mkterm(p_gram prod, int flags, int lc, p_gram result);
p_gram copyrule(p_gram p, int length);

/* Lpars.c */
void LLparse(void);
void LLscan(int t);
void LLread(void);
void LLerror(int t);
void LLsafeerror(int t);
int LLnext(int n);
int LLskip(void);
void LLnewlevel(unsigned int *LLsinfo);
void LLoldlevel(unsigned int *LLsinfo);

/* machdep.c */
void UNLINK(char * x);
void RENAME(char * x, char * y);
char * libpath(char * s);

/* main.c */
int main(int argc, char *argv[]);
void readgrammar(int argc, char *argv[]);
void doparse(p_file p);
void error(int lineno, char *s, char *t, char *u);
void warning(int lineno, char *s, char *t, char *u);
void fatal(int lineno, char *s, char *t, char *u);
void comfatal(void);
void copyfile(char *file);
void install(char *target, char *source);

/* name.c */
void name_init(void);
p_entry newentry(char * str, p_entry next);
char * store(char * s);
int hash(char * str);
p_gram search(int type, char * str, int option);

/* reach.c */
void co_reach(void);
void reachable(p_nont p);
void reachwalk(p_gram p);

/* sets.c */
void setinit(int nt_needed);
p_set get_set(void);
p_set setalloc(void);
int setunion(p_set a, p_set b);
int setintersect(p_set a, p_set b);
void setminus(p_set a, p_set b);
int setempty(p_set p);
int findindex(p_set set);
int setcount(p_set set, int *saved);

/* tokens.c */
void copyact(char ch1, char ch2, int flag, int level);
int scanner(void);
int input(void);
void unput(int c);
void skipcomment(int flag);
char * vallookup(int s);
char * cpy(int s, char * p, int inserted);
void LLmessage(int d);

#endif /* LLGEN_H */
