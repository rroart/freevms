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

#include<linux/vmalloc.h>
#include<linux/linkage.h>

#include <stdio.h>
//#include <stdlib.h>
//#include <memory.h>
#include <linux/ctype.h>
//#include "ssdef.h"
#include <ssdef.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <ddbdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <ssdef.h>
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
#include <vmstime.h>

#include "cache.h"
#include "access.h"
//#include "phyio.h"

struct phyio_info {
  unsigned status;
  unsigned sectors;
  unsigned sectorsize;
};

/* device_create() creates a device object... */

void *device_create(unsigned devsiz,void *keyval,unsigned *retsts)
{
    char *devnam = (char *) keyval;
    struct _ucb *dev = (struct _ucb *) vmalloc(sizeof(struct _ucb) + devsiz + 2);
    if (dev == NULL) {
        *retsts = SS$_INSFMEM;
    } else {
        unsigned sts;
        struct phyio_info info;
#if 0
        dev->cache.objmanager = NULL;
        dev->cache.objtype = 1;
#endif
#if 0
        memcpy(dev->devnam,devnam,devsiz);
        memcpy(dev->devnam + devsiz,":",2);
#endif
        sts = phyio_init(devsiz + 1,dev->ucb$l_ddb->ddb$t_name,&dev->ucb$l_vcb->vcb$l_aqb->aqb$l_acppid,&info);
        *retsts = sts;
        if (sts & 1) {
            dev->ucb$l_vcb = NULL;
            dev->ucb$b_state = info.status;
            dev->ucb$b_sectors = info.sectors;
	    //            dev->sectorsize = info.sectorsize;
        } else {
            vfree(dev);
            dev = NULL;
        }
    }
    return dev;
}

/* device_compare() compares a device name to that of a device object... */

int device_compare(unsigned keylen,void *keyval,void *node)
{
    struct _ucb *devnode = (struct _ucb *) node;
    int cmp = 0;
    int len = keylen;
    char *keynam = (char *) keyval;
    char *devnam = devnode->ucb$l_ddb->ddb$t_name;
    while (len-- > 0) {
        cmp = toupper(*keynam++) - toupper(*devnam++);
        if (cmp != 0) break;
    }
    return cmp;
}

/* device_lookup() is to to find devices... */

struct _ucb *dev_root = NULL;

unsigned device_lookup_not(unsigned devlen,char *devnam,int create,struct _ucb **retdev)
{
#if 0
    struct _ucb *dev;
    unsigned sts = 1,devsiz = 0;
    while (devsiz < devlen) {
        if (devnam[devsiz] == ':') break;
        devsiz++;
    }
    dev = (struct _ucb *) cache_find((void **) &dev_root,devsiz,devnam,&sts,
                                    device_compare,create ? device_create : NULL);
    if (dev == NULL) {
        if (sts == SS$_ITEMNOTFOUND) sts = SS$_NOSUCHDEV;
    } else {
        *retdev = dev;
        sts = SS$_NORMAL;
    }
    return sts;
#endif
}

extern struct _ucb * myfilelist[50];
extern int myfilelistptr;

unsigned device_lookup(unsigned devlen,char *devnam,int create,struct _ucb **retdev)
{
    struct _ucb *dev;
    int i;
    unsigned sts = 1,devsiz = 0;
    for (i=0;i<myfilelistptr;i++) {
      dev=myfilelist[i];
      if (strlen(dev->ucb$l_ddb->ddb$t_name)==strlen(devnam) && strncmp(dev->ucb$l_ddb->ddb$t_name,devnam,strlen(devnam))==0) goto end;
    }
 end:
    if (dev == NULL) {
        if (sts == SS$_ITEMNOTFOUND) sts = SS$_NOSUCHDEV;
    } else {
        *retdev = dev;
        sts = SS$_NORMAL;
    }
    return sts;
}


