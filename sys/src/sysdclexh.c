// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<misc.h>
#include<ssdef.h>
#include<system_data_cells.h>

asmlinkage int exe$dclexh(long handler)
{
    struct _exh * exh = handler;
    exh->exh$l_flink = ctl$gl_thexec;
    ctl$gl_thexec = exh;
    return SS$_NORMAL;
}
