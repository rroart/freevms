// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <syidef.h>
#include <ssdef.h>
#include <misc.h>
#include <pridef.h>
#include <sbdef.h>
#include <starlet.h>
#include <exe_routines.h>
#include <sch_routines.h>

#include <linux/version.h>

#include "ileutil.h"

extern struct _sb mysb;

asmlinkage int exe$getsyi(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb,
                          void (*astadr)(), unsigned long astprm)
{
    int status = SS$_NORMAL;
    struct _ile3 *ile = (struct _ile3 *)itmlst;

    if (!check_ile3(ile))
    {
        return SS$_ACCVIO;
    }

    // no use of namcsid yet
    if (iosb)
    {
        // check iosb write access
        memset(iosb, 0, sizeof(struct _iosb));
    }
    exe$clref(efn);
    // check ast quota

    while (ile->ile3$w_code)
    {
        switch (ile->ile3$w_code)
        {
        case SYI$_VERSION:
            set_ile3_from_string(ile, FREEVMS_RELEASE);
            break;

        case SYI$_SCSNODE:
            set_ile3_from_string(ile, mysb.sb$t_nodename);
            break;

        case SYI$_BOOTTIME:
            set_ile3_from_unsigned_long_long(ile, exe$gq_boottime);
            break;

        case SYI$_MAXBUF:
            set_ile3_from_unsigned_long(ile, ioc$gw_maxbuf);
            break;

        case SYI$_LASTFLD:
            set_ile3_from_unsigned_int(ile, 1);
            break;

        default:
            status = SS$_BADPARAM;
            break;
        }
        ile++;
    }

    struct _pcb * pcb = ctl$gl_pcb;
    sch$postef(pcb->pcb$l_pid, PRI$_NULL, efn);

    if (iosb)
    {
        iosb->iosb$w_status = status;
    }

    return status;
}

asmlinkage int exe$getsyiw(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb,
                           void (*astadr)(), unsigned long astprm)
{
    int status = exe$getsyi(efn, csidadr, nodename, itmlst, iosb, astadr, astprm);
    if ((status & 1) == 0)
    {
        return status;
    }
    return exe$synch(efn, iosb);
}

asmlinkage int exe$getsyiw_wrap(struct struct_args * s)
{
    return exe$getsyiw((unsigned int) (s->s1), (unsigned int *) (s->s2), (void *) (s->s3), (void *) (s->s4),
                       (struct _iosb *) (s->s5), s->s6, (unsigned long) (s->s7));
}

asmlinkage int exe$getsyi_wrap(struct struct_args * s)
{
    return exe$getsyi((unsigned int) (s->s1), (unsigned int *) (s->s2), (void *) (s->s3), (void *) (s->s4),
                      (struct _iosb *) (s->s5), s->s6, (unsigned long) (s->s7));
}
