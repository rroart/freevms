/*

       VMSTIME.C  v1.1

       Author: Paul Nankervis

       Please send bug reports or requests for enhancement
       or improvement via email to:     PaulNank@au1.ibm.com


       This module contains versions of the VMS time routines
       sys$numtim(), sys$asctim() and friends... They are
       intended to be compatible with the routines of the same
       name on a VMS system (so descriptors feature regularly!)

       This code relies on being able to manipluate day numbers
       and times using 32 bit arithmetic to crack a VMS quadword
       byte by byte. If your C compiler doesn't have 32 bit int
       fields give up now! On a 64 bit systems this code could
       be modified to do 64 bit operations directly....

       One advantage of doing arihmetic byte by byte is that
       the code does not depend on what 'endian' the target
       machine is - it will always treat bytes in the same order!
       (Hopefully VMS time bytes will always be in the same order!)

       A couple of stupid questions to go on with:-
           o OK, I give up! What is the difference between a zero
             date and a zero delta time?
           o Anyone notice that the use of 16 bit words in
             sys$numtim restricts delta times to 65535 days?

                                       Paul Nankervis

*/

#include "lib$routines.h"	/* Our header file! */
#include "ssdef.h"


unsigned long lib$mult_delta_time(const long *multiple, void *timadr)
{
    unsigned count = 8,carry = 0;
    long factor = *multiple;
    unsigned char *ptr = (unsigned char *)timadr;

    /* Check for delta time... */

    if (ptr[7] & 0x80) {

        /* Use absolute factor... */

        if (factor < 0) factor = -factor;

        /* Multiply delta time... */

        do {
            carry += *ptr * factor;
            *ptr++ = carry;
            carry = (carry >> 8);
        } while (--count > 0);

        return SS$_NORMAL;
    } else {
        return SS$_IVTIME;
    }
}
