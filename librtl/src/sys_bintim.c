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
#include "sys$routines.h"	/* SYS$ header file */
#include "lib$routines.h"	/* LIB$ header file */
#include "ssdef.h"

/* sys_bintim() takes ascii time and convert it to a quadword */

struct TIME {
    unsigned char time[8];
};

static const char month_names[] = "-JAN-FEB-MAR-APR-MAY-JUN-JUL-AUG-SEP-OCT-NOV-DEC-";
static const char time_sep[] = "::.";

unsigned long sys$bintim(struct dsc$descriptor *timbuf, struct TIME *timadra)
{
    struct TIME *timadr = (struct TIME *)timadra;

    register length = timbuf->dsc$w_length;
    register char *chrptr = timbuf->dsc$a_pointer;
    unsigned short wrktim[7];
    int num,tf;


    /* Skip leading spaces... */

    while (length > 0 && *chrptr == ' ') {
        length--;
        chrptr++;
    }

    /* Get the day number... */

    num = -1;
    if (length > 0 && *chrptr >= '0' && *chrptr <= '9') {
        num = 0;
        do {
            num = num * 10 + (*chrptr++ - '0');
        } while (--length > 0 && *chrptr >= '0' && *chrptr <= '9');
    }
    /* Check for month separator "-" - if none delta time... */

    if (length > 0 && *chrptr == '-') {
        chrptr++;

        /* Get current time for defaults... */

        sys$numtim(wrktim,NULL);
        if (num >= 0) wrktim[2] = num;
        num = 0;
        if (--length >= 3 && *chrptr != '-') {
            char *mn = month_names + 1;
            num = 1;
            while (num <= 12) {
                if (memcmp(chrptr,mn,3) == 0) break;
                mn += 4;
                num++;
            }
            chrptr += 3;
            length -= 3;
            wrktim[1] = num;
        }
        /* Now look for year... */

        if (length > 0 && *chrptr == '-') {
            length--;
            chrptr++;
            if (length > 0 && *chrptr >= '0' && *chrptr <= '9') {
                num = 0;
                do {
                    num = num * 10 + (*chrptr++ - '0');
                } while (--length > 0 && *chrptr >= '0' && *chrptr <= '9');
                wrktim[0] = num;
            }
        }
    } else {

        /* Delta time then... */

        wrktim[0] = wrktim[1] = 0;
        wrktim[2] = num;
        wrktim[3] = wrktim[4] = wrktim[5] = wrktim[6] = 0;
    }

    /* Skip any spaces between date and time... */

    while (length > 0 && *chrptr == ' ') {
        length--;
        chrptr++;
    }

    /* Now wrap up time fields... */

    for (tf = 0; tf < 3; tf++) {
        if (length > 0 && *chrptr >= '0' && *chrptr <= '9') {
            num = 0;
            do {
                num = num * 10 + (*chrptr++ - '0');
            } while (--length > 0 && *chrptr >= '0' && *chrptr <= '9');
            wrktim[3 + tf] = num;
            if (num > 59) wrktim[1] = 13;
        }
        if (length > 0 && *chrptr == time_sep[tf]) {
            length--;
            chrptr++;
        } else {
            break;
        }
    }

    /* Hundredths of seconds need special handling... */

    if (length > 0 && *chrptr >= '0' && *chrptr <= '9') {
        tf = 10;
        num = 0;
        do {
            num = num + tf * (*chrptr++ - '0');
            tf = tf / 10;
        } while (--length > 0 && *chrptr >= '0' && *chrptr <= '9');
        wrktim[6] = num;
    }
    /* Now skip any trailing spaces... */

    while (length > 0 && *chrptr == ' ') {
        length--;
        chrptr++;
    }

    /* If anything left then we have a problem... */

    if (length == 0) {
        return lib$cvt_vectim(wrktim,timadr);
    } else {
        return SS$_IVTIME;
    }
}


