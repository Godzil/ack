#ifndef LANG_CEM_CEMCOM_ANSI_CSTOPER_H
#define LANG_CEM_CEMCOM_ANSI_CSTOPER_H 

void cstbin(struct expr **expp, int oper, struct expr *expr);
void cut_size(struct expr *expr);
void init_cst();

#endif