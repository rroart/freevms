// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/linkage.h>

#include <starlet.h>
#include <exe_routines.h>

asmlinkage int exe$imgsta_wrap(struct struct_args * s)
{
    return exe$imgsta(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6);
}

asmlinkage int exe$imgsta(void * transfer, void * parseinfo, void * header, void * file, unsigned long linkstatus, unsigned long clistatus)
{

}
