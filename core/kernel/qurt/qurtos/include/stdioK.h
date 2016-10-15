#ifndef _STDIOK_H_
#define _STDIOK_H_

#include <qurtk_stddef.h>

typedef __builtin_va_list       va_list;
#define va_arg(ap, type)        __builtin_va_arg((ap), type)
#define va_copy(dest, src)      __builtin_va_copy((ap), type)
#define va_end(ap)              __builtin_va_end((ap))
#define va_start(ap, parmN)     __builtin_stdarg_start((ap), (parmN))

#define tolowerK(c)             (c|32)

#define is_hex_char(c)          (((c >= '0') && (c <= '9')) ||                         \
                                    ((tolowerK(c) >= 'a') && (tolowerK(c) <= 'f')))

#define is_decimal_char(c)      ((c >= '0') && (c <= '9'))

#define is_alpha_char(c)        ((tolowerK(c) >= 'a') && (tolowerK(c) <= 'z'))

static inline uint8_t hex2val(char ch)
{
    ch = tolowerK(ch);
    if (is_decimal_char(ch))
    {
        return (ch - '0');
    }
    else if ((ch <= 'f') && (ch >= 'a'))
    {
        return (0xa + ch - 'a');
    }
    else
    {
        return 0;
    }
}


/* @note Do not implement sprintfk as it is an insecure API */
int snprintfK(char *str, size_t size, const char *format, ...);
int vsnprintfK(char *p_dest, size_t dest_length, const char *p_format, va_list args);


#endif //_STDIOK_H_
