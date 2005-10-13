/*
** ile.c - Used to test out item list routines
*/

#define __NEW_STARLET

#include <stdio.h>
#include <string.h>
#include <iledef.h>			/*  Item List Entry Definitions  */ 
#include <lib$routines.h>
#include <ssdef.h>
#include <stsdef.h>
#ifdef __vms
#include <builtins.h>
#else
#include <stdlib.h>
#define lib$signal(a) exit(a)
#endif

inline int exe_std$probew(const void *p, unsigned long long size, int mode)
    {
    unsigned int status = SS$_NORMAL;
//    if (!__PAL_PROBEW(p,size-1,mode)) status = SS$_ACCVIO;
    return status;
    }

inline int exe_std$prober(const void *p, unsigned long long size, int mode)
    {
    unsigned int status = SS$_NORMAL;
//    if (!__PAL_PROBER(p,size-1,mode)) status = SS$_ACCVIO;
    return status;
    }

/*
**
** parse_ile() - This routine parses a 32 bit ile3 or a 64 bit ileb item list.
**		  The goal is that this routine can be used for all sys$get*() system services
**		  that parse item lists, so it's done in one place and done correctly.
**
**  Author: Roger Tucker
**
**  VMS return item lists (32 bit ile3, or 64 bit ileb) structures come in arrays of the following forms:
**  Item lists can exist in 64 bit addresses, and a 32 bit item list can be chained to a 64 bit item
**  list or a 64 bit item list can be chained to a 32 bit item list.
**
**	         32 bit ile3						64 bit ileb
**	+-------------------------------+		+-----------------------------------------------+
**	|  ile3$w_code	| ile3$w_length	| 0		|   ileb_64$$w_code	| ileb_64$w_mbo (1)	| 0
**	+-------------------------------+		+-----------------------------------------------+
**      |       ile3$ps_bufaddr		| 4		|		ileb_64$l_mbmo (-1) 		| 4
**	+-------------------------------+		+-----------------------------------------------+
**	|       ile3$ps_retlen_addr	| 8		|		ileb_64$q_length		| 8
**	+-------------------------------+		|						|
**							+-----------------------------------------------+
**	It takes a minium of 3 memory probes		|		ileb_64$pq_bufaddr		| 16
**	for the first item and 2 probes			|						|
**	for each item after that:			+-----------------------------------------------+
**	    1.  the first word (to get the code)	|		ileb_64$pq_retlen_addr		| 24
**	    2.  to get the bufaddr/mbmo			|						|
**	    3.  everything else plus the next code	+-----------------------------------------------+
**
**	This routine loops through theses structures, probes them to make sure they are readable,
**	parses them, verify's the return buffer is writable.  This routine handles chained item lists
**	(where the code is -1) and returns the item list information to the user.
**	This routine was intended to be used for kernel mode code that must parse item lists.
**	My hope is that this can be used everywhere 32 bit ile3 or 64 bit ileb structures are used.
**
**	Status values:
**		SS$_NORMAL	- successful section creation
**		SS$_ACCVIO 	- one of the arguments cannot be read/written.
**		SS$_BADPARAM	- element types of item list must be the same
**
**	Before calling this routine, make sure the item list address is not zero
**	and probe the first word of the item list.  Also, clear the chain_cnt counter
**	and set the itm_type to -1.  User-mode item lists are most likely pageable, so do not
**	parse user-mode item lists above ipl ASTDEL.
**
*/

