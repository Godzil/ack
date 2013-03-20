/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_STACK_H
#define LANG_CEM_CEMCOM_ANSI_STACK_H

/* lang/cem/cemcom.ansi/stack.c */
void stack_level(void);
void stack_idf(struct idf *idf, struct stack_level *stl);
struct stack_level *stack_level_of(int lvl);
void unstack_level(void);
void unstack_world(void);

#endif /* LANG_CEM_CEMCOM_ANSI_STACK_H */

