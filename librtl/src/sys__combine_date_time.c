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

#include <time.h>               /* C header for $GETTIM to find time */
#include "sys$routines.h"	/* Our header file! */
#include "ssdef.h"

#define TIMEBASE 100000         /* 10 millisecond units in quadword */
#define TIMESIZE 8640000        /* Factor between dates & times */

/* combine_date_time() is an internal routine to put date and time into a
   quadword - basically the opposite of lib_day() .... */

struct TIME {
    unsigned char time[8];
};

unsigned long sys$__combine_date_time(int days,struct TIME *timadr,int day_time)
{
    if (day_time >= TIMESIZE) {
        return SS$_IVTIME;
    } else {

        /* Put days into quad timbuf... */

        unsigned long count,time;
        unsigned char *ptr;

        count = 8;
        ptr = timadr->time;
        time = days;
        do {
            *ptr++ = time;
            time = (time >> 8);
        } while (--count > 0);

        /* Factor in the time... */

        count = 8;
        ptr = timadr->time;
        time = day_time;
        do {
            time += *ptr * TIMESIZE;
            *ptr++ = time;
            time = (time >> 8);
        } while (--count > 0);

        /* Factor by time base... */

        count = 8;
        ptr = timadr->time;
        time = 0;
        do {
            time += *ptr * TIMEBASE;
            *ptr++ = time;
            time = (time >> 8);
        } while (--count > 0);

        return SS$_NORMAL;
    }
}