int parse_ile(const void **itmp,				/* Item list work pointer */
    int mode,							/* Mode to do probes in */
    int *itm_code,						/* item code - returned */
    int *itm_type,						/* item type - start with -1, unknown */
    void **bufaddr,						/* address of buffer - returned */
    unsigned long long *buflen,					/* buffer length - returned */
    void **retlen_addr,						/* address of itm32, or itm64 return length */
    int *chain_cnt)						/* number of chained items, start with zero */
    {
    int status = SS$_NORMAL;					/* End of list by default */
    int type = *itm_type;					/* Get local copy */
    const struct _ile3 *itm = *itmp;				/* Cast */
    int code = itm->ile3$w_code;				/* Get the item code - same for 32/64 bit items */
    while (code)						/* While I have an item code */
	{
	status = exe_std$prober(&itm->ile3$ps_bufaddr, sizeof(itm->ile3$ps_bufaddr), mode);
	if (!$VMS_STATUS_SUCCESS(status)) break;		/* Probe the address (mbmo for 64 bits) */
	int flag = $is_itmlst64(itm);				/* Get the item list type */
	if (type == -1) type = flag;				/* First time - save the item type */
	if (flag != type) { status = SS$_BADPARAM; break; }	/* Items must the same type as the first one */

	/*
	** Already examined 8 bytes up to this point,
	**  so examine the rest of item list, plus the next item's first word (item code)
	*/

	int probe_len = ILE3$K_LENGTH-8+4;			/* Assume 32 bit item list  */
	if (type) probe_len = ILEB_64$K_LENGTH-8+4;		/* nope - must be 64 bit */
	status = exe_std$prober(&itm->ile3$ps_retlen_addr, probe_len, mode);
	if (!$VMS_STATUS_SUCCESS(status)) break;		/* check accessibility of rest plus next item code */

	int retlen;						/* Length of return length addres */
	if (type)						/* True for 64 bit item lists */
	    {
	    struct _ileb_64 *tmp = (void *)itm;			/* Get 64 bit information */
	    *buflen = tmp->ileb_64$q_length;
	    *bufaddr = tmp->ileb_64$pq_bufaddr;			/* get 64 bit buffer address */
	    *retlen_addr = tmp->ileb_64$pq_retlen_addr;
	    retlen = sizeof(unsigned long long);		/* Size of return length */
	    itm = (void *)((char *)itm + ILEB_64$K_LENGTH);	/* move to next item */
	    }
	else							/* Must be 32 bit item list */
	    {
	    *buflen = itm->ile3$w_length;			/* Length (or mbo for 64 bits) */
	    *bufaddr = itm->ile3$ps_bufaddr;			/* Get the buffer address */
	    *retlen_addr = itm->ile3$ps_retlen_addr;
	    retlen = sizeof(unsigned short);			/* Size of return length for probe */
	    itm = (void *)((char *)itm + ILE3$K_LENGTH);	/* move to next item */
	    }
	if (code == (unsigned short)-1)				/* Handle chained item lists */
	    {							/* Normally -1 in the item code */
	    if (*chain_cnt > 1024) { status = SS$_BADPARAM; break; }
								/* Must be looping */
	    (*chain_cnt)++;
	    itm = *bufaddr;					/* Goto the new item list */
	    status = exe_std$prober(itm, 4, mode);
	    if (!$VMS_STATUS_SUCCESS(status)) break;		/* See if first item (len/code) can be read */
	    type = -1;						/* Not sure about the item type anymore */
	    code = itm->ile3$w_code;				/* Loop around and try again */
	    }
	else
	    {
	    if (*retlen_addr)					/* If we have a return length address */
		{						/* Make sure we can write to it */
		status = exe_std$prober(&itm->ile3$ps_retlen_addr, probe_len, mode);
		if (!$VMS_STATUS_SUCCESS(status)) break;	/* If return length make sure it's writable */
		}
	    /* Note: we probe the whole buffer even though we may only write to some of it */
	    status = exe_std$probew(*bufaddr,*buflen,mode);	/* Make sure buffer is writable */
	    if ($VMS_STATUS_SUCCESS(status))			/* If success */
		{						/* Return some values for next time */
		*itm_type = type;				/* Save current type */
		*itmp = itm;					/* Return where we left off */
		}
	    break;						/* We are done! */
	    }
	} /* End while */
    *itm_code = code;
    return status;
    }

/*
** store_retlen() - used to return the return length
*/

inline void store_retlen(int type, void *retlen_addr, unsigned long long retlen)
    {
    if (retlen_addr)						/* If they want the return length */
	{
	if (type)						/* True for 64 bit item lists */
	    *(unsigned long long *)retlen_addr = retlen;	/* Write a 64 bit return length */
	else
	    *(unsigned short *)retlen_addr = retlen;		/* Write a 16 bit return length */
	}
    }

