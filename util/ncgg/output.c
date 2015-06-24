/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* #define CODEDEBUG */	/* print readable code */
#ifdef CODEDEBUG
int	code_in_c=0;	/* put readable code in "code" */
int	tabledebug=1;	/* generate code for table debugging */
#else
int	code_in_c=1;	/* put code in "tables.c" */
int	tabledebug=0;	/* do not generate code for table debugging */
#endif
int	verbose=0;	/* print all statistics */
int	use_tes;	/* use top element size information */
char	*c_file=	"tables.c";
char	*h_file=	"tables.H";
char	*cd_file=	"code";

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "assert.h"
#include "varinfo.h"
#include "param.h"
#include "reg.h"
#include "property.h"
#include "token.h"
#include "set.h"
#include "instruct.h"
#include "lookup.h"
#include "error.h"
#include "strlookup.h"
#include "subr.h"
#include <cgg_cg.h>
#include "pseudo.h"
#include "regvar.h"
#include "extern.h"

/* Since isascii is not standard, as c89 or C99, privide another method */
#define IsAscii(_c) (((_c) & ~0x7f) == 0)

#define BMASK 0xFF
#define BSHIFT 8

FILE *ctable,*htable;
FILE *code;
short *lineset;
int maxline;

extern void *beg_sbrk;

extern int nstrings;
extern char *l_strings[];

extern int ninstances;
extern inst_t l_instances[];

extern int nmoves;
extern move_t l_moves[];
extern int ntests;
extern test_t l_tests[];
extern int nstacks;
extern c1_t l_stacks[];
extern int ncoercs;
extern c3_t l_coercs[];
extern int nsplit,maxsplit;
extern c2_t l_split[];
extern set_t l_sets[];

int maxallreg=0;
int maxregvars=0;
int setsize;

void pat(int n);
void patbyte(int n);
void patshort(int n);

void opnfile(FILE **f, char *s)
{
	if ((*f=fopen(s,"w"))==NULL)
		fatal("Can't create %s",s);
}

void unlfile(FILE *f, char *s)
{
	if (f) fclose(f);
	if (unlink(s)<0)
		error("%s incorrect, must be removed!!",s);
}

void initio()
{
	opnfile(&ctable,c_file);
	opnfile(&htable,h_file);
	if (code_in_c)
		fprintf(ctable,"char coderules[] = {");
	else
		opnfile(&code,cd_file);
	patbyte(0);
	if (tabledebug)
		lineset = (short *) myalloc(SZOFSET(MAXSOURCELINES)*sizeof(short));
}

void finishcode()
{
	if (code_in_c)
		fprintf(ctable,"\n};\n\n");
	fprintf(ctable, "int allsetno = %d;\n", allsetno);
	if (tabledebug) {
		int fd;
		int sz;
		
		if ((fd=creat("lineset",0666))>=0) {
			sz = SZOFSET(maxline)*2;
			write(fd,&sz,sizeof(int));
			write(fd,lineset,sz);
			close(fd);
		} else
			error("Can't create lineset");
	}
}

void errorexit()
{
	unlfile(ctable,c_file);
	unlfile(htable,h_file);
	if (!code_in_c)
		unlfile(code,cd_file);
}

#ifdef CODEDEBUG
#define code8(x) fprintf(code,"%s","x")
#define code8nl(x) fprintf(code,"%s\n","x")
#define code53(x,y) fprintf(code,"%s-%d","x",y)
#define codeint(x) fprintf(code," %d",x)
#define codenl() fprintf(code,"\n")
#else /* CODEDEBUG */
#define codenl()
#define code8nl(x) code8(x)

void code8(int x)
{
	codeindex++;
	if (code_in_c)
		fprintf(ctable,"%d,",x&0377);
	else
		putc(x,code);
}

void code53(int x, int y)
{
	code8(x+(y<<5));
}

void codeint(int x)
{
	assert(x>=0 && x<=32767);
	if (x<128) {
		code8(x);
	} else {
		code8(x/256+128);
		code8(x%256);
	}
}

#endif /* CODEDEBUG */

int prevind=0;
int npatbytes= -1;
char pattern[MAXPATBYTES];
int pathash[256];

