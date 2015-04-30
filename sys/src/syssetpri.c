// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file syssetpri.c
 \brief system service set priority
 \author Roar Thron�s
 */

#include <linux/unistd.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <starlet.h>
#include <ssdef.h>
#include <evtdef.h>
#include <pridef.h>
#include <exe_routines.h>
#include <sch_routines.h>

/* Author: Roar Thron�s */

asmlinkage int exe$setpri_wrap(struct struct_setpri *s)
{
	return exe$setpri(s->pidadr, s->prcnam, s->pri, s->prvpri, s->pol,
			s->prvpol);
}

/**
 \brief set priority - see 5.2 13.4.1
 */

asmlinkage int exe$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri,
		unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol)
{
	struct _pcb *p;
	struct _pcb * retpcb;
	unsigned long ipid, epid;
	int sts;
	/** locate process by nampid */
	sts = exe$nampid(current, pidadr, prcnam, &retpcb, &ipid, &epid);
	p = retpcb;

	/** cwps - MISSING */
	if (!p)
	{
		p = current;
	}

	/** set prisav, pribsav - MISSING */
	/** set prib */
	p->pcb$b_prib = 31 - pri;

	/** if current process, call change cur priority */
	if (p == current)
	{
		sch$change_cur_priority(p, p->pcb$b_prib);
	}

	/** call rse */
	sch$rse(p, PRI$_IOCOM, EVT$_SETPRI);

	return SS$_NORMAL;
}
