#ifndef vms_drivers_h
#define vms_drivers_h

//#include <builtins.h>

#include <fkbdef.h>
#include <irpdef.h>
#include <pdscdef.h>
#include <splcoddef.h>
#include <spldef.h>
#include <ssdef.h>
#include <stsdef.h>
#include <ucbdef.h>
#include <vecdef.h>

#include <exe_routines.h>
#include <ioc_routines.h>
#include <smp_routines.h>

#define fork(fork_routine, fr3, fr4, fkb)			\
          {							\
            ((struct _fkb *) fkb)->fkb$l_fpc = fork_routine;	\
            ((struct _fkb *) fkb)->fkb$l_fr3 = (long) fr3;	\
            ((struct _fkb *) fkb)->fkb$l_fr4 = (long) fr4;	\
            exe_std$queue_fork( (struct _fkb *) fkb );		\
          }

#endif
