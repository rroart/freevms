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

#include "lib$routines.h"	/* LIB$ header file */
#include "ssdef.h"

#define OFFSET_DAYS     94187
/*   ((1858_1601)*365) + ((1858_1601)/4) - ((1858_1601)/100)
   + ((1858_1601)/400) + 320
        OFFSET FROM 1/1/1601 TO 17/11/1858  */
#define BASE_YEAR       1601

struct TIME {
    unsigned char time[8];
};

unsigned long sys$__combine_date_time(int days, const struct TIME *timadr,
	int day_time);

/* lib_cvt_vectim() takes individual time fields in seven word buffer and
   munges into a quadword... */

static const unsigned short month_end[] =
	{0,31,59,90,120,151,181,212,243,273,304,334,365};

unsigned long lib$cvt_vectim(const void* timbufa, void *timadra)
{
    const unsigned short * timbuf = (const unsigned short *)timbufa;
    struct TIME *timadr = (struct TIME *)timadra;

    int delta = 0;
    register unsigned sts,days,day_time;
    sts = SS$_NORMAL;

    /* lib_cvt_vectim packs the seven date/time components into a quadword... */

    if (timbuf[0] == 0 && timbuf[1] == 0) {
        delta = 1;
        days = timbuf[2];
    } else {
        register leap = 0,year = timbuf[0],month = timbuf[1];
        if (month >= 2) {
            if ((year % 4) == 0) {
                if ((year % 100) == 0) {
                    if ((year % 400) == 0) {
                        leap = 1;
                    }
                } else {
                    leap = 1;
                }
            }
        }
        days = timbuf[2];
        if (year >= 1858 && year <= 9999 && month >= 1 &&
            month <= 12 && days >= 1) {
            days += month_end[month - 1];
            if (month > 2) days += leap;
            if (days <= month_end[month] + leap) {
                year -= BASE_YEAR;
                days += year * 365 + year / 4 - year / 100 + year / 400
                     - OFFSET_DAYS - 1;
            } else {
                sts = SS$_IVTIME;
            }
        } else {
            sts = SS$_IVTIME;
        }
    }
    if (timbuf[3] > 23 || timbuf[4] > 59 ||
        timbuf[5] > 59 || timbuf[6] > 99) {
        sts = SS$_IVTIME;
    }
    if (sts & 1) {
        day_time = timbuf[3] * 360000 + timbuf[4] * 6000 +
            timbuf[5] * 100 + timbuf[6];
        sts = sys$__combine_date_time(days,timadr,day_time);
        if (delta) {

            /* We have to 2's complement delta times - sigh!! */

            register unsigned count,time;
            register unsigned char *ptr;
            count = 8;
            ptr = timadr->time;
            time = 1;
            do {
                time = time + ((~*ptr) & 0xFF);
                *ptr++ = time;
                time = (time >> 8);
            } while (--count > 0);
        }
    }
    return sts;
}


