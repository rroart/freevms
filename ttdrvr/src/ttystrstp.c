#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#include<linux/blkdev.h>

static unsigned long tty$startio (struct _irp * i, struct _ucb * u)
{
  unsigned long sts=SS$_NORMAL;
  struct request * rq = kmalloc (sizeof(struct request), GFP_KERNEL);

  //printk("ide startio %x %x %x\n",i->irp$l_qio_p1,i->irp$l_qio_p2,i->irp$l_qio_p3);

  //  ide_drive_t *drive=u->ucb$l_orb;

  //i->irp$l_qio_p3 <<= 1;
  i->irp$l_qio_p3 += partadd(u->ucb$l_orb,i->irp$l_qio_p4);
    //drive->part[i->irp$l_qio_p4].start_sect + drive->sect0;

  ideu=u;

  switch (i->irp$v_fcode) {

    case IO$_WRITELBLK:
    case IO$_WRITEVBLK:
    case IO$_WRITEPBLK: {
    }

    case IO$_READLBLK:
    case IO$_READVBLK:
    case IO$_READPBLK: {
    }

    /* Who knows what */

    default: {
      return (SS$_BADPARAM);
    }
  }
}

int tty$wrtstartio(struct _irp * i,struct _ucb * u) {
}