void outpatterns()
{
	extern int npatterns;
	extern int patindex[];
	extern int empatlen;
	extern int emmnem[];
	extern int empatexpr;
	int i;

	if (!inproc) {
		patbyte(0);
		patshort(prevind);
		prevind = npatbytes-2;
		patbyte(empatlen);
		for(i=0;i<empatlen;i++)
			patbyte(emmnem[i]);
		pat(empatexpr);
	}
	if (callproc==0) {
		patbyte(npatterns);
		for(i=0;i<npatterns;i++)
			pat(patindex[i]);
	} else {
		patbyte(0);
		pat(callproc);
		pat(nprocargs);
		for (i = 0; i < nprocargs; i++) pat(procarg[i]);
	}
}

void pat(int n)
{
	assert(n>=0);
	if (n<128)
		patbyte(n);
	else {
		patbyte(n/256+128);
		patbyte(n%256);
	}
}

void patshort(int n)
{
	patbyte(n%256);
	patbyte(n/256);
}

void patbyte(int n)
{
	NEXT(npatbytes, MAXPATBYTES, "Pattern bytes");
	pattern[npatbytes]=n;
}

void hashpatterns()
{
	short index;
	char *bp,*tp;
	short i;
	short hashvalue;
	int patlen;

	index = prevind;
	while (index != 0) {
		bp = &pattern[index];
		tp = &bp[PO_MATCH];
		i = *tp++&BMASK;
		if (i==BMASK) {
			i = *tp++&BMASK;
			i |= (*tp++&BMASK)<<BSHIFT;
		}
		patlen = i;
		hashvalue = 0;
		switch(patlen) {
		default:        /* 3 or more */
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 2:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 1:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		}
		assert(hashvalue!= ILLHASH);
		i=index;
		index = (bp[PO_NEXT]&BMASK)|(bp[PO_NEXT+1]<<BSHIFT);
		bp[PO_HASH] = hashvalue>>BSHIFT;
		hashvalue &= BMASK;
		bp[PO_NEXT] = pathash[hashvalue]&BMASK;
		bp[PO_NEXT+1] = pathash[hashvalue]>>BSHIFT;
		pathash[hashvalue] = i;
	}
}

void outincludes()
{
	fprintf(ctable,"#include \"param.h\"\n");
	fprintf(ctable,"#include \"tables.h\"\n");
	fprintf(ctable,"#include \"types.h\"\n");
	fprintf(ctable,"#include <cgg_cg.h>\n");
	fprintf(ctable,"#include \"data.h\"\n");
}

void outregs()
{
	int i,j,k;
	short rset[SZOFSET(MAXREGS)];
	short clashlist[MAXREGS*MAXREGS];
	int iclashlist = 0;
	int t,ready;


	fprintf(ctable,"char stregclass[] = {\n");
	for (i=0;i<nregs;i++)
		fprintf(ctable,"\t%d,\n",l_regs[i].ri_class);
	fprintf(ctable,"};\n\nstruct reginfo machregs[] = {\n{0},\n");
	for (i=1;i<nregs;i++) {
		fprintf(ctable,"{%d,%d",strlookup(l_regs[i].ri_repr),
			l_regs[i].ri_size);
		if (maxmembers!=0) {
			fprintf(ctable,",{");
			for(j=0;j<maxmembers;j++)
				fprintf(ctable,"%d,",l_regs[i].ri_memb[j]);
			/* now compute and print set of registers
			 * that clashes with this register.
			 * A register clashes with al its children (and theirs)
			 * and with all their parents.
			 */
			for (j=0;j<SZOFSET(MAXREGS);j++)
				rset[j]=0;
			BIS(rset,i);
			do {
			    ready=1;
			    for (j=1;j<nregs;j++)
				if (BIT(rset,j))
				    for (k=0;k<2;k++)
					if ((t=l_regs[j].ri_memb[k])!=0) {
					    if (BIT(rset,t)==0)
						ready=0;
					    BIS(rset,t);
					}
			} while (!ready);
			do {
			    ready=1;
			    for (j=1;j<nregs;j++)
				for (k=0;k<2;k++)
				    if ((t=l_regs[j].ri_memb[k])!=0)
					if (BIT(rset,t)) {
						if (BIT(rset,j)==0)
						    ready=0;
						BIS(rset,j);
					}
			} while (!ready);
			fprintf(ctable,"},{");
			for (j=0;j<SZOFSET(nregs);j++)
				fprintf(ctable,"0%o,",rset[j]&0xFFFF);
			fprintf(ctable,"}, %d", iclashlist);
			for (j = 1; j < nregs; j++) {
				if (BIT(rset, j)) clashlist[iclashlist++] = j;
			}
			clashlist[iclashlist++] = 0;
		}
		fprintf(ctable,",%d",l_regs[i].ri_rregvar>=0);
		fprintf(ctable,"},\n");
	}
	fprintf(ctable,"};\n\n short clashlist[] = {\n\t");
	for (i = 0; i < iclashlist; i++) {
		fprintf(ctable, "%d, ", clashlist[i]);
		if (clashlist[i] == 0) fprintf(ctable, "\n\t");
	}
	fprintf(ctable, "0};\n\n");
}

