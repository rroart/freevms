// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <ssdef.h>
#include <asm/atomic.h>
#include <linux/tqueue.h>
#include <mmg_routines.h>

asmlinkage int exe$rundwn(int mode)
{

    // clear requested powerfail AST

    // run rundown routines

    // setrwm

    // more rundown routines

    // call lmf

    // cpu capability

    // account log

    // phd$l_imgcnt++;

    // put rsb in 4 p1 space vectors for user-written system services, rundown
    // routines and image-specific message sections

    // setpfm off

    // do dassigns

    // dealloc rights identifier

    // cantim
    // canwak

    // $deq

    mmg$imgreset();

    // another dassgn round

    // dalloc

    // dacefc

    // vmslock(&SPIN_SCHED,IPL$_SCHED);

    // check error log mailbox queue EXE$AQ_ERLMBX

    // remove ASTs

    // eliminate change mode handlers

    // cancel exit handlers

    // eliminate exception handlers

    // fix AST active/enable bits

    // disable system service failure exceptions

    // cancel compatibility mode handler

    // sch$newlvl

    // clear pcb$v_forcepen wakepen

    // vmsunlock

    // reenable ast delivery

    // delete logicals

    // reset P0 extension

    // resource wait mode to prev state

    return SS$_NORMAL;
}
