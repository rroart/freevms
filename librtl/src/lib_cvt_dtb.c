/*
 * lib_cvt_dtb.c
 *
 *  Code for VAX LIB$CVT_DTB routine
 *
 * Description:
 *
 *  Converts an ascii decimal string representation to
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
 * str$trim
 *
 */
int lib$cvt_dtb(int byte_count, const char* source_string, long* result)
{
    int loop; /* Loop for characters */
    long answer = 0; /* Result returned back */
    int status = 1; /* Exit status */

    /*
     * Loop through all the characters passed
     */
    for (loop = 0; loop < byte_count; loop++)
    {
        /*
         * Make sure it is a valid character
         */
        if (isdigit(source_string[loop]))
        {
            /*
             * Add one more digit to result
             */
            answer = answer * 10 + source_string[loop] - '0';
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

