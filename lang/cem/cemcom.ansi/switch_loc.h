/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_SWITCH_H
#define LANG_CEM_CEMCOM_ANSI_SWITCH_H

/* lang/cem/cemcom.ansi/switch.c */
int compact(int nr, arith low, arith up);
void code_startswitch(struct expr **expp);
void code_endswitch(void);
void code_case(struct expr *expr);
void code_default(void);

#endif /* LANG_CEM_CEMCOM_ANSI_SWITCH_H */

