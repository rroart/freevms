// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

int exe$procstrt(struct _pcb * p) {
  // get pcb and copy pqb
  // store rms dispatcher address
  // initialize dispatch vector for system services
  // init msg vect
  // propagate image dump flag
  // init krp
  // set cpu and ast limits
  // init wslist
  // copy b_pri to b_authpri
  // copy priv mask
  // copy msg flags to p1
  // save logintime in ctl$gq_login
  // copy security clearance
  // init getjpi queue
  // init image activator listheads
  // init cwps queue
  // init process_scan queue
  // create p1
  // init logical hash table
  // init lnm$process_directory
  // create lnm$job lnm$group lnm$process and insert them
  // create sys$input etc
  // create job and group tables
  // create space for process-private logicals cache
  // copy from pqb
  // copy more from pqb
  // copy uaf
  // copy jib
  // dealloc pqb
  // mmg$imgreset
  // $imgfix
  // merge f11xqp into p1
  // change access mode
  // request to image activator
  // exe$rmsexh
  // dummy cli
  // change access mode again
  // clear FP
  // initial call frame
  // exe$catch_all
  // $imgfix
  // arg list
  // check hib stsflg
  // call inital image transfer address
}
