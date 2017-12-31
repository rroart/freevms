// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

#include <string.h>

// no check for LIB$_INVARG, LIB$_INSVIRMEM

int lib$lookup_key (void * search_string, long * key_table_array, long * key_value, void * keyword_string, unsigned short int * resultant_length)
{
    struct dsc$descriptor * dsc = search_string;
    int max = * key_table_array;
    int i;
    key_table_array++;
    int found = 0;
    for (i = 0; i < max; i += 2)
    {
        char * s = key_table_array[i];
        char len = *s;
        s++;
        if (len <= dsc->dsc$w_length)
        {
            if (0 == strncmp (dsc->dsc$a_pointer, s, len))
            {
                if (found)
                    return LIB$_AMBKEY;
                found = i;
            }
        }
    }
    if (!found)
        return LIB$_UNRKEY;
    char * s = key_table_array[found];
    char len = *s;
    s++;
    if (key_value)
        *key_value = key_table_array[found + 1];
    if (resultant_length)
        *resultant_length = len;
    if (keyword_string)
    {
        int min = len;
        if (min < dsc->dsc$w_length)
            min = dsc->dsc$w_length;
        memcpy (dsc->dsc$a_pointer, s, min);
        if (dsc->dsc$w_length < len)
            return LIB$_STRTRU;
    }
    return SS$_NORMAL;
}


