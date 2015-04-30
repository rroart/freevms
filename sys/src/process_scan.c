// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file process_scan.c
 \brief scan process
 \author Roar Thron�s
 */

#include <linux/sched.h>
#include <system_data_cells.h>

/** system service process_scan - MISSING */

// we will need struct _pscanctx soon
unsigned long pscan_id = 2;

/**
 \brief get next pcb
 \details fix implementation. make multiuser version
 */

int exe$pscan_next_id(struct _pcb ** p)
{
    int i;
    unsigned long *vec = sch$gl_pcbvec;
    for (i = pscan_id; i < MAXPROCESSCNT; i++)
    {
        if (vec[i] == 0)
            continue;
        {
            struct _pcb *p = vec[i];
            if (p == init_tasks[p->pcb$l_cpu_id])
                continue;
        }
        *p = vec[i];
        pscan_id++;
        return 1;
    }
    pscan_id = 2;
    return 0;
}
