// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file iociopost.c
   \brief QIO I/O postprocessing - TODO still more doc
   \author Roar Thron�s
*/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <cxbdef.h>
#include <bufiodef.h>
#include <dyndef.h>
#include <irpdef.h>
#include <acbdef.h>
#include <ipldef.h>
#include <pridef.h>
#include <ipl.h>
#include <phddef.h>
#include <system_data_cells.h>
#include <internals.h>
#include <rsndef.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <sch_routines.h>
#include <queue.h>
#include <linux/slab.h>
#include <statedef.h>

/**
   \brief free single buffer or buffer chain
   \param d buffer
*/

kfreebuf(void * d)
{
    struct _bufio * bd = d;
    struct _cxb * cx = d;
    switch (bd->bufio$b_type)
    {
    case DYN$C_BUFIO:

        //    kfree(bd->bufio$ps_pktdata); not necessary?
        kfree(bd);
        break;

    case DYN$C_CXB:
        while (cx)
        {
            void * next=cx->cxb$l_link;
            kfree(cx->cxb$ps_pktdata);
            kfree(cx);
            cx=next;
        }
        break;

    default:
        panic("kfreebuf\n");
    }
}

/**
   \brief move i/o buffer data
   \details may be single buffer or chain, then free the buffer
   \param i irp containing the buffer
*/

movbuf(struct _irp * i)
{
    // still skipping access checks and such
    if (i->irp$l_svapte==&i->irp$l_svapte)
    {
        printk("self ref svapte\n");
        return;
    }
    struct _bufio * bd = i->irp$l_svapte;
    struct _cxb * cx = bd;

    if (bd==0) return;
    if (i->irp$l_bcnt==0) goto end;

    switch (bd->bufio$b_type)
    {
    case DYN$C_BUFIO:

        if (bd->bufio$w_size==0) goto end;
        if (bd->bufio$ps_uva32==0) goto end;
        memcpy(bd->bufio$ps_uva32,bd->bufio$ps_pktdata,i->irp$l_bcnt);
        break;

    case DYN$C_CXB:
        while (cx)
        {
            if (cx->cxb$w_length==0) goto skip;
            if (cx->cxb$ps_uva32==0) goto skip;
            memcpy(cx->cxb$ps_uva32,cx->cxb$ps_pktdata,cx->cxb$w_length);
skip:
            cx=cx->cxb$l_link;
        }
        break;

    default:
        panic("movbuf\n");
    }

end:
    kfreebuf(bd);
    i->irp$l_svapte = 0;
}

/**
   \brief dirpost entry point
   \details TODO restructure
 */

dirpost(struct _irp * i)
{
    printk("doing dirpost\n");
}

/**
   \brief buffered read completion - see 5.2 21.7.3.1
   \details dirpost may be mixed into this, too? TODO check
   \param i irp
*/

