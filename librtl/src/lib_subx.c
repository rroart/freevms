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
#include "ssdef.h"

/* addx & subx COULD use 32 bit arithmetic to do a word at a time. But
   this only works on hardware which has the same endian as the VAX!
   (Intel works fine!! :-) ) So this version works byte by byte which
   should work on VMS dates regardless of system endian - but they
   will NOT perform arithmetic correctly for native data types on
   systems with opposite endian!!! */

unsigned long lib$subx(const void *subant, const void *subee, void *result,
	const long *lenadd)
{
    long count;
    unsigned long carry = 0;
    const unsigned char *ant = (const unsigned char *) subant;
    const unsigned char *ee = (const unsigned char *) subee;
    unsigned char *res = (unsigned char *) result;

    if (lenadd == NULL) {
        count = 8;
    } else {
        count = *lenadd * 4;
    }

    while (count-- > 0) {
        carry = *ant++ - (carry + *ee++);
        *res++ = carry;
        carry = (carry >> 8) & 1;

    }
    return SS$_NORMAL;
}
