/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_IVAL_H
#define LANG_CEM_CEMCOM_ANSI_IVAL_H

#include "estack.h"

/* lang/cem/cemcom.ansi/ival.g */
void gen_tpcheck(struct type **tpp);
void gen_simple_exp(struct type **tpp, struct expr **expp);
struct type **arr_elem(struct type **tpp, struct e_stack *p);
struct sdef *next_field(struct sdef *sd, struct e_stack *p);
struct type **gen_tphead(struct type **tpp, int nest);
struct type **gen_tpmiddle();
struct sdef *gen_align_to_next(struct e_stack *p);
void gen_tpend();
/*	check_and_pad() is given a simple initialisation expression
	where the type can be either a simple or an aggregate type.
	In the latter case, only the first member is initialised and
	the rest is zeroed.
*/
void check_and_pad(struct expr **expp, struct type **tpp);
/*	pad() fills an element of type tp with zeroes.
	If the element is an aggregate, pad() is called recursively.
*/
void pad(struct type *tpx);
/*	check_ival() checks whether the initialisation of an element
	of a fundamental type is legal and, if so, performs the initialisation
	by directly generating the necessary code.
	No further comment is needed to explain the internal structure
	of this straightforward function.
*/
void check_ival(struct expr **expp, struct type *tp);
/*	ch_array() initialises an array of characters when given
	a string constant.
	Alignment is taken care of.
*/

/* tpp -> type tp = array of characters	*/
void ch_array(struct type **tpp, struct expr *ex);
/*	As long as some parts of the pipeline cannot handle very long string
	constants, string constants are written out in chunks
*/
void str_cst(char *str, int len, int inrom);
#ifndef NOBITFIELD
/*	put_bf() takes care of the initialisation of (bit-)field
	selectors of a struct: each time such an initialisation takes place,
	put_bf() is called instead of the normal code generating routines.
	Put_bf() stores the given integral value into "field" and
	"throws" the result of "field" out if the current selector
	is the last of this number of fields stored at the same address.
*/
void put_bf(struct type *tp, arith val);
#endif /* NOBITFIELD */

int zero_bytes(struct sdef *sd);
int valid_type(struct type *tp, char *str);
void con_int(struct expr *ex);
void illegal_init_cst(struct expr *ex);
void too_many_initialisers();


#endif /* LANG_CEM_CEMCOM_ANSI_IVAL_H */