bufpost(struct _irp * i)
{
    struct _acb * a=(struct _acb *) i;
    struct _pcb * pcb = ctl$gl_pcb;
    struct _phd * phd = pcb->pcb$l_phd;
    //printk("doing bufpost\n");
    /* do iosb soon? */

    /** TODO common entrypoint and if bufio bit set only do bufio part? */
    /** increment biocnt TODO redo */
    if (i->irp$l_sts & IRP$M_BUFIO)
        phd->phd$l_biocnt++;
    else
        phd->phd$l_diocnt++;

#define         IO$_WRITEPBLK           11
#define         IO$_WRITELBLK           32
#define         IO$_WRITEVBLK           48
    int fcode = i->irp$l_func&63;
    /** 21.7.3.1 buffered read completion */
    /** a little workaround, must be there, but forgot most of it TODO */
    /** TODO maybe because some dirio ended up here? TODO check driver configs */
    /** invoke movbuf */
    int skipmovbuf = (fcode==IO$_WRITEPBLK) || (fcode==IO$_WRITELBLK) || (fcode==IO$_WRITEVBLK); // temp workaround
    if (skipmovbuf)
    {
        if (i->irp$l_svapte && i->irp$l_svapte!=&i->irp$l_svapte)
        {
            kfree(i->irp$l_svapte); // check. sufficient?
            i->irp$l_svapte = 0;
        }
    }
    else
        movbuf(i);

    /** if mailbox read, call ravail. TODO check read func */
    if (i->irp$l_sts&IRP$M_MBXIO)
        sch_std$ravail(RSN$_MAILBOX);

    /** dirpost to begin here? */
    /** 21.7.3.2 common completion */

    /** should be either of these iocnt increments */
    phd->phd$l_biocnt++;
    phd->phd$l_diocnt++;

    /** copy eventual diagnostic with movbuf - MISSING */

    /** decr ccb$w_ioc - MISSING */

    /** if last channel i/o and deaccess pending, call ioc$wakacp - MISSING */

    /** set iosb iost etc */
#ifdef __i386__
    if (i->irp$l_iosb)
    {
        memcpy(i->irp$l_iosb,&i->irp$l_iost1,8);
    }
#else
    if (i->irp$l_iosb)
    {
        memcpy(i->irp$l_iosb,&i->irp$l_iost1,4);
        memcpy(((long)i->irp$l_iosb)+4,&i->irp$l_iost2,4);
    }
#endif

    /** do an eventual setting of common event flag - MISSING */

    /** deallocate irpes, if any - MISSING? */

    /** check for quota bit set, then do ast - MISSING TODO redo */

    if (a->acb$l_ast)
    {
        a->acb$b_rmod&=~ACB$M_KAST;
        a->acb$b_rmod&=~ACB$M_NODELETE;
        /** post event flag */
        sch$postef(pcb->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
        /** queue ast */
        sch$qast(i->irp$l_pid,PRI$_NULL,i);
    }
    else
        kfree(i);
    /** otherwise free irp */

    /** return to sch$astdel */
}

/**
   \brief I/O postprocessing - se 5.2 21.7
*/

asmlinkage void ioc$iopost(void)
{
    struct _irp * i = 0;
    struct _pcb * p;

#ifdef __x86_64__
    if (intr_blocked(IPL$_IOPOST))
        return;
    regtrap(REG_INTR, IPL$_IOPOST);
#endif

    /** set ipl */
    setipl(IPL$_IOPOST);
    if (ctl$gl_pcb->pcb$l_cpu_id != smp$gl_primid)
        printk("iociopost 1\n");

    //printk("iopost %x %x %x %x\n",&ioc$gq_postiq,ioc$gq_postiq,current->pid,ioc$gq_postiq>>32); //,&ioc$gq_postiq,ioc$gq_postiq &ioc$gq_postiq,ioc$gq_postiq);
again:
    {}
#if 0
    int savipl2 = vmslock(&SPIN_IOPOST, 8);
#else
    /** extra global cli TODO check why and still needed? */
    __global_cli();
#endif
    /** fetch irp from postprocessing queues */
    if (ctl$gl_pcb->pcb$l_cpu_id == smp$gl_primid && !rqempty(&ioc$gq_postiq))
    {
        i=remqhi(&ioc$gq_postiq,i);
#if 0
        vmsunlock(&SPIN_IOPOST, savipl2);
#else
        __global_sti();
#endif
    }
    else
    {
#if 0
        vmsunlock(&SPIN_IOPOST, savipl2);
#else
        __global_sti();
#endif
        if (!aqempty(&smp$gl_cpu_data[smp_processor_id()]->cpu$l_psfl))
        {
            i=remque(smp$gl_cpu_data[smp_processor_id()]->cpu$l_psfl,i);
        }
        else
        {
            return;
        }
    }
    /** 21.7.1 check if negative irp pid (a routine), then perform system i/o completion and call this - MISSING */

    /** 21.7.2 normal i/o completion */
    p=exe$ipid_to_pcb(i->irp$l_pid);

#if 0
    // these two sch should not be here permanently
    sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
    sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);

    return; // the rest is not finished
#endif

    /** checks bufio flag */
    if (i->irp$l_sts & IRP$M_BUFIO) goto bufio;

dirio:
    /** 21.7.2.2 buffered i/o completion */

    /** maybe do ioc$mapvblk - MISSING */
    /** do mmg$unlock - MISSING */
    /** test present irpes - MISSING */
    /** increment diocnt - MISSING? */
    /** store bufpost ast in irp and use kast */
    i->irp$b_rmod|=ACB$M_KAST;
    if (i->irp$l_ast)
        i->irp$b_rmod|=ACB$M_NODELETE;
    ((struct _acb *) i)->acb$l_kast=dirpost;
    // not this? ((struct _acb *) i)->acb$l_astprm=i;
    /* find other class than 1 */
    /** 21.7.2.3 final steps in ioc$iopost - TODO restructure, double */
    /** spinlock */
    int savipl=vmslock(&SPIN_SCHED,IPL$_SYNCH);
    if (p->pcb$w_state != SCH$C_CUR)   // internals book says change order
    {
        sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
        sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);
    }
    else
    {
        sch$qast(i->irp$l_pid,PRI$_IOCOM,i);
        sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
    }
    /** spinunlock */
    vmsunlock(&SPIN_SCHED,savipl);
    /** reiterate */
    goto again;

bufio:
    /** 21.7.2.1 buffered i/o completion */
    /** increment biocnt - MISSING? */
    /** if filacp set also incr diocnt - MISSING */
    /** invoke credit_bytcnt - MISSING */

    /** store bufpost ast in irp and use kast */
    i->irp$b_rmod|=ACB$M_KAST;
    if (i->irp$l_ast)
        i->irp$b_rmod|=ACB$M_NODELETE;

    // put ioc$bufpost here?
    ((struct _acb *) i)->acb$l_kast=bufpost;
    // not this?  ((struct _acb *) i)->acb$l_astprm=i;
    /* find other class than 1 */
    /** 21.7.2.3 final steps in ioc$iopost - TODO restructure, double */
    /** spinlock */
    savipl=vmslock(&SPIN_SCHED,IPL$_SYNCH);
    if (p->pcb$w_state != SCH$C_CUR)   // internals book says change order
    {
        sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
        sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);
    }
    else
    {
        sch$qast(i->irp$l_pid,PRI$_IOCOM,i);
        sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
    }
    /** spinunlock */
    vmsunlock(&SPIN_SCHED,savipl);
    /** reiterate */
    goto again;

    ioc$bufpost(0);
}

/**
   \details needed by end of file system dispatcher
 */

ioc$bufpost(struct _irp * i)
{
    i->irp$b_rmod|=ACB$M_KAST; // think this belongs here too
    i->irp$l_wind=bufpost; // really acb$l_kast;
    i->irp$l_astprm=i; // think this belongs here too
}

#if 0
void ioc$myiopost(struct _pcb * p,unsigned long priclass)
{
    sch$postef(p->pcb$l_pid,priclass,0);
}
#endif
