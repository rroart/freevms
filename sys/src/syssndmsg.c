// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/kernel.h>

#include<descrip.h>
#include<opcdef.h>

asmlinkage int exe$sndopr(void *msgbuf, unsigned short int chan)
{
    struct dsc$descriptor * d=msgbuf;
    struct _opcdef * msg = d->dsc$a_pointer;

    printk("%s\n",&msg->opc$l_ms_text);
    // reimplement this
}

