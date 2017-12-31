// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysascefc.c
   \brief associate event flags
   \author Roar Thronæs
 */

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<ssdef.h>
#include<starlet.h>
#include<cebdef.h>
#include<pridef.h>
#include<statedef.h>
#include<evtdef.h>
#include<ipldef.h>
#include<descrip.h>
#include <system_data_cells.h>
#include <internals.h>
#include <misc_routines.h>
#include <exe_routines.h>
#include <queue.h>
#include <linux/vmalloc.h>

// Author. Roar Thronæs.

/**
   \brief associating to a common event flag cluster - See 5.2 9.3
   \details Does not do mutex lock, and no access check regarding uic and group check.
   No priv check, no tqe quota. No sch$gw_cebcnt.
   Does not set protection and permanent flag.
   \param efn event flag number
   \param name event flag name
   \param prot protection
   \param perm permanent
*/

asmlinkage int exe$ascefc(unsigned int efn, void *name, char prot, char perm)
{
    int sts;
    struct _ceb * first=&sch$gq_cebhd;
    struct _ceb * tmp=first->ceb$l_cebfl;
    struct _ceb * c;
    struct _pcb * p;
    unsigned long * efcp;
    int found=1;
    /** test event flag range */
    if (efn<64 || efn >127)
        return SS$_ILLEFC;
    /** test legal logical */
    if (((struct dsc$descriptor *)name)->dsc$w_length==0)
        return SS$_IVLOGNAM;
    if (((struct dsc$descriptor *)name)->dsc$w_length>15)
        return SS$_IVLOGNAM;
    /** lock ceb mutex - MISSING */
    /** search ceb list */
    while (tmp!=first)
    {
        //    int res;
        //printk("%x %x %x %x %x %x \n",((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1], strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]),((struct dsc$descriptor *)name)->dsc$w_length,tmp->ceb$t_efcnam[0],((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0]);
        // res=strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]);
        if (((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0] && strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0])==0) goto out;
        tmp=tmp->ceb$l_cebfl;
        //res=strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]);
    }
    found=0;
out:
    /** access check - MISSING */
    if (!found)
    {
        /** if permanent check privs - MISSING */
        /** if not permanent charge tqe quota - MISSING */
        /** create ceb cluster */
        int alosize;
        sts=exe_std$allocceb(&alosize,&c);
        if (sts!=SS$_NORMAL)
            return sts;
        qhead_init(&c->ceb$l_wqfl);
        c->ceb$l_state=SCH$C_CEF;
        c->ceb$t_efcnam[0]=((struct dsc$descriptor *)name)->dsc$w_length;
        memcpy(&c->ceb$t_efcnam[1],((struct dsc$descriptor *)name)->dsc$a_pointer,c->ceb$t_efcnam[0]);
        /** incr cebcnt, set perm flag, set prot flag - MISSING */
        insque(c,first);
        tmp=c;
    }
    tmp->ceb$l_refc++; // do a adawi?
    p=current;
    efcp=getefcp(p,efn);
    *efcp=tmp;
    /** unlock mutex - MISSING */
    return SS$_NORMAL;
}

/**
   \brief dissociating to a common event flag cluster - See 5.2 9.4
   \details Does not do mutex lock, and no access check regarding uic and group check.
   No priv check, no tqe quota. No sch$gw_cebcnt.
   Does not set protection and permanent flag.
   The use of vfree indicates very early code. Change.
   \param efn event flag number
*/

asmlinkage int exe$dacefc(unsigned int efn)
{
    unsigned long * efcp;
    struct _pcb * p;
    int retval = SS$_NORMAL;
    struct _ceb * c;
    /** test event flag range */
    if (efn<64 || efn >127)
        return SS$_ILLEFC;
    /** lock ceb mutex - MISSING */
    p=current;
    /** test if it has associated cluster */
    efcp=getefcp(p,efn);
    if (!(*efcp))
    {
        retval=SS$_ILLEFC;
        goto end;
    }
    /** locate and clear cluster */
    c=*efcp;
    *efcp=0;
    /** decr refc */
    c->ceb$l_refc--;
    /** if temporary etc and no associated process, delete cluster */
    if (!c->ceb$l_refc && aqempty(c->ceb$l_wqfl))
    {
        /** if noquota return job quota - MISSING */
        /** remove from ceb list */
        remque(c,c);
        /** decr cebcnt - MISSING */
        /** vfree - change */
        vfree(c);
    }
end:
    /** unlock ceb mutex - MISSING */
    return retval;
}

/**
   \brief deleting a common event flag cluster - See 5.2 9.5
   \details Does not do mutex lock, and no access check regarding uic and group check.
   No priv check, no tqe quota. No sch$gw_cebcnt.
   Does not set protection and permanent flag.
   The use of vfree indicates very early code. Change.
   \param name event flag name
*/

asmlinkage int exe$dlcefc(void *name)
{
    struct _ceb * first=((struct _ceb *)sch$gq_cebhd)->ceb$l_cebfl;
    struct _ceb * tmp=sch$gq_cebhd;
    struct _ceb * c;
    int retval;
    int found=1;
    /** lock ceb mutex - MISSING */
    /** scan ceb list */
    while (tmp!=first)
    {
        if (((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0] && !strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0])) goto out;
        tmp=tmp->ceb$l_cebfl;
    }
    found=0;
out:

    if (!found)
        goto end;
    /** test protection and privs - MISSING */
    c=tmp;

    /* some unsupported stuff */

    /* overlaps with dacefc. copy. */

    /** if not deleting temporary, set noquota and clear perm flags */
    /** if temporary etc and ... */
    if (!c->ceb$l_refc && aqempty(c->ceb$l_wqfl))
    {
        /** remove from ceb list */
        remque(c,c);
        /** vfree - change */
        vfree(c);
    }

    retval=SS$_NORMAL;

end:
    {
    }
    /** unlock ceb mutex - MISSING */
}

