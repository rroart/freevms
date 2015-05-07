#include <string.h>

#include "ileutil.h"

inline int check_ile3(struct _ile3 *ile)
{
    return (ile) && (ile->ile3$ps_bufaddr) && (ile->ile3$ps_retlen_addr);
}

inline void set_ile3(struct _ile3 *ile, void *p, unsigned short length)
{
    if (length > ile->ile3$w_length)
    {
        length = ile->ile3$w_length;
    }
    memcpy(ile->ile3$ps_bufaddr, p, length);
    *(ile->ile3$ps_retlen_addr) = length;
}

inline void set_ile3_from_string(struct _ile3 *ile, char *s)
{
    set_ile3(ile, s, strlen(s));
}

inline void set_ile3_from_char(struct _ile3 *ile, char c)
{
    set_ile3(ile, &c, sizeof(char));
}

inline void set_ile3_from_short(struct _ile3 *ile, short n)
{
    set_ile3(ile, &n, sizeof(short));
}

inline void set_ile3_from_int(struct _ile3 *ile, int n)
{
    set_ile3(ile, &n, sizeof(int));
}

inline void set_ile3_from_long(struct _ile3 *ile, long n)
{
    set_ile3(ile, &n, sizeof(long));
}

inline void set_ile3_from_long_long(struct _ile3 *ile, long long n)
{
    set_ile3(ile, &n, sizeof(long long));
}

inline void set_ile3_from_unsigned_char(struct _ile3 *ile, unsigned char c)
{
    set_ile3(ile, &c, sizeof(unsigned char));
}

inline void set_ile3_from_unsigned_short(struct _ile3 *ile, unsigned short n)
{
    set_ile3(ile, &n, sizeof(unsigned short));
}

inline void set_ile3_from_unsigned_int(struct _ile3 *ile, unsigned int n)
{
    set_ile3(ile, &n, sizeof(unsigned int));
}

inline void set_ile3_from_unsigned_long(struct _ile3 *ile, unsigned long n)
{
    set_ile3(ile, &n, sizeof(unsigned long));
}

inline void set_ile3_from_unsigned_long_long(struct _ile3 *ile, unsigned long long n)
{
    set_ile3(ile, &n, sizeof(unsigned long long));
}
