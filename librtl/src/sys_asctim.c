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

#include <stdlib.h>
#include "descrip.h"
#include "sys$routines.h"	/* Our header file! */
#include "ssdef.h"

struct TIME {
    unsigned char time[8];
};

const char month_names[] = "-JAN-FEB-MAR-APR-MAY-JUN-JUL-AUG-SEP-OCT-NOV-DEC-";

/* sys_asctim() converts quadword to ascii... */

unsigned long sys$asctim(unsigned short *timlen,
    struct dsc$descriptor *timbuf,
    const void *timadra, unsigned long cvtflg)
{
    const struct TIME *timadr = (const struct TIME *)timadra;
    long count,timval;
    unsigned short wrktim[7];
    long length = timbuf->dsc$w_length;
    char *chrptr = timbuf->dsc$a_pointer;

    /* First use sys_numtim to get the date/time fields... */

    {
        register unsigned sts;
        sts = sys$numtim(wrktim, timadr);
        if ((sts & 1) == 0) {
            return sts;
        }
    }

    /* See if we want delta days or date... */

    if (cvtflg == 0) {

        /* Check if date or delta time... */

        if (*wrktim) {

            /* Put in days and month... */

            if (length > 0) {
                if ((timval = wrktim[2]) / 10 == 0) {
                    *chrptr++ = ' ';
                } else {
                    *chrptr++ = '0' + timval / 10;
                }
                length--;
            }
            if (length > 0) {
                *chrptr++ = '0' + (timval % 10);
                length--;
            }
            if ((count = length) > 5) count = 5;
            memcpy(chrptr,month_names + (wrktim[1] * 4 - 4),count);
            length -= count;
            chrptr += count;
            timval = *wrktim;
        } else {

            /* Get delta days... */

            timval = wrktim[2];
        }

        /* Common code for year number and delta days!! */

        count = 10000;
        if (timval < count) {
            count = 1000;
            while (length > 0 && timval < count && count > 1) {
                length--;
                *chrptr++ = ' ';
                count /= 10;
            }
        }
        while (length > 0 && count > 0) {
            length--;
            *chrptr++ = '0' + (timval / count);
            timval = timval % count;
            count /= 10;
        }

        /* Space between date and time... */

        if (length > 0) {
            *chrptr++ = ' ';
            length--;
        }
    }
    /* Do time... :-) */

    count = 3;
    do {
        timval = wrktim[count];
        if (length >= 1) *chrptr++ = '0' + (timval / 10);
        if (length >= 2) {
            *chrptr++ = '0' + (timval % 10);
            length -= 2;
        } else {
            length = 0;
        }
        if (count < 6 && length > 0) {
            length--;
            if (count == 5) {
                *chrptr++ = '.';
            } else {
                *chrptr++ = ':';
            }
        }
    } while (++count < 7);

    /* We've done it - time to return length... */

    if (timlen != NULL) *timlen = timbuf->dsc$w_length - length;
    return SS$_NORMAL;
}