void outregvars()
{
	int i,j;

	fprintf(htable,"#define REGVARS\n");
	fprintf(ctable,"#include \"regvar.h\"\n");
	fprintf(ctable,"int nregvar[4] = { ");
	for (i=0;i<4;i++) {
		fprintf(ctable,"%d, ",nregvar[i]);
		if (nregvar[i]>maxregvars)
			maxregvars = nregvar[i];
	}
	fprintf(ctable,"};\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(ctable,"struct regassigned ratar%d[%d];\n",
					i,nregvar[i]);
	for (i=0;i<4;i++) if (nregvar[i]>0) {
		fprintf(ctable,"int rvtar%d[] = {",i);
		for (j=0;j<nregvar[i];j++)
			fprintf(ctable,"%d,",rvnumbers[i][j]);
		fprintf(ctable,"};\n");
	}
	fprintf(ctable,"\nint *rvnumbers[] = {\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(ctable,"\trvtar%d,\n",i);
		else
			fprintf(ctable,"\t0,\n");
	fprintf(ctable,"};\n\nstruct regassigned *regassigned[] = {\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(ctable,"\tratar%d,\n",i);
		else
			fprintf(ctable,"\t0,\n");
	fprintf(ctable,"};\n");
}

int typeconv(int n)
{
	if (n>=0) return(2);
	if (n== -1) return(1);
	if (n== -2) return(3);
	assert (n== -3);
	return(0);
}

void outfmt(char *p)
{
	register int c;
	fprintf(ctable,"\"");
	while ((c= (*p++&0377))!=0) {
		if (! IsAscii(c) || iscntrl(c)) {
			fprintf(ctable,"\\%c%c%c",
				((c&~0300)>>6) + '0', ((c&070)>>3)+'0',
				(c&07)+'0');
		}
		else fprintf(ctable, "%c",c);
	}
	fprintf(ctable,"\"");
}

void outtokens()
{
	int tokno,i;
	token_p tp;

	fprintf(ctable,"tkdef_t tokens[] = {{0},\n");
	for (tokno=1;tokno<ntokens;tokno++) {
		tp = l_tokens[tokno];
		fprintf(ctable,"/* %3d */{%d,{%d,%d},{", tokno,
			tp->tk_size, tp->tk_cost.ct_space, tp->tk_cost.ct_time);
		for(i=0;i<maxtokensize;i++)
			fprintf(ctable,"%d,",typeconv(tp->tk_att[i].ta_type));
		fprintf(ctable,"},%d},\t/* ",tp->tk_format);
		if (tp->tk_format >= 0) outfmt(l_strings[tp->tk_format]);
		else fprintf(ctable, "(no format)");
		fprintf(ctable," */\n");
	}
	fprintf(ctable,"{0}};\n\n");
}

void outenodes()
{
	node_p np;
	extern node_t nodes[];
	extern int nnodes;

	fprintf(ctable,"node_t enodes[] = {\n");
	for (np=nodes;np<&nodes[nnodes];np++)
		fprintf(ctable,"{%d,%d,%d},\n",
			np->ex_operator,np->ex_lnode,np->ex_rnode);
	fprintf(ctable,"};\n\n");
}

void outstrings()
{
	int i;
#if 0
	char *p;
	int c;
#endif
	extern char * filename;

	if (tabledebug)
		fprintf(ctable,"char *tablename = \"%s\";\n",filename);
	fprintf(ctable,"string codestrings[] = {\n");
	for(i=0;i<nstrings;i++) {
		fprintf(ctable,"\t");
		outfmt(l_strings[i]);
#if 0
		while ((c= (*p++&0377))!=0) {
			if (! IsAscii(c) || iscntrl(c)) {
				fprintf(ctable,"\\%c%c%c",
					((c&~0300)>>6) + '0', ((c&070)>>3)+'0',
					(c&07)+'0');
			}
			else fprintf(ctable, "%c",c);
		}
		fprintf(ctable,"\",\n");
#endif
		fprintf(ctable,",\n");
	}
	fprintf(ctable,"};\n\n");
}

extern set_t unstackset;

void outsets()
{
	int i;
	set_p sp;

	fprintf(ctable,"set_t machsets[] = {\n");
	for (sp=l_sets;sp< &l_sets[nsets]; sp++) {
		fprintf(ctable,"/* %3ld */ {%3d,{",(long)(sp-l_sets),sp->set_size);
		for (i=0;i<setsize;i++)
			fprintf(ctable,"0x%x,",sp->set_val[i]&0xFFFF);
		fprintf(ctable,"}},\n");
	}
	fprintf(ctable,"};\n\n");

	fprintf(ctable, "set_t unstackset = { %3d,{\n", unstackset.set_size);
	for (i = 0; i<setsize;i++)
		fprintf(ctable,"0x%x,",unstackset.set_val[i]&0xFFFF);
	fprintf(ctable,"}};\n\n");
}

void outinstances()
{
	inst_p ip;
	int i;

	fprintf(ctable,"inst_t tokeninstances[] = {\n");
	for (ip=l_instances;ip< &l_instances[ninstances]; ip++) {
		fprintf(ctable,"{ %d, {",ip->in_which);
		for(i=0;i<=maxtokensize;i++)
			fprintf(ctable,"%d,",ip->in_info[i]);
		fprintf(ctable,"}},\n");
	}
	fprintf(ctable,"};\n\n");
}

void outmoves()
{
	move_p mp;

	fprintf(ctable,"move_t moves[] = {\n");
	for (mp=l_moves; mp< &l_moves[nmoves]; mp++)
		fprintf(ctable,"{%d,%d,%d,%d,%d},\n",
			mp->m_set1, mp->m_expr1,
			mp->m_set2, mp->m_expr2,
			mp->m_cindex);
	fprintf(ctable,"{-1}\n};\n\n");
}

void outtests()
{
	test_p tp;

	fprintf(ctable,"test_t tests[] = {\n");
	for (tp=l_tests; tp< &l_tests[ntests]; tp++)
		fprintf(ctable,"{%d,%d,%d},\n",
			tp->t_set, tp->t_expr,
			tp->t_cindex);
	fprintf(ctable,"{-1}\n};\n\n");
}

void outstacks()
{
	c1_p cp;

	fprintf(ctable,"c1_t c1coercs[] = {\n");
	for (cp=l_stacks; cp< &l_stacks[nstacks]; cp++)
		fprintf(ctable,"{%d,%d,%d,%d},\n",
			cp->c1_texpno, cp->c1_expr,
			cp->c1_prop, cp->c1_codep);
	fprintf(ctable,"{-1}\n};\n\n");
}

void outsplits()
{
	c2_p cp;
	int i;

	fprintf(ctable,"c2_t c2coercs[] = {\n");
	for (cp=l_split; cp< &l_split[nsplit]; cp++) {
		fprintf(ctable,"{%d,%d,%d,{",
			cp->c2_texpno, cp->c2_expr, cp->c2_nsplit);
		for (i=0;i<maxsplit;i++)
			fprintf(ctable,"%d,",cp->c2_repl[i]);
		fprintf(ctable,"},%d},\n",cp->c2_codep);
	}
	fprintf(ctable,"{-1}\n};\n\n");
}

void outcoercs()
{
	c3_p cp;

	fprintf(ctable,"c3_t c3coercs[] = {\n");
	for (cp=l_coercs; cp< &l_coercs[ncoercs]; cp++)
		fprintf(ctable,"{%d,%d,%d,%d,%d},\n",
			cp->c3_texpno, cp->c3_expr,
			cp->c3_prop, cp->c3_repl, cp->c3_codep);
	fprintf(ctable,"{-1}\n};\n\n");
}

void outproplists()
{
	int propno;
	int regno;

	for(propno=0;propno<nprops;propno++) {
		fprintf(ctable,"struct reginfo *rlist%d[] = {\n",propno);
		for(regno=1;regno<nregs;regno++)
			if (BIT(l_props[propno].pr_regset,regno))
				fprintf(ctable,"&machregs[%d],\n",regno);
		fprintf(ctable,"0\n};\n");
	}
	fprintf(ctable,"struct reginfo **reglist[] = {\n");
	for(propno=0;propno<nprops;propno++)
		fprintf(ctable,"rlist%d,\n",propno);
	fprintf(ctable,"};\n\n");
}

void outconsts()
{
	fprintf(ctable,"unsigned cc1 = %u;\n",fc1);
	fprintf(ctable,"unsigned cc2 = %u;\n",fc2);
	fprintf(ctable,"unsigned cc3 = %u;\n",fc3);
	fprintf(ctable,"unsigned cc4 = %u;\n",fc4);
}

void cdef(char *s, int n)
{
	fprintf(htable,"#define %s %d\n",s,n);
}

void passon(char *s)
{
	char buf[32];

	sprintf(buf,"T%s",s);
	cdef(buf,cmustbeset(s));
}

void outdefs()
{
	symbol *sy_p;
	extern int maxempatlen,maxrule;
	char *wrdfmt;

	passon("EM_WSIZE");
	passon("EM_PSIZE");
	passon("EM_BSIZE");
	if ((sy_p=lookup("FORMAT",symsconst,justlooking))!=0) {
		wrdfmt = l_strings[sy_p->sy_value.syv_stringno];
		fprintf(htable,"#define WRD_FMT \"%s\"\n",wrdfmt);
	}
	cdef("MAXALLREG",maxallreg);
	cdef("SETSIZE",setsize);
	cdef("NREGS",nregs);
	cdef("REGSETSIZE",SZOFSET(nregs));
	cdef("TOKENSIZE",maxtokensize);
	cdef("MAXMEMBERS",maxmembers);
	cdef("LONGESTPATTERN",maxempatlen);
	cdef("MAXPATLEN",maxtokpatlen);
	cdef("MAXREPLLEN",maxtokrepllen);
	cdef("MAXEMREPLLEN",maxemrepllen);
	cdef("MAXPROCARG",maxprocargs);
	cdef("MAXRULE",maxrule<16 ? 16 : maxrule);
	if (nsplit>0) {
		cdef("MAXSPLIT",maxsplit);
	}
	if (tabledebug)
		cdef("TABLEDEBUG",1);
	if (use_tes)
		cdef("USE_TES",1);
}

void outars()
{
	int i;
	
	if (code_in_c)
		fprintf(htable,"#define CODEINC 1\n");
	else {
		fprintf(ctable,"char coderules[%d];\n",codeindex);
		fprintf(ctable,"int ncodebytes=%d;\n",codeindex);
	}
	fprintf(ctable,"char pattern[%d]={\n",npatbytes+1);
	for(i=0;i<=npatbytes;i++) {
		fprintf(ctable,"%d,%c",pattern[i]&BMASK,i%16==15 ? '\n' : ' ');
	}
	fprintf(ctable,"};\n\n");
	fprintf(ctable,"int pathash[256]={\n");
	for(i=0;i<256;i++) {
		fprintf(ctable,"%d,%c",pathash[i]&0xFFFF,i%10==9 ? '\n' : ' ');
	}
	fprintf(ctable,"};\n");
}

void finishio() 
{
	int nregs = 0;

	finishcode();
	hashpatterns();
	setsize = SZOFSET(nregs+ntokens);
	outdefs();
	outincludes();
	outregs();
	outtokens();
	outenodes();
	outstrings();
	outsets();
	outinstances();
	outmoves();
	outtests();
	outstacks();
	if (nsplit>0)
		outsplits();
	outcoercs();
	outproplists();
	outconsts();
	if (rvused)
		outregvars();
	outars();
}

void codecoco(cocono)
{
	if (cocono== -1)
		return;
	code8(DO_SETCC);
	codeint(cocono);
	codenl();
}

void dopattern(int stackcoerc, varinfo *kills, varinfo *allocates, varinfo *generates, varinfo *yields, varinfo *leaving)
{
	int i;
	int n,nops;
	struct varinfo *vp,*vivp;
	instr_p instp;
	int al,deal;
	int vil;
	int cocono= -1;
	cost_t totcost;
	int nremoves;
	int removelist[100];
	static char tlab[] = "0:";
	extern int optexact,optstack,startline;
	extern char *filename;
	extern int lineno;

#ifdef CODEDEBUG
	fprintf(code,"Code(%d) at \"%s\", line %d\n",stackcoerc,filename,lineno);
#endif
	if (code_in_c)
		fprintf(ctable,"\n/* \"%s\", line %d */ ",filename,lineno);
	if (tabledebug) {
		code8(DO_DLINE);
		codeint(startline); 
		codenl();
		if (startline<MAXSOURCELINES) {
			if (startline>maxline)
				maxline=startline;
			BIS(lineset,startline);
		} else {
			static int beenhere=0;

			if (!beenhere) {
				beenhere++;
				error("Too many source lines for table debug");
			}
		}
	}
	/* MATCH part */
	if (tokpatlen) {
		if (optexact)
			if (optstack)
				code53(DO_XXMATCH,tokpatlen);
			else
				code53(DO_XMATCH,tokpatlen);
		else
			code53(DO_MATCH,tokpatlen);
		for (i=0;i<tokpatlen;i++)
			codeint(tokpatset[i]);
		codenl();
	} else if (stackcoerc)
		code8nl(DO_COERC);
	if (optstack) {
		code53(DO_TOSTACK,0);
		codeint(allsetno);
		codenl();
	}
	/* The kills */
	for (vp=kills;vp!=0;vp=vp->vi_next) {
		if (vp->vi_int[1] != 0) {
			code53(DO_REMOVE,1);
			codeint(vp->vi_int[0]);
			codeint(vp->vi_int[1]);
		} else if (vp->vi_int[0] >= 0) {
			code53(DO_REMOVE,0);
			codeint(vp->vi_int[0]);
		} else {
			code8(DO_KILLREG);
			codeint(-vp->vi_int[0] - 1);
		}
		codenl();
	}
	nremoves=0;
	for(vp=generates;vp!=0;vp=vp->vi_next) {
		if (vp->vi_int[0] != INSREMOVE)
			continue;
		for(i=0;i<nremoves;i++)
			if (vp->vi_int[1]==removelist[i])
				break;
		if (i==nremoves) {
			assert(nremoves<(sizeof(removelist)/sizeof(int)));
			removelist[nremoves++] = vp->vi_int[1];
		}
	}
	for(i=0;i<nremoves;i++) {
		code8(DO_RREMOVE);
		codeint(removelist[i]);
		codenl();
	}
	/* allocate part */
	deal=0;al=0;
	for (vp=allocates;vp!=0;vp=vp->vi_next) {
		if (vp->vi_int[0] == -1) { /* Deallocate */
			deal++;
			code8(DO_DEALLOCATE);
			codeint(vp->vi_int[1]);
			codenl();
		} else {
			if (vp->vi_int[1]==0) {
				code53(DO_ALLOCATE,0);
				codeint(vp->vi_int[0]);
				codenl();
			} else {
				code53(DO_ALLOCATE,1);
				codeint(vp->vi_int[0]);
				codeint(vp->vi_int[1]);
				codenl();
			}
			al++;
		}
	}
	if (deal)
		code8nl(DO_REALLOCATE);
	if (al>maxallreg)
		maxallreg=al;
	totcost.ct_space = 0;
	totcost.ct_time  = 0;
	for(vp=generates;vp!=0;vp=vp->vi_next) {
		n= vp->vi_int[0];
		switch(n) {
		default:
			assert(n>=0);
			instp = &l_instr[n];
			nops=instp->i_nops;
			code53(DO_INSTR,nops);
			if (vp->vi_int[1]==0) {
				codeint(instp->i_asname);
			} else {
				codeint(10000+vp->vi_int[1]);
			}
			vivp=vp->vi_vi;
			for(i=0;i<nops;i++) {
				codeint(vivp->vi_int[0]);
				vivp = vivp->vi_vi;
			}
			codenl();
			totcost.ct_space += instp->i_cost.ct_space;
			totcost.ct_time  += instp->i_cost.ct_time ;
			break;
		case INSREMOVE:
			break;
		case INSMOVE:
			codecoco(cocono);
			code8(DO_MOVE);
			codeint(vp->vi_int[1]);
			codeint(vp->vi_int[2]);
			codenl();
			break;
		case INSTEST:
			codecoco(cocono);
			code8(DO_TEST);
			codeint(vp->vi_int[1]);
			codenl();
			break;
		case INSPRETURN:
			code8(DO_PRETURN);
			codenl();
			break;
		case INSTLAB:
			cocono = 0;
			tlab[0] = vp->vi_int[1] + '0';
			code53(DO_INSTR,0);
			codeint(strlookup(tlab));
			codenl();
			break;
		case INSSETCC:
			cocono=vp->vi_int[1];
			break;
		case INSERASE:
			code8(DO_ERASE);
			codeint(vp->vi_int[1]);
			codenl();
			break;
		case INSLABDEF:
			cocono = 0;
			code8(DO_LABDEF);
			codeint(vp->vi_int[1]);
			codenl();
			break;
		}
	}
	codecoco(cocono);
	vil = vilength(yields);
	if (vil!=0 || tokpatlen!=0 || allocates!=0) {
		code53(DO_TOKREPLACE,vilength(yields));
		for(vp=yields;vp!=0;vp=vp->vi_next) {
			codeint(vp->vi_int[0]);
		}
		codenl();
	}
	if (leaving!=0) {
		code53(DO_EMREPLACE,vilength(leaving));
		while (leaving!=0) {
			codeint(leaving->vi_int[0]);
			codeint(leaving->vi_int[1]);
			leaving = leaving->vi_next;
		}
		codenl();
	}
	if (totcost.ct_space!=0 || totcost.ct_time!=0) {
		code8(DO_COST);
		codeint(totcost.ct_space);
		codeint(totcost.ct_time);
		codenl();
	}
	if (empatlen==0 && !inproc)
		code8nl(DO_RETURN);
	else
		code8nl(DO_NEXTEM);
}

void used(char *resource, int use, int max)
{
	if (verbose || 4*use > 3*max)
		fprintf(stderr,"%s %d(%d)\n",resource,use,max);
}

void statistics()
{
	extern int nnodes, maxempatlen,maxrule;

	used("Registers",nregs,MAXREGS);
	used("Properties",nprops,MAXPROPS);
	used("Tokens",ntokens,MAXTOKENS);
	used("Tokensize",maxtokensize,MAXATT);
	used("Sets",nsets,MAXSETS);
	used("Instructions",ninstr,MAXINSTR);
	used("Strings",nstrings,MAXSTRINGS);
	used("Exp-nodes",nnodes,MAXNODES);
	used("EM-pat length",maxempatlen,EMPATMAX);
	used("rules/EM-pattern",maxrule,MAXPATTERNS);
	used("Allocates/rule",maxallreg,MAXALLREG);
	used("Instances",ninstances,MAXINSTANCES);
	used("Moves",nmoves,MAXMOVES);
	used("Tests",ntests,MAXTESTS);
	used("Stacks",nstacks,MAXSTACKS);
	used("1->1 Coercions",ncoercs,MAXCOERCS);
	used("Splitting coercions",nsplit,MAXSPLCOERC);
	used("Register variables",maxregvars,MAXREGVAR);
	used("Pat bytes",npatbytes+1,MAXPATBYTES);
	if (tabledebug)
		used("Source lines",maxline,MAXSOURCELINES);
	fprintf(stderr,"%ldK heap used\n",((long) (sbrk(0)-beg_sbrk+1023))/1024);
}
