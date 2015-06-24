/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "assert.h"
#include <unistd.h>
#include <stdio.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "compute.h"
#include "utils.h"
#include "gencode.h"
#ifdef USE_TES
#include "mach.h"
#endif

FILE *codefile;

void out_init(char *filename)
{
#ifndef NDEBUG
	static char stderrbuff[BUFSIZ];

    if (Debug) {
	codefile = stderr;
	if (!isatty(2))
		setbuf(stderr,stderrbuff);
    } else {
#endif
	if (filename == (char *) 0)
		codefile = stdout;
	else
		if ((codefile=freopen(filename,"w",stdout))==NULL)
			error("Can't create %s",filename);
#ifndef NDEBUG
    }
#endif
}

void out_finish()
{
#ifndef NDEBUG
	if (Debug)
		fflush(stderr);
	else
#endif
		if (codefile) fclose(codefile);
#ifdef TABLEDEBUG
	termlset();
#endif
}

void tstoutput()
{
	if (ferror(codefile))
		error("Write error on output");
}

void genstr(int stringno)
{
	fputs(codestrings[stringno],codefile);
}

string ad2str(addr_t ad)
{
	static char buf[100];

	if (ad.ea_str==0)
		ad.ea_str="";
	if ((long)ad.ea_off==(long)0) {
		if(ad.ea_str[0]==0)
			return(mystrcpy("0")); /* don't return empty string */
		else
			return(mystrcpy(ad.ea_str));
	}
	sprintf(buf,"%s%c%ld",ad.ea_str,ad.ea_off>=0 ? '+' : ' ',(long)ad.ea_off);
	return(mystrcpy(buf));
}

void praddr(addr_t ad)
{
	if (ad.ea_str==0 || *(ad.ea_str) == '\0')
		fprintf(codefile,WRD_FMT,ad.ea_off);
	else {
		fputs(ad.ea_str, codefile);
		if (ad.ea_off<0) {
			putc('-', codefile);
			fprintf(codefile,WRD_FMT,-ad.ea_off);		
		}
		else if(ad.ea_off>0) {
			putc('+',codefile);
			fprintf(codefile,WRD_FMT,ad.ea_off);
		}
	}
}

void gennl()
{
	putc('\n',codefile);
}

void prtoken(token_p tp, int leadingchar)
{
	int c;
	char *code;
	tkdef_p tdp;

	putc(leadingchar,codefile);
	if (tp->t_token == -1) {
		fputs(codestrings[machregs[tp->t_att[0].ar].r_repr],codefile);
		return;
	}
	tdp = &tokens[tp->t_token];
	assert(tdp->t_format != -1);
	code = codestrings[tdp->t_format];
	while ((c = *code++) != 0) {
		if (c>=' ' && c<='~')
			putc(c,codefile);
		else {
			assert(c>0 && c<=TOKENSIZE);
			switch(tdp->t_type[c-1]) {
			default:
				assert(FALSE);
			case EV_INT:
				fprintf(codefile,WRD_FMT,tp->t_att[c-1].aw);
				break;
			case EV_ADDR:
				praddr(tp->t_att[c-1].aa);
				break;
			case EV_REG:
				fputs(codestrings[machregs[tp->t_att[c-1].ar].r_repr],codefile);
				break;
			}
		}
	}
}

#ifdef USE_TES
void printlabel(int labnum)
{
	newilb(dollar[labnum].e_v.e_addr.ea_str);
}
#endif
