/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_STAB_H
#define LANG_CEM_CEMCOM_ANSI_STAB_H

/* lang/cem/cemcom.ansi/stab.c */
void stb_tag(register struct tag *tg, char *str);
void stb_typedef(register struct type *tp, char *str);
void stb_string(register struct def *df, int kind, char *str);

#endif /* LANG_CEM_CEMCOM_ANSI_STAB_H */

