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
#include "lib$routines.h"	/* Our header file! */

struct TIME {
    unsigned char time[8];
};

unsigned long lib$sub_times(const void *time1a, const void *time2a,
    void *resulta)
{
    const struct TIME *time1 = (const struct TIME *)time1a;
    const struct TIME *time2 = (const struct TIME *)time2a;
    struct TIME *result = (struct TIME *)resulta;

    if ((time1->time[7] & 0x80) != (time2->time[7] & 0x80)) {
        return lib$addx(time1,time2,result,NULL);
    } else {
        register cmp,count = 7;
        do {
            if ((cmp = (time1->time[count] - time2->time[count]))) break;
        } while (--count >= 0);
        if (cmp < 0) {
            return lib$subx(time1,time2,result,NULL);
        } else {
            return lib$subx(time2,time1,result,NULL);
        }
    }
}

