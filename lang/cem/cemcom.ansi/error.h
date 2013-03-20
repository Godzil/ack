/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_ERROR_H
#define LANG_CEM_CEMCOM_ANSI_ERROR_H

/* lang/cem/cemcom.ansi/error.c */
void error(char *fmt, ...);
void  expr_error(struct expr *expr, char *fmt, ...);
void lexstrict(char *fmt, ...);
void  strict(char *fmt, ...);
void  expr_strict(struct expr *expr, char *fmt, ...);
void  warning(char *fmt, ...);
void  expr_warning(struct expr *expr, char *fmt, ...);
void  lexerror(char *fmt, ...);
void  lexwarning(char *fmt, ...);
void  crash(char *fmt, ...);
void  fatal(char *fmt, ...);

#endif /* LANG_CEM_CEMCOM_ANSI_ERROR_H */

