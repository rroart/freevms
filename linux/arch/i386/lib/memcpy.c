#include <linux/config.h>
#include <linux/string.h>

#undef memcpy
#undef memset

void * memcpy(void * to, const void * from, size_t n)
{
    return __memcpy(to, from, n);
}

void * memset(void * s, int c, size_t count)
{
    return __memset(s, c, count);
}
