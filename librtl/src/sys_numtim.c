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

#include "sys$routines.h"	/* Our header file */
#include "lib$routines.h"	/* LIB$ Header File */
#include "ssdef.h"

struct TIME {
    unsigned char time[8];
};

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



/* sys_numtim() takes quadword and breaks it into a seven word time buffer */

static const unsigned char month_days[] = {31,29,31,30,31,30,31,31,30,31,30,31};

unsigned long sys$numtim(unsigned short timbuf[7], struct TIME *timadra)
{
    struct TIME *timadr = (struct TIME *)timadra;
    register date,time;

    /* Use lib_day to crack time into date/time... */

    {
        int days,day_time;
        register unsigned sts;
        sts = lib$day(&days, timadr, &day_time);
        if ((sts & 1) == 0) {
            return sts;
        }
        date = days;
        time = day_time;
    }

    /* Delta or date... */

    if (date < 0 || time < 0) {
        timbuf[2] = -date;      /* Days */
        timbuf[1] = 0;          /* Month */
        timbuf[0] = 0;          /* Year */
        time = -time;

    } else {

        /* Date... */

        register year,month;
        date += OFFSET_DAYS;
        year = BASE_YEAR + (date / QUAD_CENTURY_DAYS) * 400;
        date %= QUAD_CENTURY_DAYS;

        /* Kludge century division - last century in quad is longer!! */

        if ((month = date / CENTURY_DAYS) == 4) month = 3;
        date -= month * CENTURY_DAYS;
        year += month * 100;

        /* Use the same technique to find out the quad year and year -
           last year in quad is longer!! */

        year += (date / QUAD_YEAR_DAYS) * 4;
        date %= QUAD_YEAR_DAYS;

        if ((month = date / YEAR_DAYS) == 4) month = 3;
        date -= month * YEAR_DAYS;
        year += month;

        /* Adjust for years which have no Feb 29th */

        if (date++ > 58) {
            if (month != 3) {
                date++;
            } else {
                if ((year % 100) == 0 && (year % 400) != 0) date++;
            }
        }
        /* Figure out what month it is... */

        {
            unsigned char *mthptr = month_days;
            month = 1;
            while (date > *mthptr) {
                date -= *mthptr++;
                month++;
            }
        }

        /* Return date results... */

        timbuf[2] = date;       /* Days */
        timbuf[1] = month;      /* Month */
        timbuf[0] = year;       /* Year */
    }

    /* Return time... */

    timbuf[6] = time % 100;     /* Hundredths */
    time /= 100;
    timbuf[5] = time % 60;      /* Seconds */
    time /= 60;
    timbuf[4] = time % 60;      /* Minutes */
    timbuf[3] = time / 60;      /* Hours */

    return SS$_NORMAL;
}

