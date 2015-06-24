#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mnem.h>
#include <em_flag.h>
#include "alloc.h"
#include "line.h"
#include "lookup.h"
#include "proinf.h"
#include "optim.h"
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
void putargs(arg_p ap);
void putstr(argb_p abp);
void outdef(sym_p sp);
void outocc(sym_p sp);
void outinst(int m);
void outoff(offset off);
void outint(short i);
void outshort(short i);
void numlab(num_p np);
void outnum(num_p np);
void outsym(sym_p sp);

#define outbyte(b) putc(b,outfile)

void putlines(line_p lnp)
{
	arg_p ap;
	line_p temp;
	int instr;
	short curlin= -2;
	short thislin;

	while ( lnp != (line_p) 0) {
		instr = lnp->l_instr&BMASK;
		switch(lnp->l_optyp) {
		case OPSYMBOL:
			if ((lnp->l_instr&BMASK) == ps_sym)
				outdef(lnp->l_a.la_sp);
			else
				outocc(lnp->l_a.la_sp);
			break;
		case OPSVAL:
			outocc(lnp->l_a.la_sval.lasv_sp);
			break;
#ifdef LONGOFF
		case OPLVAL:
			outocc(lnp->l_a.la_lval.lalv_sp);
			break;
#endif
		case OPLIST:
			ap = lnp->l_a.la_arg;
			while (ap != (arg_p) 0) {
				switch(ap->a_typ) {
				case ARGSYM:
					outocc(ap->a_a.a_sp);
					break;
				case ARGVAL:
					outocc(ap->a_a.a_val.av_sp);
					break;
				}
				ap = ap->a_next;
			}
			break;
		}

		/*
		 * global symbols now taken care of
		 */


		switch(instr) {
		case ps_sym:
			break;
		case op_lni:
			if (curlin != -2)
				curlin++;
			outinst(instr);
			break;
		case op_lin:
			switch(lnp->l_optyp) {
			case OPNO:
			case OPOFFSET:
			case OPNUMLAB:
			case OPSYMBOL:
			case OPSVAL:
			case OPLVAL:
			case OPLIST:
				outinst(instr);
				goto processoperand;
			case OPSHORT:
				thislin = lnp->l_a.la_short;
				break;
			default:
				thislin = (lnp->l_optyp&BMASK)-Z_OPMINI;
				break;
			}
			if (thislin == curlin && !nflag) {
				temp = lnp->l_next;
				oldline(lnp);
				lnp = temp;
				OPTIM(O_LINGONE);
				continue;
			} else if (thislin == curlin+1 && !nflag) {
				instr = op_lni;
				outinst(instr);
				temp = lnp->l_next;
				oldline(lnp);
				OPTIM(O_LINLNI);
				lnp = newline(OPNO);
				lnp->l_next = temp;
				lnp->l_instr = instr;
			} else {
				outinst(instr);
			}
			curlin = thislin;
			break;
		case op_lab:
			curlin = -2;
			break;
		default:
			if ((em_flag[instr-sp_fmnem]&EM_FLO)==FLO_P)
				curlin = -2;
			outinst(instr);
		}
processoperand:
		switch(lnp->l_optyp) {
		case OPNO:
			if ((em_flag[instr-sp_fmnem]&EM_PAR)!=PAR_NO)
				outbyte( (byte) sp_cend) ;
			break;
		default:
			outint((lnp->l_optyp&BMASK)-Z_OPMINI);
			break;
		case OPSHORT:
			outint(lnp->l_a.la_short);
			break;
#ifdef LONGOFF
		case OPOFFSET:
			outoff(lnp->l_a.la_offset);
			break;
#endif
		case OPNUMLAB:
			if (instr == op_lab)
				numlab(lnp->l_a.la_np->n_repl);
			else if (instr < sp_fpseu) /* plain instruction */
				outint((short) lnp->l_a.la_np->n_repl->n_number);
			else
				outnum(lnp->l_a.la_np->n_repl);
			break;
		case OPSYMBOL:
			outsym(lnp->l_a.la_sp);
			break;
		case OPSVAL:
			outbyte( (byte) sp_doff) ;
			outsym(lnp->l_a.la_sval.lasv_sp);
			outint(lnp->l_a.la_sval.lasv_short);
			break;
#ifdef LONGOFF
		case OPLVAL:
			outbyte( (byte) sp_doff) ;
			outsym(lnp->l_a.la_lval.lalv_sp);
			outoff(lnp->l_a.la_lval.lalv_offset);
			break;
#endif
		case OPLIST:
			putargs(lnp->l_a.la_arg);
			switch(instr) {
			case ps_con:
			case ps_rom:
			case ps_mes:
				outbyte( (byte) sp_cend) ;
			}
		}
		/*
		 * instruction is output now.
		 * remove its useless body
		 */

		temp = lnp->l_next;
		oldline(lnp);
		lnp = temp;
		if (ferror(outfile))
			error("write error");
	}
}

