
#ifndef NORCSID
static char rcsid[] = "$Header$" ;
#endif

con_part(sz, w)
	register int	sz;
	word		w;
{
	while (part_size % sz) part_size++;

	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1 || sz == 2) {
		w &= (sz == 1 ? 0xFF : 0xFFFF);
		w <<= 8 * part_size;
		part_word |= w;
	} else {
		assert(sz == 4);
		part_word = w;
	}
	part_size += sz;
}

con_mult(sz)
	word	sz;
{
	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".long\t%s\n",str);
}

#ifdef REGVARS
full lbytes;
struct regadm {
	char *ra_str;
	long ra_off;
} regadm[4];
int n_regvars;

regscore(off,size,typ,score,totyp) long off; {

	/*This function is copied from pdp/ncg
	 */
	/*
	 * This function is full of magic constants.
	 * They are a result of experimentation.
	 */

	if (size != 2)
		return(-1);
	score -= 1;	/* allow for save/restore */
	if (off>=0)
		score -= 2;
	if (typ==reg_pointer)
		score *= 17;
	else if (typ==reg_loop)
		score = 10*score+50;	/* Guestimate */
	else
		score *= 10;
	return(score);	/* 10 * estimated # of words of profit */
}

i_regsave() {

	n_regvars=0;
}

f_regsave(){
	register i;

	fprintf(codefile, "enter [");
	if (n_regvars>=1){
		fprintf(codefile,"%s",regadm[0].ra_str);
		for (i=1; i<n_regvars; i++)
			fprintf(codefile,",%s",regadm[0].ra_str);
	}
	fprintf(codefile, "], %d\n", lbytes);
	for (i=0;i<n_regvars;i++)
		if (regadm[i].ra_off>=0)
			fprintf(codefile, "movd %ld(fp), %s",
			  regadm[i].ra_off, regadm[i].ra_str);
}

regsave(regstr,off,size) char *regstr; long off; {

	fprintf(codefile,"\t! Local %ld into %s\n",off,regstr);
	regadm[n_regvars].ra_str = regstr;
	regadm[n_regvars].ra_off = off;
	n_regvars++;
}

regreturn() {
	register int i;

	fprintf(codefile, "exit [");
	if (n_regvars>=1){
		fprintf(codefile,"%s",regadm[0].ra_str);
		for (i=1; i<n_regvars; i++)
			fprintf(codefile,",%s",regadm[0].ra_str);
	}
	fprintf(codefile, "]\n");
}

#endif

mes(type) word type ; {
	int argt ;

	switch ( (int)type ) {
	case ms_ext :
		for (;;) {
			switch ( argt=getarg(
			    ptyp(sp_cend)|ptyp(sp_pnam)|sym_ptyp) ) {
			case sp_cend :
				return ;
			default:
				strarg(argt) ;
				fprintf(codefile,".define %s\n",argstr) ;
				break ;
			}
		}
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}

prolog(nlocals) full nlocals; {

#ifndef REGVARS
	fprintf(codefile, "STMFD R12<, {R14}\n");
	fprintf(codefile, "STMFD R12<, {R13}\n");
	fprintf(codefile, "MOV R13, R12\n");
	if (nlocals)
		fprintf(codefile, "SUB R12, R12, #%d\n", nlocals);
	return;
#else
	lbytes = nlocals;
#endif
}

char *segname[] = {
	".sect .text",
	".sect .data",
	".sect .data",
	".sect .bss"
};

con_float() {

	fatal("no reals");
}