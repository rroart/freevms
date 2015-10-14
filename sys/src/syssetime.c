// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file syssetime.c
 \brief set system time
 \author Roar Thron�s
 */

#include <linux/linkage.h>
#include <linux/string.h>

#include <exe_routines.h>
#include <ssdef.h>
#include <system_data_cells.h>

/* Author: Roar Thron�s */

/**
 \brief set new system time - see 5.2 11.4
 */

asmlinkage int exe$setime(struct _generic_64 *timadr)
{
    /** test privs - MISSING */
    /** TODO wrong use */
    /** if zero, recalibrate - MISSING */
    if ((unsigned long) timadr < 4096)
    {
        return SS$_ACCVIO;
    }
    /** test capability - MISSING */
    memcpy(&exe$gq_systime, timadr, sizeof(struct _generic_64));
    return SS$_NORMAL;
}
