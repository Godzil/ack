/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_MAIN_H
#define LANG_CEM_CEMCOM_ANSI_MAIN_H

/* lang/cem/cemcom.ansi/main.c */
int main(int argc, char *argv[]);
void list_dependencies(char *source);
void add_dependency(char *s);
void dependency(char *s, char *source);
void compile(int argc, char *argv[]);
void init(void);
void init_specials(struct sp_id *si);
void No_Mem(void);
void C_failed(void);

#endif /* LANG_CEM_CEMCOM_ANSI_MAIN_H */

