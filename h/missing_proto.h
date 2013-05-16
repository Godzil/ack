#ifndef H_MISSING_PROTO_H
#define H_MISSING_PROTO_H

#ifdef NOSBRK
void *sbrk(__intptr_t increment);
void *brk(void * addr);
#endif

#ifdef NOMKTEMP
char *mktemp(char *template);
#endif

#ifdef EMULATE_BRK
void *sbrk_emu(int increment);
void *brk_emu(const void * addr);

#ifdef sbrk
#undef sbrk
#endif
#ifdef brk
#undef brk
#endif

#define sbrk sbrk_emu
#define brk brk_emu

#endif

#endif /* H_MISSING_H */