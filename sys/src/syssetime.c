// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file syssetime.c
   \brief set system time
   \author Roar Thronæs
*/

#include<linux/linkage.h>
#include <linux/string.h>
#include <system_data_cells.h>	/* SYS$ header file */
#include <sys$routines.h>	/* SYS$ header file */
#include <ssdef.h>	/* SYS$ header file */

/* Author: Roar Thronæs */

/**
   \brief set new system time - see 5.2 11.4
*/

asmlinkage int exe$setime(unsigned long long *timadr)
{
    /** test privs - MISSING */
    long * tmp=(long *)&exe$gq_systime;
    /** TODO wrong use */
    /** if zero, recalibrate - MISSING */
    if (!timadr) return SS$_ACCVIO;
#if 0
    printk("setime %x %x\n",tmp[0],tmp[1]);
#endif
    /** test capability - MISSING */
    memcpy(&exe$gq_systime,timadr,8);
#if 0
    printk("setime %x %x\n",tmp[0],tmp[1]);
#endif
    return SS$_NORMAL;
}
