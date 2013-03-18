#ifndef H_MISSING_PROTO_H
#define H_MISSING_PROTO_H

#ifdef NOSBRK
void *sbrk(__intptr_t increment);
#endif

#ifdef NOMKTEMP
char *mktemp(char *template);
#endif

#endif /* H_MISSING_H */