void putargs(arg_p ap)
{
	while (ap != (arg_p) 0) {
		switch(ap->a_typ) {
		default:
			assert(FALSE);
		case ARGOFF:
			outoff(ap->a_a.a_offset);
			break;
		case ARGNUM:
			outnum(ap->a_a.a_np->n_repl);
			break;
		case ARGSYM:
			outsym(ap->a_a.a_sp);
			break;
		case ARGVAL:
			outbyte( (byte) sp_doff) ;
			outsym(ap->a_a.a_val.av_sp);
			outoff(ap->a_a.a_val.av_offset);
			break;
		case ARGSTR:
			outbyte( (byte) sp_scon) ;
			putstr(&ap->a_a.a_string);
			break;
		case ARGICN:
			outbyte( (byte) sp_icon) ;
			goto casecon;
		case ARGUCN:
			outbyte( (byte) sp_ucon) ;
			goto casecon;
		case ARGFCN:
			outbyte( (byte) sp_fcon) ;
		casecon:
			outint(ap->a_a.a_con.ac_length);
			putstr(&ap->a_a.a_con.ac_con);
			break;
		}
		ap = ap->a_next;
	}
}

void putstr(argb_p abp)
{
	argb_p tbp;
	int length;

	length = 0;
	tbp = abp;
	while (tbp!= (argb_p) 0) {
		length += tbp->ab_index;
		tbp = tbp->ab_next;
	}
	outint(length);
	while (abp != (argb_p) 0) {
		for (length=0;length<abp->ab_index;length++)
			outbyte( (byte) abp->ab_contents[length] );
		abp = abp->ab_next;
	}
}

void outdef(sym_p sp)
{
	/*
	 * The surrounding If statement is removed to be friendly
	 * to Backend writers having to deal with assemblers
	 * not following our conventions.
	if ((sp->s_flags&SYMOUT)==0) {
	 */
		sp->s_flags |= SYMOUT;
		if (sp->s_flags&SYMGLOBAL) {
			outinst(sp->s_flags&SYMPRO ? ps_exp : ps_exa);
			outsym(sp);
		}
	/*
	}
	 */
}

void outocc(sym_p sp)
{
	if ((sp->s_flags&SYMOUT)==0) {
		sp->s_flags |= SYMOUT;
		if ((sp->s_flags&SYMGLOBAL)==0) {
			outinst(sp->s_flags&SYMPRO ? ps_inp : ps_ina);
			outsym(sp);
		}
	}
}

void outpro()
{

	outdef(curpro.symbol);
	outinst(ps_pro);
	outsym(curpro.symbol);
	outoff(curpro.localbytes);
}

void outend()
{
	outinst(ps_end);
	outoff(curpro.localbytes);
}

void outinst(int m)
{
	outbyte( (byte) m );
}

void outoff(offset off)
{

#ifdef LONGOFF
	if ((short) off == off)
#endif
		outint((short) off);
#ifdef LONGOFF
	else {
		outbyte( (byte) sp_cst4) ;
		outshort( (short) (off&0177777L) );
		outshort( (short) (off>>16) );
	}
#endif
}

void outint(short i)
{
	if (i>= -sp_zcst0 && i< sp_ncst0-sp_zcst0)
		outbyte( (byte) (i+sp_zcst0+sp_fcst0) );
	else {
		outbyte( (byte) sp_cst2) ;
		outshort(i);
	}
}

void outshort(short i)
{
	outbyte( (byte) (i&BMASK) );
	outbyte( (byte) (i>>8) );
}

void numlab(num_p np)
{
	if (np->n_number < sp_nilb0)
		outbyte( (byte) (np->n_number + sp_filb0) );
	else
		outnum(np);
}

void outnum(num_p np)
{
	if(np->n_number<256) {
		outbyte( (byte) sp_ilb1) ;
		outbyte( (byte) (np->n_number) );
	} else {
		outbyte( (byte) sp_ilb2) ;
		outshort((short) np->n_number);
	}
}

void outsym(sym_p sp)
{
	byte *p;
	unsigned int num;
	printf("May do something nasty... (%s)", __func__);
	if (sp->s_name[0] == '.') {
		num = atoi(&sp->s_name[1]);
		if (num < 256) {
			outbyte( (byte) sp_dlb1) ;
			outbyte( (byte) (num) );
		} else {
			outbyte( (byte) sp_dlb2) ;
			outshort((short) num);
		}
	} else {
		p= sp->s_name;
		while ( (*p) && p < &sp->s_name[IDL] )
			p++;
		num = p - sp->s_name;
		outbyte( (byte) (sp->s_flags&SYMPRO ? sp_pnam : sp_dnam) );
		outint((short) num);
		p = sp->s_name;
		while (num--)
			outbyte( (byte) *p++ );
	}
}
