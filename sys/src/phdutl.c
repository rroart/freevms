// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/config.h>
#include <linux/linkage.h>
#include <linux/sched.h>

#include <ssdef.h>
#include <va_rangedef.h>
#include <system_data_cells.h>
#include <rdedef.h>
#include <phddef.h>
#include <queue.h>
#include <mmg_routines.h>
#include <exe_routines.h>
#include <linux/slab.h>
#include <secdef.h>

#define DEBUG_LEAK
#undef DEBUG_LEAK

#ifdef DEBUG_LEAK
static int cnt = 0;
static int table[1024*8];
#endif

void mmg$imgreset()
{
    // rm$reset (in rmsreset)

    // exe$pscan_imgreset (process_can)

    // ws peak check to prev state

    // release icbs

#if 0
    // now delete all P0 space
    // but just up to 0-0x8000000 for now
    struct _va_range va;
    va.va_range$ps_start_va=0;
    va.va_range$ps_end_va=0x3f000000;
    exe$deltva(&va,0,0);
    va.va_range$ps_start_va=0x40000000;
    va.va_range$ps_end_va=0x60000000;
    exe$deltva(&va,0,0);
#endif
    // this may belong here? check internal books?
    struct _pcb * p=ctl$gl_pcb;
    struct _secdef *sec, *pstl;
    pstl=p->pcb$l_phd->phd$l_pst_base_offset;
    int next;
    while ((next = p->pcb$l_phd->phd$l_pst_free) >= 0)
    {
        sec=&pstl[next - 1];
        if (sec->sec$l_vpx >= 0x60000000)   // keep dcl parts
        {
            break;
        }
        p->pcb$l_phd->phd$l_pst_free--;
    }

    // should also remove related rdes?
    struct _rde * rde_head = &ctl$gl_pcb->pcb$l_phd->phd$ps_p0_va_list_flink;
    struct _rde * tmp = rde_head->rde$ps_va_list_flink;
#if 0
    printk("start %x\n",&rde_head->rde$ps_va_list_flink);
#endif
    while (tmp!=rde_head)
    {
        struct _rde * next=tmp->rde$ps_va_list_flink;
#if 0
        printk("start %x %x %x\n",tmp,tmp->rde$pq_start_va,tmp->rde$q_region_size);
#endif
        if (tmp->rde$pq_start_va == 0x3f000000 || tmp->rde$pq_start_va >= 0x40000000) goto cont; // temp fix
#if 0
#ifdef __i386__
#else
        if (tmp->rde$pq_start_va == 0x3f000000 || tmp->rde$pq_start_va >= 0x588000) goto cont; // temp fix
#endif
#endif
        if (tmp->rde$pq_start_va<0x60000000)
        {
            if (tmp->rde$pq_start_va==0 && tmp->rde$q_region_size==0)
            {
                printk("can not figure out where this gets inserted; can not kfree it either\n");
                printk("rde %x %x %x %x %x\n",rde_head, tmp,tmp->rde$pq_start_va,tmp->rde$q_region_size);
            }
            else
            {
#if 1
                struct _va_range va;
                va.va_range$ps_start_va = tmp->rde$pq_start_va;
                va.va_range$ps_end_va = tmp->rde$pq_start_va + tmp->rde$q_region_size;
                exe$deltva(&va,0,0);
#endif
                remque(tmp,0);
                kfree(tmp);
            }
        }
cont:
        tmp=next;
    }

    if (0)
    {
        // this will show eventual leaks, like the one in showmemory.c
        // one more rde each time show memory is used
        tmp = rde_head->rde$ps_va_list_flink;
        while (tmp!=rde_head)
        {
            struct _rde * next=tmp->rde$ps_va_list_flink;
            printk("remain rde %x %x %x %x %x\n",rde_head, tmp,tmp->rde$pq_start_va,tmp->rde$q_region_size);
            tmp=next;
        }
    }

    // delete nonpermanent P1

    // set ws list to default value

    // ipl to 2
    // mmg$sectblrst

    // reset priv mask
    ctl$gl_pcb->pcb$l_priv=ctl$gq_procpriv;

    ctl$gl_imghdrbf=0;

    // if last release more

    // set phd$w_wslast

#ifdef DEBUG_LEAK
    char * mytable=&table[1024*cnt];
    memset(mytable, 0, 1024*4);
    struct page * pg = pfn$al_head[0];
    int i = 0;
    do
    {
        i = pg - &mem_map[0];
        set_bit(i, mytable);
        pg = pg->pfn$l_flink;
    }
    while (pg && i<8192);

    if (cnt==0) goto dont;
    char * mytable2=&table[1024*(cnt-1)];
    for(i=0; i<128; i++)
    {
        if (mytable[i]!=mytable2[i])
        {
            int j;
            for(j=0; j<8; j++)
            {
                char c1 = mytable[i] & (1 << j);
                char c2 = mytable2[i] & (1 << j);
                if (c1 != c2)
                {
                    printk("%x000 ",i*8+j);
                }
            }
        }
    }
    printk("\n");
dont:
    cnt++;
#endif

}

/**
   \brief check process section table for deallocatable sections - see 5.2 15.4.1
*/

void mmg$dalcstxscn(void)
{
    /** MISSING content */
}
