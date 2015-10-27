// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thron�s.

/* Device.c v1.3  Module to remember and find devices...*/

/*
 This is part of ODS2 written by Paul Nankervis,
 email address:  Paulnank@au1.ibm.com

 ODS2 is distributed freely for all members of the
 VMS community to use. However all derived works
 must maintain comments in their source to acknowledge
 the contibution of the original author.
 */

/* Should have mechanism to return actual device name... */

/*  This module is simple enough - it just keeps track of
 device names and initialization... */

#include <linux/vmalloc.h>
#include <linux/linkage.h>

#include <linux/ctype.h>
#include <ssdef.h>

#include <aqbdef.h>
#include <ccbdef.h>
#include <ddbdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <rmsdef.h>
#include <ucbdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <fh2def.h>
#include <hm2def.h>
#include <fcbdef.h>

#include "cache.h"
#if 0
#include "access.h"
#endif
#include <exe_routines.h>
#include <misc_routines.h>

/* device_create() creates a device object... */

void *device_create(unsigned devsiz, void *keyval, unsigned *retsts)
{
    char *devnam = (char *) keyval;
    struct dsc$descriptor dsc;
    struct _ucb *dev = (struct _ucb *) vmalloc(
                           sizeof(struct _ucb) + devsiz + 2);
    int chan;
    dsc.dsc$w_length = strlen(devnam);
    dsc.dsc$a_pointer = devnam;
    if (dev == NULL )
    {
        *retsts = SS$_INSFMEM;
    }
    else
    {
        unsigned sts;
        struct phyio_info info;
        sts = phyio_init(devsiz + 1, &dev->ucb$l_ddb->ddb$t_name[1],
                         &dev->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count, &info, 0); // check
        *retsts = sts;
        if (sts & 1)
        {
            dev->ucb$l_vcb = NULL;
            dev->ucb$b_state = info.status;
            dev->ucb$b_sectors = info.sectors;
            sts = exe$assign(&dsc, &chan, 0, 0, 0);
            dev->ucb$ps_adp = chan; //wrong field and use, but....
            //            dev->sectorsize = info.sectorsize;
        }
        else
        {
            vfree(dev);
            dev = NULL;
        }
    }
    return dev;
}

/* device_compare() compares a device name to that of a device object... */

int device_compare(unsigned keylen, void *keyval, void *node)
{
    struct _ucb *devnode = (struct _ucb *) node;
    int cmp = 0;
    int len = keylen;
    char *keynam = (char *) keyval;
    char *devnam = &devnode->ucb$l_ddb->ddb$t_name[1];
    while (len-- > 0)
    {
        cmp = toupper(*keynam++) - toupper(*devnam++);
        if (cmp != 0)
            break;
    }
    return cmp;
}

/* device_lookup() is to to find devices... */

struct _ucb *dev_root = NULL;

extern struct _ucb * myfilelist[50];
extern char * myfilelists[50];
extern int myfilelistptr;
extern char root_device_name[];
extern struct _ccb ctl$ga_ccb_table[];

int device_lookup(unsigned devlen, char *devnam, int create,
                       short int *retchan)
{
    struct _ucb *dev;
    int i;
    int sts = 1;
    unsigned devsiz = 0;
    char * colon;
    int chan = 0;
    struct dsc$descriptor d;

    if (*devnam == '_')
    {
        devnam++;
    }
    colon = strchr(devnam, ':');
    if (colon)
        devlen = colon - devnam;

    for (i = 0; i < myfilelistptr; i++)
    {
        dev = myfilelist[i];
        if (strlen(myfilelists[i]) == devlen
                && strncmp(myfilelists[i], devnam, devlen) == 0)
            goto end;
    }

    // real disk
    d.dsc$w_length = devlen;
    d.dsc$a_pointer = do_translate(devnam);
    int sys$assign(void *devnam, unsigned short int *chan, unsigned int acmode,
                   void *mbxnam, int flags);
    if (ctl$gl_pcb->psl_cur_mod)
        sts = sys$assign(&d, &chan, 0, 0, 0);
    else
        sts = exe$assign(&d, &chan, 0, 0, 0);
    dev = ctl$ga_ccb_table[chan].ccb$l_ucb;

end:
    if (dev == NULL )
    {
        if (sts == SS$_ITEMNOTFOUND)
            sts = SS$_NOSUCHDEV;
    }
    else
    {
        *retchan = chan;
        sts = SS$_NORMAL;
    }

    return sts;
}
