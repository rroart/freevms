// $Id$
// $Locker$

// Author. Roar Thron�s.

#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

extern void fancy_abort (const char * a, int b, const char * c)
{
    abort();
}

long ggc_calloc (long s1, long s2)
{
    long i= malloc(s1*s2);
    memset(i,0,s1*s2);
    return i;
}

long ggc_alloc(long x)
{
    return malloc(x);
}

long xcalloc (long s, long t)
{
    return malloc(s*t);
}

long xrealloc(long x, long y)
{
    return xrealloc(x,y);
}

extern void layout_type (tree y)
{
}

long ggc_alloc_tree(length)
{
    // check
    return malloc(length);
}

