/*
 * lib_cvt_htb.c
 *
 *  Code for VAX LIB$CVT_HTB routine
 *
 * Description:
 *
 *  Converts an ascii hexidecimal string representation to
 *  binary.
 *
 * Bugs:
 *
 *
 * History
 *
 *  March 30, 1996 - Kevin Handy
 *      Preliminary design.
 *
 */

#include <ctype.h>
#include "libdef.h"
#include "lib$routines.h"

/*************************************************************
 * xtoi
 *
 *  Convert one hex digit to a number
 *
 * Bugs:
 *  Assumes that it is actually givin a hex digit.
 */
static int xtoi(char ch)
{
    /*
     * 0-9
     */
    if ((ch >= '0') && (ch <= '9'))
    {
        return(ch - '0');
    }

    /*
     * a-f
     */
    if ((ch >= 'a') && (ch <= 'f'))
    {
        return ch - 'a' + 10;
    }

    /*
     * A-F
     */
    return ch - 'A' + 10;
}

/*************************************************************
 * str$trim
 *
 */
int lib$cvt_htb(int byte_count, const char* source_string,
                          long* result)
{
    int loop;           /* Loop for characters */
    long answer = 0;        /* Result returned back */
    int status = 1;   /* Exit status */

    /*
     * Loop through all the characters passed
     */
    for (loop = 0; loop < byte_count; loop++)
    {
        /*
         * Make sure it is a valid character
         */
        if (isxdigit(source_string[loop]))
        {
            /*
             * Add one more digit to result
             */
            answer = answer * 10 + xtoi(source_string[loop]);
        }
        else
        {
            /*
             * Bad character
             */
            status = 0;
        }
    }

    /*
     * Return result
     */
    *result = answer;
    return status;
}


