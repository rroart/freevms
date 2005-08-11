/*
**  lib$get_ef(unsigned int *efn) - allocate an event flag
**  lib$free_ef(unsigned int *efn) - free an event flag
**  lib$reserve_ef(unsigned int *efn) - reserve a specific event flag
**
**	Author: Roger Tucker
**
**		Made AST/thread reentrant by looping if already allocated in get_ef()
**		Fixed proto-types to match VMS
**		Allow freeing of event flags 1-23
**		Added routine reserve_ef()
**		Added comments
**		Added regression test and verified with AXP VMS 8.2
*/

#define __USE_GNU

#define __NEW_STARLET
#include <lib$routines.h>			/* Verify my public proto-types */
#include <libdef.h>				/* lib messages */
#include <ssdef.h>				/* system messages */

#ifndef __vms					/* Real VMS? */
#include <string.h>				/* Needed for ffsll */
#include <linux/bitops.h>			/* Need Atomic bit operations */
extern int ffsll(unsigned long long bits);	/* BUG?  should be in string.h */
#else						/* for testing on real VMS */
#include <builtins.h>
#pragma __required_pointer_size long
int ffsll(unsigned long long bits);
int test_and_clear_bit(int pos, void *bits);
int test_and_set_bit(int pos, void *bits);
#endif


/*
** Local bit mask of reserved event flags.  0 = reserved, 1=free.
**	Note: This is in process space so it could get trashed by a run-away pointer.
**
** Event flags are a little strange.  There are 64 local event flags, numbered 0-63.
**     Event flags 0 and 24-31 are permantly reserved, and cannot be allocated or freed.
**     Event flags 1-23 are initially reserved, but can be freed, then allocated.
**     Event flags 32-63 are initially free and can be reserved.
**     Event flags over 63 are for common event flags.
** Reserved event flags are shared for all threads and AST reentrant, so changing
** them must be atomic.  The first event flag to be allocated is 63.
** The low order bit represents event flag 63.
*/

static volatile unsigned long long efn_flags = 0x00000000ffffffffULL;

/*
**  lib$get_ef(unsigned int *efn) - allocate an event flag.
**              efn - Number allocated, or -1 if no local event flag was available.
**      returns:  SS$_NORMAL - Event flag allocated
**                LIB$INSEF - If no more event flags available for allocation.
*/

unsigned int lib$get_ef(unsigned int *efn)			/* allocate an event flag */
    {
    *efn = (unsigned int)-1;					/* get_ef return -1 if not allocated */
    int bit;
    int flag;
    do
	{
	bit = ffsll(efn_flags);			/* Find the first free event flag */
	if (bit == 0) return LIB$_INSEF;			/* No more event flags */
	flag = test_and_clear_bit(bit-1, (volatile void *)&efn_flags);
								/* Get bit and clear - ATOMIC! */
	} while (!flag);					/* If set - an AST or thread beat me here */
    *efn = 64 - bit;						/* return the event flag allocated */
    return SS$_NORMAL;
    }

/*
**  lib$free_ef(const unsigned int *efn) - free an event flag
**
**     Event flags 0 and 24-31 are permantly reserved, and cannot be allocated or freed.
**     Event flags 1-23 are initially reserved, but can be freed, then allocated.
**     Event flags 32-63 are initially free and can be reserved.
**
**	returns:
**		SS$_NORMAL      - Routine successfully completed.
**		LIB$_EF_ALRFRE  - Event flag already free.
**		LIB$_EF_RESSYS  - Event flag reserved to system.
**			This is returned if efn is outside the ranges of 1-23 and 32-63.
*/

unsigned int lib$free_ef(const unsigned int *efn)		/* Free an event flag */
    {
    unsigned int status = LIB$_EF_RESSYS;			/* Event flag reserved to system */
    int ef = *efn;
    if ((ef >= 1 && ef <= 23) || (ef >= 32 && ef <= 63))	/* Make sure it's in range */
	{
	status = SS$_NORMAL;					/* Assume success */
	int flag = test_and_set_bit(63-ef, (volatile void *)&efn_flags);
								/* Get bit and set - ATOMIC! */
	if (flag) status = LIB$_EF_ALRFRE;			/* If already free */
	}
    return status;
    }

/*
**  lib$reserve_ef() - reserve a specific event flag number
**		To deallocate use LIB$FREE_EF.
**
**	returns:
**		SS$_NORMAL - Routine successfully completed.
**		LIB$_EF_ALRRES - event flag already reserved
**		LIB$_EF_RESSYS - Event flag reserved to system.
**			This is returned if efn is outside the ranges of 1-23 and 32-63.
*/

unsigned int lib$reserve_ef(const unsigned int *efn)
    {
    unsigned int status = LIB$_EF_RESSYS;			/* Event flag reserved to system */
    int ef = *efn;
    if ((ef >= 1 && ef <= 23) || (ef >= 32 && ef <= 63))	/* Make sure it's in range */
	{
	status = SS$_NORMAL;					/* Assume success */
	int flag = test_and_clear_bit(63-ef, (volatile void *)&efn_flags);
								/* Get bit and set - ATOMIC! */
	if (!flag) status = LIB$_EF_ALRRES;			/* If already reserved */
	}
    return status;
    }

#ifdef _vms
int ffsll(unsigned long long bits)
    {
    int pos = 0;
    for (int i = 0; i < 64; i++)
	{
	if (bits & 0x01) { pos = i+1; break; }
	bits = bits >> 1;
	}
    return pos;
    }
int test_and_clear_bit(int pos, void *bits)			/* Not atomic but good enough for testing */
    {
    unsigned long long *p = (unsigned long long *)bits;
    int test = (*p >> pos) & 0x01;				/* Get the bit */
    if (test) *p &= ~(1LL << pos);				/* Clear it */
    return test;
    }
int test_and_set_bit(int pos, void *bits)			/* Not atomic but good enough for testing */
    {
    unsigned long long *p = (unsigned long long *)bits;
    int test = (*p >> pos) & 0x01;				/* Get the bit */
    if (!test) *p |= (1LL << pos);				/* set it */
    return test;
    }
#endif

#ifdef TEST_ROUTINE
#include <stdio.h>

int main()
    {
    int i;
    for (i = 0; i < 65; i++)
      {
      unsigned int efn1 = i;
      unsigned int status = lib$free_ef(&efn1);
      printf("freed efn %d, status %d\n",efn1,status);
      }

    for (i = 0; i < 64; i++)
        {
        unsigned int efn1;
        unsigned int status = lib$get_ef(&efn1);
        printf("Got efn %d, status %d\n",efn1, status);
        }

    for (i = 0; i < 65; i++)
      {
      unsigned int efn1 = i;
      unsigned int status = lib$free_ef(&efn1);
      printf("freed efn %d, status %d\n",efn1,status);
      }
    for (i = 0; i < 65; i++)
      {
      unsigned int efn1 = i;
      unsigned int status = lib$free_ef(&efn1);
      printf("freed efn %d, status %d\n",efn1,status);
      }

    for (i = 0; i < 65; i++)
        {
        unsigned int efn1 = i;
	unsigned int status = lib$reserve_ef(&efn1);
        printf("Reserved efn %d, status = %d\n",efn1, status);
        }
    for (i = 0; i < 65; i++)
        {
        unsigned int efn1 = i;
	unsigned int status = lib$reserve_ef(&efn1);
        printf("Reserved efn %d, status = %d\n",efn1, status);
        }
    return 0;
    }
#endif
