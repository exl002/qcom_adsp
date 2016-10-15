#ifndef _STDLIBK_H_
#define _STDLIBK_H_

#include <qurtk_stddef.h>

#define STD_NEGATIVE  -1
#define STD_BADPARAM  -2
#define STD_OVERFLOW  -3
#define STD_NODIGITS  -4

uint32_t strtoulK(const char *  str,
                   int           base,
                   const char ** end_ptr,
                   int *         p_error_num);

#endif //_STDLIBK_H_
