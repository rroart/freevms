#include<linux/linkage.h>
#include<linux/kernel.h>

#include<descrip.h>

asmlinkage int exe$sndopr(void *msgbuf, unsigned short int chan) {
     struct dsc$descriptor * d=msgbuf;
     printk("%s\n",d->dsc$a_pointer);
     // reimplement this
}

