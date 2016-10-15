#ifndef _STRINGK_H_
#define _STRINGK_H_

#include <qurtk_stddef.h>

size_t strlenK(const char *s);                              //strlenK.c
void * memsetK(void * ptr, int value, size_t num);          //mem_init.c
//void * memcpyK(void * destination, const void * source, size_t num); <- this is the standard prototype
int memcpyK(void * destination, void * source, size_t num); //mem_init.c
int strncmpK(const char *s1, const char *s2, size_t n);     //strncmpyK.c
int strcmpK(void *dest, void *src, size_t n);               //mem_init.c
size_t strlcpyK(char *dst, const char *src, size_t siz);    //strlcpyK.c
size_t strlcatK(char *dst, const char *src, size_t siz);    //strlcatK.c
//declaration in stdlibK.h
//uint32_t strtoulK(const char * pchBuf, int nRadix, const char ** ppchEnd, int *pnError);  //strtoulK.c

#endif //_STRINGK_H_