unsigned int test_ile(void *itmlst)
    {
    int callers_mode = 0;
    unsigned int status = SS$_NORMAL;
    int itm_code;						/* Item code to return information for */
    void *bufaddr;						/* buffer address to write data to */
    unsigned long long buflen;					/* length of buffer to write to */
    void *retlen_addr;						/* 32/64 bit item list return length address */
    int chain_cnt = 0;						/* Number of item lists chained together */
    int itm_type = -1;						/* Item list type is unknown */
    const void *itm = itmlst;					/* Work pointer for item list */
    while ($VMS_STATUS_SUCCESS(status))				/* Continue until error or zero item list */
	{
	int pstatus = parse_ile(&itm, callers_mode, &itm_code, &itm_type, &bufaddr, &buflen,
	    &retlen_addr, &chain_cnt);
	if (!$VMS_STATUS_SUCCESS(pstatus))
	    {	
	    status = pstatus;					/* SS$_BUFFEROVF - may be returned from switch */
	    break;
	    }
	if (!itm_code) break;					/* End of item list */
printf("code = %d, type = %d, buffer = %p, buflen = %Ld, retlen = %p\n",
		itm_code,itm_type,bufaddr,buflen,retlen_addr);
	switch (itm_code)
	    {
	    case 1:					/* Return reference count */
		if (buflen >= sizeof(unsigned long)) 
		    {
		    *(unsigned long *)bufaddr = 1;
		    store_retlen(itm_type, retlen_addr, sizeof(unsigned long));
		    }
		else
		    status = SS$_BADPARAM;
		break;
	    case 2:
		break;
	    default:
		status = SS$_BADPARAM;
		break;
	    }
	}	/* End while */
    return status;
    }

/*
** notes:  Different get() system services on openVMS work differently, for example:
**	for getjpi(), All values are moved zero filled to the output buffer (movc5)
**	    If the buffer is too small only the low order bytes are copied, no sign extending, etc.
**	    This works OK with positive values on a little endian box!
**	for trnlnm() returns SS$_BUFFEROVF if not room, and only writes the right number of bytes (movc3)
**	    other buffers have to be the correct size or SS$_BADPARAM is returned.  This is the way I prefer.
**	The return size is never more than the max size.
**	SS$_INSFMEM - vms returns this for calls to EXE$ALONONPAGED
*/

int main(void)
    {
    char buffer[10] = "TESTING";
    struct _ileb_64 ile64[3];

    /*
    ** Test out 32 bit item lists
    */

    int test = 0;
    unsigned short int retlen = 0;
    struct _ile3 ile[4];

    ile[0].ile3$w_code = 1;					/* Return reference count */
    ile[0].ile3$w_length = sizeof(test);
    ile[0].ile3$ps_bufaddr = &test;
    ile[0].ile3$ps_retlen_addr = &retlen;

    ile[1].ile3$w_code = 2;
    ile[1].ile3$w_length = sizeof(buffer);
    ile[1].ile3$ps_bufaddr = buffer;
    ile[1].ile3$ps_retlen_addr = &retlen;

    ile[2].ile3$w_code = -1;
    ile[2].ile3$w_length = -1;
    ile[2].ile3$ps_bufaddr = ile64;
    ile[2].ile3$ps_retlen_addr = 0;

    ile[3].ile3$w_code = 0;

    /*
    ** Test out 64 bit item lists
    */

    int test2 = 0;
    unsigned long long retlen64 = 6;
    ile64[0].ileb_64$w_code = 1;
    ile64[0].ileb_64$w_mbo = 1;
    ile64[0].ileb_64$l_mbmo = -1;
    ile64[0].ileb_64$q_length = sizeof(test2);
    ile64[0].ileb_64$pq_bufaddr = &test2;
    ile64[0].ileb_64$pq_retlen_addr = &retlen64;

    ile64[1].ileb_64$w_code = 2;
    ile64[1].ileb_64$w_mbo = 1;
    ile64[1].ileb_64$l_mbmo = -1;
    ile64[1].ileb_64$q_length = sizeof(buffer);
    ile64[1].ileb_64$pq_bufaddr = buffer;
    ile64[1].ileb_64$pq_retlen_addr = &retlen64;

    ile64[2].ileb_64$w_code = 0;

    unsigned int status = test_ile(&ile[0]);
    lib$signal(status);

    return 0;
    }
