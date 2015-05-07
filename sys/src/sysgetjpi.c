// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file sysgetjpi.c
 \brief system get job and process information
 \author Roar Thron�s
 */

#include <starlet.h>
#include <jpidef.h>
#include <ssdef.h>
#include <misc.h>
#include <pcbdef.h>
#include <phddef.h>
#include <pridef.h>
#include <exe_routines.h>
#include <sch_routines.h>

#include "ileutil.h"

// this behaves like getjpiw for now

/**
 \brief system service get job or process info - see 5.2 13.2.3
 \details make multiuser version
 */

asmlinkage int exe$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(),
                          unsigned long long astprm)
{
    struct _pcb *p;
    int status = SS$_NORMAL;
    struct _ile3 *ile = itmlst;

    if (!check_ile3(ile))
    {
        return SS$_ACCVIO;
    }
    /** use pidadr - MISSING */
    /** clear event flag */
    exe$clref(efn);
    /** find next pcb */
    if (!exe$pscan_next_id(&p))
    {
        return SS$_NOMOREPROC;
    }
    /** invoke nampid - MISSING */
    /** if other node, cwps - MISSING */
    /** iosb writecheck - MISSING */
    /** if ast, check ast quota and charge - MISSING */
    /** writetest for buffer descriptors - MISSING */
    /** gather some usual information - TODO still more remains */
    while (ile->ile3$w_code)
    {
        switch (ile->ile3$w_code)
        {
        case JPI$_PRCNAM:
            set_ile3_from_string(ile, p->pcb$t_lname);
            break;

        case JPI$_PID:
            set_ile3_from_unsigned_long(ile, p->pcb$l_epid);
            break;

        case JPI$_MASTER_PID:
            set_ile3_from_unsigned_long(ile, p->pcb$l_pid);
            break;

        case JPI$_STATE:
            set_ile3_from_unsigned_short(ile, p->pcb$w_state);
            break;

        case JPI$_PRI:
            set_ile3_from_unsigned_char(ile, p->pcb$b_pri);
            break;

        case JPI$_PAGEFLTS:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_pageflts);
            break;

        case JPI$_PPGCNT:
            set_ile3_from_unsigned_long(ile, p->pcb$l_ppgcnt);
            break;

        case JPI$_GPGCNT:
            set_ile3_from_unsigned_long(ile, p->pcb$l_gpgcnt);
            break;

        case JPI$_WSAUTH:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_wsauth);
            break;

        case JPI$_WSQUOTA:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_wsquota);
            break;

        case JPI$_WSSIZE:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_wssize);
            break;

        case JPI$_WSAUTHEXT:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_wsauthext);
            break;

        case JPI$_WSEXTENT:
            set_ile3_from_unsigned_long(ile, p->pcb$l_phd->phd$l_wsextent);
            break;

        case JPI$_BIOCNT:
            set_ile3_from_unsigned_short(ile, p->pcb$w_biocnt);
            break;

        case JPI$_BIOLM:
            set_ile3_from_unsigned_short(ile, p->pcb$w_biolm);
            break;

        case JPI$_DIOCNT:
            set_ile3_from_short(ile, p->pcb$w_diocnt);
            break;

        case JPI$_DIOLM:
            set_ile3_from_short(ile, p->pcb$w_diolm);
            break;

        case JPI$_CPUTIM:
            set_ile3_from_unsigned_int(ile, p->pcb$l_phd->phd$l_cputim);
            break;

        case JPI$_DIRIO:
            set_ile3_from_unsigned_int(ile, p->pcb$l_phd->phd$l_diocnt);
            break;

        case JPI$_BUFIO:
            set_ile3_from_unsigned_int(ile, p->pcb$l_phd->phd$l_biocnt);
            break;

        default:
            status = SS$_BADPARAM;
            break;
        }
        ile++;
    }

    /** post event flag */
    struct _pcb * pcb = ctl$gl_pcb;
    sch$postef(pcb->pcb$l_pid, PRI$_NULL, efn);

    /** eventual ast queue - MISSING */

    /** eventual iosb write */
    if (iosb)
    {
        iosb->iosb$w_status = status;
    }

    return status;

    /** handle target process, kast etc - MISSING */
    /** handle target process, status, state - MISSING */

}

asmlinkage int exe$getjpiw(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb,
                           void (*astadr)(), unsigned long long astprm)
{

    /* I think this is about it */

    int status = exe$getjpi(efn, pidadr, prcnam, itmlst, iosb, astadr, astprm);
    if ((status & 1) == 0)
        return status;
    return exe$synch(efn, iosb);

}

asmlinkage int exe$getjpi_wrap(struct struct_getjpi *s)
{
    return exe$getjpi(s->efn, s->pidadr, s->prcnam, s->itmlst, s->iosb, s->astadr, s->astprm);
}

asmlinkage int exe$getjpiw_wrap(struct struct_getjpi *s)
{
    return exe$getjpiw(s->efn, s->pidadr, s->prcnam, s->itmlst, s->iosb, s->astadr, s->astprm);
}

