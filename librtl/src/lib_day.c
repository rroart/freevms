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
#include "sys$routines.h"
#include "ssdef.h"

#define TIMEBASE 100000         /* 10 millisecond units in quadword */
#define TIMESIZE 8640000        /* Factor between dates & times */

#define QUAD_CENTURY_DAYS 146097
/*   (400*365) + (400/4) - (400/100) + (400/400)   */
#define CENTURY_DAYS    36524
/*   (100*365) + (100/4) - (100/100)    */
#define QUAD_YEAR_DAYS  1461
/*   (4*365) + (4/4)    */
#define YEAR_DAYS       365
/*   365        */
#define OFFSET_DAYS     94187
/*   ((1858_1601)*365) + ((1858_1601)/4) - ((1858_1601)/100)
   + ((1858_1601)/400) + 320
        OFFSET FROM 1/1/1601 TO 17/11/1858  */
#define BASE_YEAR       1601


struct TIME {
    unsigned char time[8];
};


/* lib_day() is a routine to crack quadword into day number and time */

unsigned long lib$day(long *days, const void *timadra, int *day_time)
{
    const struct TIME *timadr = (const struct TIME *)timadra;

    register unsigned date,time,count;
    register const unsigned char *srcptr;
    register unsigned char *dstptr;
    struct TIME wrktim;
    int delta;

    /* If no time specified get current using gettim() */

    if (timadr == NULL) {
        register unsigned sts;
        sts = sys$gettim(&wrktim);
        if ((sts & 1) == 0) {
            return sts;
        }
        delta = 0;
        srcptr = wrktim.time + 7;
    } else {

        /* Check specified time for delta... */

        srcptr = timadr->time + 7;
        if ((delta = (*srcptr & 0x80))) {

            /* We have to 2's complement delta times - sigh!! */

            count = 8;
            srcptr = timadr->time;
            dstptr = wrktim.time;
            time = 1;
            do {
                time = time + ((~*srcptr++) & 0xFF);
                *dstptr++ = time;
                time = (time >> 8);
            } while (--count > 0);
            srcptr = wrktim.time + 7;
        }
    }


    /* Throw away the unrequired time precision */

    count = 8;
    dstptr = wrktim.time + 7;
    time = 0;
    do {
        time = (time << 8) | *srcptr--;
        *dstptr-- = time / TIMEBASE;
        time %= TIMEBASE;
    } while (--count > 0);


    /* Seperate the date and time */

    date = time = 0;
    srcptr = wrktim.time + 7;
    count = 8;
    do {
        time = (time << 8) | *srcptr--;
        date = (date << 8) | (time / TIMESIZE);
        time %= TIMESIZE;
    } while (--count > 0);

    /* Return results... */

    if (delta) {
        *days = -(int) date;
        if (day_time != NULL) *day_time = -(int) time;
    } else {
        *days = date;
        if (day_time != NULL) *day_time = time;
    }

    return SS$_NORMAL;
}

