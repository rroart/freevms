#if 0
//+++2002-08-17
//    Copyright (C) 2001,2002  Mike Rieker, Beverly, MA USA
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---2002-08-17

/************************************************************************/
/*									*/
/*  82078 floppy disk controller driver					*/
/*									*/
/*  Creates devices named floppy.cn					*/
/*									*/
/*	c = controller: p for primary, s for secondary			*/
/*	n = unitnumber: 0 or 1						*/
/*									*/
/************************************************************************/

#include<crbdef.h>
#include<cdtdef.h>
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
#include<asm/floppy.h>

#if 0
#include "ozone.h"

#include "oz_dev_disk.h"
#include "oz_dev_timer.h"
#include "oz_io_disk.h"
#include "oz_knl_devio.h"
#include "oz_knl_event.h"
#include "oz_knl_hw.h"
#include "oz_knl_kmalloc.h"
#include "oz_knl_procmode.h"
#include "oz_knl_sdata.h"
#include "oz_knl_section.h"
#include "oz_knl_spte.h"
#include "oz_knl_status.h"
#include "oz_knl_thread.h"
#include "oz_knl_userjob.h"
#include "oz_sys_dateconv.h"
#include "oz_sys_xprintf.h"
#endif

//typedef struct  { int a; } DEv;
//typedef struct  { int a; } ;
typedef struct OZ_Event { int a; } OZ_Event;
typedef struct OZ_Timer { int a; } OZ_Timer;
typedef struct OZ_Devunit { int a; } OZ_Devunit;
typedef struct OZ_Iochan { int a; } OZ_Iochan;
typedef struct OZ_Ioop { int a; } OZ_Ioop;
typedef struct OZ_Procmode { int a; } OZ_Procmode;
typedef struct OZ_Devfunc { int a; } OZ_Devfunc;
#if 0
typedef struct OZ_ { int a; } OZ_;
typedef struct OZ_ { int a; } OZ_;
typedef struct OZ_ { int a; } OZ_;
#endif

#define NAME "floppy"

#define L2DISK_BLOCK_SIZE (devex -> sizecode + 7)
#define DISK_BLOCK_SIZE (128 << devex -> sizecode)
#define MAXSIZECODE (sizeof sectorspertrack / sizeof sectorspertrack[0])
#define TRACKSPERCYLINDER (2)
#define CYLINDERS (80)
#define GPL_1 (0x1B)
#define GPL_2 (0x54)

#define IRQ_LEVEL 6		// all controllers share this IRQ
#define INT_TIMEOUT 5		// number of seconds to wait for an interrupt
#define DRIVE_RDY_TIME 3	// within 3Sec after recalibrate, drive should be ready
#define MSR_RQM_TIMEOUT 50000	// within 50mS after interrupt, RQM should be set
#define MOTOR_TIMER_SECONDS 2	// let motor run for 2Sec after last using it

#define MAX_READ_RETRIES 8	// retry reads 8 times

#if 0
#define LOCKDMA() while (oz_knl_event_set (dmapage_event, 0) == 0) oz_knl_event_waitone (dmapage_event)
#define UNLKDMA() oz_knl_event_set (dmapage_event, 1)
#endif

#if 0
#define LOCKDMA() do { volatile int flag=1; while (flag) { exe$readef(66,&flag);  } } while (0)
#define UNLKDMA() exe$setef(66)
#endif

#define LOCKDMA()
#define UNLKDMA()

typedef struct Controller Controller;
typedef struct Devex Devex;
typedef struct Iopex Iopex;

/* Registers */

#define SRB (1)		/* R/W - Status Register B */
#define SRB_IDLE (0x01)
#define SRB_PD (0x02)
#define SRB_IDLEMSK (0x04)
#define DOR (2)		/* R/W - Digital Output Register */
#define DOR_DRIVESEL (0x01)
#define DOR_RESET_ (0x04)
#define DOR_DMAGATE (0x08)
#define DOR_MOTEN0 (0x10)
#define DOR_MOTEN1 (0x20)
#define DOR_DRIVE0 (0x1c)	/* Composite value to enable drive 0 */
#define DOR_DRIVE1 (0x2d)	/* Composite value to enable drive 1 */
#define TDR (3)		/* R/W - Tape Drive Register */
#define MSR (4)		/* R-O - Main Status Register */
#define MSR_DRV0BUSY (0x01)	/* Drive 0 is busy doing a seek */
#define MSR_DRV1BUSY (0x02)	/* Drive 1 is busy doing a seek */
#define MSR_CB (0x10)		/* 0: No command in progress; 1: Command in progress */
#define MSR_NONDMA (0x20)	/* 0: Result data can be read; 1: Disk data can be read */
#define MSR_DIO (0x40)		/* 0: Host can write data; 1: Host can read data */
#define MSR_RQM (0x80)		/* 0: Host cannot transfer data; 1: Host can transfer data */
#define DSR (4)		/* W-O - Data Rate Select Register */
#define FIFO (5)	/* R/W - Data Register */
#define DIR (7)		/* R-O - Digital Input Register */
#define DIR_DSKCHG_ (0x80)	/* 0: Disk has not been changed; 1: Disk has been changed */
#define CCR (7)		/* W-O - Configuration Control Register */

#define ST3_WP (0x40)	/* R-O - write protected */
#define ST3_TRK0 (0x10)	/* R-O - track zero */

/* DMA registers (channel 2) */

#define DMA_ADDR2 (0x04)		/* W-O - start address */
					/* R-O - current address */
#define DMA_COUNT2 (0x05)		/* W-O - start count */
					/* R-O - remaining count */
#define DMA_CTRL (0x08)			/* W-O - control */
					/*       <0> = 0 : mem-to-mem disabled */
					/*             1 : mem-to-mem enabled */
					/*       <1> = 0 : ch 0 addr hold disabled */
					/*             1 : ch 0 addr hold enabled */
					/*       <2> = 0 : controller enabled */
					/*             1 : controller disabled */
					/*       <3> = 0 : normal timing */
					/*             1 : compressed timing */
					/*       <4> = 0 : fixed priority */
					/*             1 : rotating priority */
					/*       <5> = 0 : late write selection */
					/*             1 : extended write selection */
					/*       <6> = 0 : DRQn sense asserted HI */
					/*             1 : DRQn sense asserted LO */
					/*       <7> = 0 : DAKn sense asserted LO */
					/*             1 : DAKn sense asserted HI */
#define DMA_MODE (0x0B)			/* W-0 - dma mode */
					/*       <0:1> = channel select */
					/*       <2:3> = 00 : verify */
					/*               01 : device-to-memory */
					/*               10 : memory-to-device */
					/*               11 : reserved */
					/*       <4> = 0 : no auto-init */
					/*             1 : auto-init */
					/*       <5> = 0 : address increment */
					/*             1 : address decrement */
					/*       <6:7> = 00 : demand transfer mode */
					/*               01 : single transfer mode */
					/*               10 : block transfer mode */
					/*               11 : cascade transfer mode */
#define DMA_CBP (0x0C)			/* writing to this causes the DMAC to reset the hi/lo byte sequencing */
#define DMA_MCLR (0x0D)			/* writing to this does a hardware reset of the DMAC */
#define DMA_ACT (0x0E)			/* writing to this activates all four DMA channels */
#define DMA_MASK (0x0F)			/* mask bit for each four channels (when set, inhibits operation of the channel) */
#define DMA_PAGE2 (0x81)		/* R/W - address page (64K page) */

#define DMA_DEV2MEM (0x04)
#define DMA_MEM2DEV (0x08)

/* One of these per controller */

#if 0
struct Controller { Controller *next;		/* next controller in list */
                    OZ_Event *event;		/* associated event */
                    OZ_Timer *inttimer;		/* interrupt timer */
                    const char *ctrlname;	/* controller name ("primary", "secondary") */
                    Devex *devexs;		/* list of devices on this controller */
                    Iopex *iopexqh;		/* list of I/O requests waiting for thread */
                    Iopex **iopexqt;
                    Iopex *iopexip;		/* the I/O request currently in progress */
                    volatile int waitingforint;	/* 0=not (no longer) waiting for interrupt; 1=waiting for interrupt; -1=timedout */
                    unsigned short ioportbase;		/* io port base address, 0x03F0 or 0x0370 */
                  };

/* Device extension structure */

struct Devex { Devex *next;			/* next device on controller */
               Devex *prev;
               OZ_Timer *motor_timer;		/* motor timer */
               OZ_Datebin motoroff;		/* when the motor is to be turned off (or 0 if it is off) */
               Controller *controller;		/* the controller it is connected to */
               struct _ucb *devunit;		/* device unit struct pointer */
               const char *unitname;		/* unit name string (for error messages) */
               OZ_Dbn startblock;		/* starting block of the track in trackbuf */
               unsigned long validblocks;		/* number of blocks in trackbuf that are valid */
               unsigned long dirtybeg;			/* offset of first block in trackbuf that is dirty */
               unsigned long dirtyend;			/* offset past last block in trackbuf that is dirty */
               unsigned char *trackbuffer;		/* virtual address of track buffer */
               unsigned long trackbuf_pageoffs;		/* offset of first byte of track buffer in first physical page */
               OZ_Mempage trackbuf_phypages[8];	/* array of track buffer physical page numbers */
               unsigned char sizecode;			/* 0=128, 1=256, 2=512, ... 7=16k */
               unsigned char drivesel;			/* drive select: 0=drive 0; 1=drive 1 */
               unsigned char datarate;			/* datarate for the media currently in drive */
               char failed;			/*  0: initialization in progress */
						/*  1: failed to initialize, delete in progress */
						/* -1: init completed successfully, drive is online */
               char volvalid;			/* volume is valid */
               char writable;			/* volume is writable */
             };

/* I/O operation extension */

struct Iopex { Iopex *next;			/* next request in queue */
               struct _irp *ioop;			/* corresponding ioop */
               unsigned long (*be) (Iopex *iopex);	/* backend routine */
               Devex *devex;			/* device that it is for */
               int aborted;			/* tagged for abort by floppy_abort routine */
               int writing;			/* 0=reading; 1=writing */
               int writethru;			/* 0=normal; 1=writethru */
               unsigned long size;			/* size of buffer */
               const OZ_Mempage *phypages;	/* points to array of physical page numbers */
               unsigned long byteoffs;			/* byte offset in first physical page */
               OZ_Dbn slbn;			/* starting logical block number */
             };
#endif

/* Function table */

static int floppy_shutdown (struct _ucb *devunit, void *devexv);
static int floppy_clonedel (struct _ucb *devunit, void *devexv, int cloned);
static void floppy_abort (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, 
                          struct _irp *ioop, void *iopexv, unsigned long procmode);
static unsigned long floppy_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, unsigned long procmode, 
                           struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap);

#if 0
static const OZ_Devfunc floppy_functable = { sizeof (Devex), 0, sizeof (Iopex), 0, floppy_shutdown, NULL, 
                                             floppy_clonedel, NULL, NULL, floppy_abort, floppy_start, NULL };
#endif 

static struct _fdt fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE
|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD
|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};


static const struct _ddt floppy_functable = {
  ddt$l_start: floppy_start,
  ddt$l_cancel: floppy_abort,

};

/* Internal static data */

                                        // 128,  256,  512, 1024, 2048, 4096, 8192	corresponding bytes per sector
static const unsigned char sectorspertrack[8] = {   34,   26,   18,   11,    6,    3,    1 };	// indexed using devex->sizecode

#define MAX_TRACK_SIZE (2048*6)			// what blocksize*sectorspertrack size gives the max data bytes on a track

static struct _idb *controllers;
static int initialized = 0;
//static OZ_Devclass *devclass;
//static OZ_Devdriver *devdriver;
//static OZ_Event *dmapage_event;			/* dma page's access event flag */
//static OZ_Smplock *smplock_fd;			/* floppy drive's irq smplock */
static unsigned char dmapage_buff[MAX_TRACK_SIZE*2];	/* dma buffer, phyaddr = virtaddr */
static unsigned char *dmapage_addr;			/* dma page's virtual address = dma page's physical address */
static unsigned long dmapage_size;			/* number of bytes available in dmapage_phypage */
//static struct OZ_Hw486_irq_many fd_irq_many;		/* interrupt handler link block */

/* Internal routines */

static void config (unsigned short ioportbase, const char *ctrlname);
static unsigned long floppy_thread (void *controllerv);
static void setdrivefailed (Devex *devex);
static void setdrivestatus (Devex *devex, const char *status);
static unsigned long be_setvolvalid (Iopex *iopex);
static unsigned long be_writeblocks (Iopex *iopex);
static unsigned long be_readblocks (Iopex *iopex);
static unsigned long be_format (Iopex *iopex);
static int mediaonline (Iopex *iopex);
static unsigned long flushtrackbuffer (Devex *devex);
static unsigned long filltrackbuffer (Devex *devex);
static int resetctrlr (struct _idb *controller, unsigned char datarate);
static unsigned long resetctrlr_check (void *ioportbasev);
static int recalibrate (Devex *devex);
static void driveidlewait (Devex *devex);
static unsigned long write_blocks (unsigned long size, const unsigned char *buff, unsigned long slbn, unsigned long *wlen, Devex *devex);
static unsigned long read_blocks (unsigned long size, unsigned char *buff, unsigned long slbn, unsigned long *rlen, Devex *devex);
static unsigned long chkvolvalid (Devex *devex);
static void motor_timer_start (Devex *devex);
static void motor_timer_expired (void *devexv, OZ_Timer *timer);
static int send_command (int size, unsigned char *buff, struct _idb *controller, int interrupts);
static void int_timeout (void *controllerv, OZ_Timer *timer);
static unsigned long recv_results (int size, unsigned char *buff, unsigned short port, Devex *devex, unsigned char command);
static int command_done (unsigned short port, unsigned char command);
static unsigned long command_done_check (void *portv);
static unsigned char wait_for_rqm (unsigned short port);
static unsigned long check_for_rqm (void *rqmprmv);
static unsigned char wait_for_rqmdio (unsigned short port);
static unsigned long check_for_rqmdio (void *rqmprmv);
static void print_rw_err (const char *rw, Devex *devex, unsigned char *command, unsigned char *results, unsigned long len);
static unsigned long dmapage_setup (int dmadir, unsigned long transferlen, Devex *devex, unsigned long byteoffs, OZ_Mempage *phypages, OZ_Dbn slbn);
static int fd_interrupt (void *dummy, OZ_Mchargs *mchargs);


static struct _dpt dv_dpt;
static struct _dpt dv_ddt;

/************************************************************************/
/*									*/
/*  Boot-time initialization routine					*/
/*									*/
/************************************************************************/

void oz_dev_floppy_init ()

{
  unsigned long pm, sts;

  if (initialized) return;

  printk ("oz_dev_floppy_init\n");
  initialized = 1;

  /* Make sure the dmapage_buff meets the restrictions of the DMA controller.  The stupid DMA  */
  /* controller can't cross 64K boundaries and it can't address anything above 0xFFFFFF (16M). */

  dmapage_size = sizeof dmapage_buff;
  dmapage_addr = dmapage_buff;
  if (((unsigned long)dmapage_addr & -65536) != ((((unsigned long)dmapage_addr) + dmapage_size - 1) & -65536)) {
    dmapage_size /= 2;
    if (((unsigned long)dmapage_addr & -65536) != ((((unsigned long)dmapage_addr) + dmapage_size - 1) & -65536)) {
      dmapage_addr  = (unsigned char *)((((unsigned long)dmapage_addr) + 65535) & -65536);
      if (((unsigned long)dmapage_addr & -65536) != ((((unsigned long)dmapage_addr) + dmapage_size - 1) & -65536)) {
        panic ("oz_dev_floppy_init: dmapage_addr %p size %x still crosses 64K boundary", dmapage_addr, dmapage_size);
      }
    }
    printk ("oz_dev_floppy_init: dmapage_buff size %x at %p crosses 64K boundary\n", sizeof dmapage_buff, dmapage_buff);
    printk ("oz_dev_floppy_init: - using size %x at %p instead\n", dmapage_size, dmapage_addr);
  }

  if (((unsigned long)dmapage_addr + dmapage_size) > 16*1024*1024) {
    printk ("oz_dev_floppy: dmapage_buff %p+0x%x is beyond 16Meg limit\n", dmapage_addr, dmapage_size);
    return;
  }

  /* Allocate an event flag for the (potentially) two threads to serialize access to the one-and-only DMA controller */

  $DESCRIPTOR(NAME1,"floppy dma");

  exe$ascefc(66,&NAME1);

#if 0
  sts = oz_knl_event_create (10, "floppy dma", NULL, &dmapage_event);
  if (sts != SS$_NORMAL) panic ("oz_dev_floppy: error %u creating dmpage_event", sts);
#endif
  UNLKDMA ();

  /* Create device driver database entries */

  //devclass   = oz_knl_devclass_create (OZ_IO_DISK_CLASSNAME, OZ_IO_DISK_BASE, OZ_IO_DISK_MASK, "floppy_486");
  //devdriver  = oz_knl_devdriver_create (devclass, "floppy_486");
  //fd_irq_many.entry = floppy_interrupt;
  //fd_irq_many.param = NULL;
  //fd_irq_many.descr = NAME;
  //smplock_fd = oz_hw486_irq_many_add (IRQ_LEVEL, &fd_irq_many);
  fd_request_irq();

  /* Configure controllers */

  controllers = NULL;
  config (0x03F0, "primary");
  //config (0x0370, "secondary");
}

/* This routine sets up the devunit structs immediately then forks a thread to perform the actual probing and     */
/* initialization.  Meanwhile, the main thread is free to continue on.   If anything tries to queue I/O to the    */
/* floppy devices before they have been configured, the requests will wait until the configuration is complete.   */
/* If the configuration finds that the controller and/or drive does not exist, it will abort any queued requests. */

static void config (unsigned short ioportbase, const char *ctrlname)

{
  char threadname[OZ_THREAD_NAMESIZE], unitname[OZ_DEVUNIT_NAMESIZE];
  struct _idb *controller;
  Devex *devex, **ldevex;
  int drivesel;
  struct _ucb *devunit;
  OZ_Thread *thread;
  unsigned long sts;

  struct _ucb * u=vmalloc(sizeof(struct _ucb));
  bzero(u,sizeof(struct _ucb));
  struct _ddb * d=vmalloc(sizeof(struct _ddb));
  bzero(d,sizeof(struct _ddb));
  struct _crb * c=vmalloc(sizeof(struct _crb));
  struct _idb * idb=vmalloc(sizeof(struct _idb));
  bzero(c,sizeof(struct _crb));
  bzero(u,sizeof(struct _ucb));
  bzero(d,sizeof(struct _ddb));
  bzero(c,sizeof(struct _crb));
  bzero(idb,sizeof(struct _idb));

  insertdevlist(d);

  bcopy("DVA0",d->ddb$t_name,4)

  bcopy("DVDRIVER",dv_dpt.dpt$t_name,8);

  /* Create and initialize controller struct */

  controller = kmalloc (sizeof *controller, GFP_KERNEL);
  memset (controller, 0, sizeof *controller);
  controller -> idb$q_csr = ioportbase;
  controller -> ctrlname   = ctrlname;
  //controller -> iopexqt    = &(controller -> iopexqh);
  qheadinit(&u->ucb$l_ioqfl);
  controller -> inttimer   = oz_knl_timer_alloc ();

  /* Create the devices now in case someone wants to queue requests before we sense if the drive is there or not. */
  /* This will happen if we are booting from floppy and the floppy will be mounted right away.  In that case, the */
  /* mount will wait for the initialization to complete.  I hate waiting for boot routines to grind on floppies.  */

  ldevex = &(controller -> devexs);
  for (drivesel = 0; drivesel < 2; drivesel ++) {

    /* Create the device unit struct */

    sprintf (sizeof unitname, unitname, NAME ".%c%u", ctrlname[0], drivesel);
    devunit = oz_knl_devunit_create (devdriver, unitname, "", &floppy_functable, 0, oz_s_secattr_sysdev);
    if (devunit == NULL) continue;

    /* Fill in the device unit extension info */

    devex = oz_knl_devunit_ex (devunit);
    devex -> failed      = 0;
    devex -> devunit     = devunit;
    devex -> motor_timer = oz_knl_timer_alloc ();
    devex -> motoroff    = 0;
    devex -> ucb$ps_pb_idb  = controller;
    devex -> sizecode    = 0;
    devex -> drivesel    = drivesel;
    devex -> volvalid    = 0;
    devex -> unitname    = oz_knl_devunit_devname (devunit);

    /* Link it to list of devices on the controller */

    *ldevex = devex;
    ldevex = &(devex -> next);

    /* Set its status */

    setdrivestatus (devex, "probing");

    /* Set it up to automount a filesystem (theoretically, it can do partitions, too) */

    oz_knl_devunit_autogen (devunit, oz_dev_disk_auto, NULL);
  }
  *ldevex = NULL;

  /* Start the kernel thread to process the initialization and then the I/O requests */

  sprintf (sizeof threadname, threadname, NAME " %s", ctrlname);
  sts = oz_knl_event_create (sizeof threadname, threadname, NULL, &(controller -> event));
  if (sts != SS$_NORMAL) printk ("oz_dev_floppy config: error %u creating event %s\n", sts, ctrlname);
  else {
    sts = oz_knl_thread_create (oz_s_systemproc, oz_knl_user_getmaxbasepri (NULL), NULL, NULL, NULL, 0, 
                                floppy_thread, controller, OZ_ASTMODE_INHIBIT, sizeof threadname, threadname, 
                                NULL, &thread);
    if (sts != SS$_NORMAL) printk ("oz_dev_floppy config: error %u creating thread %s\n", sts, ctrlname);
    else {
      oz_knl_thread_orphan (thread);
      oz_knl_thread_increfc (thread, -1);
    }
  }
}

/************************************************************************/
/*									*/
/*  This routine is called when the system is being shut down.  It can 	*/
/*  be assumed that there is no floppy I/O going as disks have been 	*/
/*  dismounted at this point.						*/
/*									*/
/************************************************************************/

static int floppy_shutdown (struct _ucb *devunit, void *devexv)

{
  /* Turn both motors off and reset controller */

  oz_hw_outb (0, ((Devex *)devexv) -> ucb$ps_pb_idb -> idb$q_csr + DOR);
  return (1);
}

/************************************************************************/
/*									*/
/*  The last reference was removed from the device and now the system 	*/
/*  wants to know if it should be deleted				*/
/*									*/
/*    Input:								*/
/*									*/
/*	devunit/devexv = device to be deleted				*/
/*	cloned = as passed to oz_knl_devunit_create			*/
/*	smplevel = dv							*/
/*									*/
/*    Output:								*/
/*									*/
/*	floppy_clonedel = 1 : ok to delete the device			*/
/*									*/
/*    Note:								*/
/*									*/
/*	This routine is used to delete a device that failed to 		*/
/*	initialize at boot time.  It should only get called with 	*/
/*	'failed=1' after the setdevicefailed routine decrements the 	*/
/*	reference count.						*/
/*									*/
/*	It should never be called for good devices, as the driver 	*/
/*	keeps its original reference count out on the devunit.		*/
/*									*/
/************************************************************************/

static int floppy_clonedel (struct _ucb *devunit, void *devexv, int cloned)

{
  struct _idb *controller;
  Devex *devex, **ldevex, *xdevex;

  devex = devexv;
  if (devex -> failed <= 0) panic ("oz_dev_floppy_486 clonedel: devex %p -> failed %d", devex, devex -> failed);
  controller = devex -> ucb$ps_pb_idb;		// unlink it from controller
  for (ldevex = &(controller -> devexs); (xdevex = *ldevex) != devex; ldevex = &(xdevex -> next)) {}
  *ldevex = xdevex -> next;
  return (1);					// it's ok to delete it
}

/************************************************************************/
/*									*/
/*  Abort I/O request in progress					*/
/*									*/
/************************************************************************/

static unsigned long floppy_abort (struct _irp * i, struct _ucb * u)

#if 0
static void floppy_abort (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, 
                          struct _irp *ioop, void *iopexv, unsigned long procmode)
#endif

{
  struct _idb *controller;
  Devex *devex;
  Iopex *iopex, **liopex, *xiopex;
  unsigned long fd;

  devex  = devexv;
  xiopex = NULL;								// haven't found anything to abort yet
  controller = devex -> ucb$ps_pb_idb;						// point to controller struct

  /* Remove any matching requests from controller queue */

  //fd  = oz_hw_smplock_wait (smplock_fd);					// lock its queue
  for (liopex = &(controller -> iopexqh); (iopex = *liopex) != NULL;) {		// scan the queue
    if (oz_knl_ioabortok (iopex -> ioop, iochan, procmode, ioop)) {		// see if this is something we should abort
      *liopex = iopex -> next;							// if so, unlink from queue
      iopex -> next = xiopex;							// link to stuff to abort
      xiopex = iopex;
    } else {
      liopex = &(iopex -> next);						// leave it alone
    }
  }
  controller -> iopexqt = liopex;						// set up possibly new tail pointer

  iopex = controller -> iopexip;						// maybe abort the request in progress
  if ((iopex != NULL) && oz_knl_ioabortok (iopex -> ioop, iochan, procmode, ioop)) iopex -> aborted = 1;
  // oz_hw_smplock_clr (smplock_fd, fd);						// unlock the queue

  /* Now that we're back at softint level, abort all the requests we found */

  while ((iopex = xiopex) != NULL) {
    xiopex = iopex -> next;
    oz_knl_iodone (iopex -> ioop, OZ_ABORTED, NULL, NULL, NULL);
  }
}

/************************************************************************/
/*									*/
/*  Start performing a disk i/o function				*/
/*									*/
/************************************************************************/

#if 0
static unsigned long floppy_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, unsigned long procmode, 
				   struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap)
#endif

     static unsigned long floppy_start (struct _irp * i, struct _ucb * u)
{
  Devex *devex;
  Iopex *iopex;
  unsigned long fd, sts;

  devex = devexv;
  iopex = iopexv;
  iopex -> aborted = 0;

  /* Process individual functions */

  switch (i->irp$v_fcode) {

    /* Set volume valid bit one way or the other */

    case IO$_NOP: /*OZ_IO_DISK_SETVOLVALID*/ {
      OZ_IO_disk_setvolvalid disk_setvolvalid;

      movc4 (as, ap, sizeof disk_setvolvalid, &disk_setvolvalid);
      iopex -> writing = disk_setvolvalid.valid;
      iopex -> be = be_setvolvalid;
      break;
    }

    /* Write blocks to the disk */

    case IO$_WRITEPBLK: {
      OZ_IO_disk_writeblocks disk_writeblocks;

      movc4 (as, ap, sizeof disk_writeblocks, &disk_writeblocks);
      sts = oz_knl_ioop_lockr (ioop, disk_writeblocks.size, disk_writeblocks.buff, &(iopex -> phypages), NULL, &(iopex -> byteoffs));
      if (sts != SS$_NORMAL) return (sts);

      iopex -> size      = disk_writeblocks.size;
      iopex -> slbn      = disk_writeblocks.slbn;
      iopex -> writethru = disk_writeblocks.writethru;
      iopex -> be        = be_writeblocks;
      break;
    }

    /* Read blocks from the disk */

    case IO$_READPBLK: {
      OZ_IO_disk_readblocks disk_readblocks;

      movc4 (as, ap, sizeof disk_readblocks, &disk_readblocks);
      sts = oz_knl_ioop_lockw (ioop, disk_readblocks.size, disk_readblocks.buff, &(iopex -> phypages), NULL, &(iopex -> byteoffs));
      if (sts != SS$_NORMAL) return (sts);

      iopex -> size = disk_readblocks.size;
      iopex -> slbn = disk_readblocks.slbn;
      iopex -> be   = be_readblocks;
      break;
    }

#if 0
    /* Write pages to the disk */

    case OZ_IO_DISK_WRITEPAGES: {
      OZ_IO_disk_writepages disk_writepages;

      if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);

      movc4 (as, ap, sizeof disk_writepages, &disk_writepages);

      iopex -> phypages  = disk_writepages.pages;
      iopex -> byteoffs  = disk_writepages.offset;
      iopex -> size      = disk_writepages.size;
      iopex -> slbn      = disk_writepages.slbn;
      iopex -> writethru = disk_writepages.writethru;
      iopex -> be        = be_writeblocks;
      break;
    }

    /* Read pages from the disk */

    case OZ_IO_DISK_READPAGES: {
      OZ_IO_disk_readpages disk_readpages;

      if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);

      movc4 (as, ap, sizeof disk_readpages, &disk_readpages);

      iopex -> phypages = disk_readpages.pages;
      iopex -> byteoffs = disk_readpages.offset;
      iopex -> size = disk_readpages.size;
      iopex -> slbn = disk_readpages.slbn;
      iopex -> be   = be_readblocks;
      break;
    }
#endif

    /* Get info part 1 */

  case IO$_SENSECHAR: /* OZ_IO_DISK_GETINFO1 */ {
      OZ_IO_disk_getinfo1 disk_getinfo1;

      if (!OZ_HW_WRITABLE (as, ap, procmode)) return (OZ_ACCVIO);
      memset (&disk_getinfo1, 0, sizeof disk_getinfo1);				/* zero fill to clear params we don't care and/or know about */
      if (devex -> volvalid) {
        disk_getinfo1.blocksize   = DISK_BLOCK_SIZE;				/* return what we know about */
        disk_getinfo1.secpertrk   = sectorspertrack[devex->sizecode];
        disk_getinfo1.trkpercyl   = TRACKSPERCYLINDER;
        disk_getinfo1.cylinders   = CYLINDERS;
        disk_getinfo1.totalblocks = disk_getinfo1.secpertrk * disk_getinfo1.trkpercyl * disk_getinfo1.cylinders;
      }
      if (as <= sizeof disk_getinfo1) {
        memcpy (ap, &(disk_getinfo1), as);
      } else {
        memcpy (ap, &disk_getinfo1, sizeof disk_getinfo1);
        memset (((unsigned char *)ap) + sizeof disk_getinfo1, 0, as - sizeof disk_getinfo1);
      }
      return (SS$_NORMAL);
    }

    /* Format -- param string 'sectorsize[,gaplength]' */

    case IO$_FORMAT: {
      char paramstr[16];
      int usedup, usedup2;
      OZ_IO_disk_format disk_format;
      unsigned long blocksize, gaplength, sizecode;

      movc4 (as, ap, sizeof disk_format, &disk_format);
      sts = oz_knl_section_ugetz (procmode, sizeof paramstr, disk_format.paramstr, paramstr, NULL);
      if (sts != SS$_NORMAL) return (sts);				// param string is not addressible
      blocksize = oz_hw_atoi (paramstr, &usedup);			// get sector size from param string
      for (sizecode = 0; sizecode < MAXSIZECODE; sizecode ++) {		// try to find it in the table
        devex -> sizecode = sizecode;
        if (DISK_BLOCK_SIZE == blocksize) break;
      }
      if (sizecode == MAXSIZECODE) return (OZ_BADPARAM);		// if not found, return error status
      gaplength = GPL_2;						// set up default gap length
      if (paramstr[usedup] == ',') {					// see if gap length specified
        gaplength = oz_hw_atoi (paramstr + (++ usedup), &usedup2);	// ok, decode it
        if (gaplength > 0xFF) return (OZ_BADPARAM);			// make sure it is not too big
        usedup += usedup2;						// increment to where terminator is
      }
      if (paramstr[usedup] != 0) return (OZ_BADPARAM);			// check for no junk left in param string
      iopex -> size     = sizecode;					// save the size code here
      iopex -> writing  = 1;						// we are writing to floppy
      iopex -> phypages = NULL;						// we will use the dmapage_buff
      iopex -> byteoffs = gaplength;					// save the gap length here
      iopex -> be = be_format;						// queue the formatting routine
      break;
    }

    /* Who knows what */

    default: {
      return (OZ_BADIOFUNC);
    }
  }

  /* Queue request to kernel thread and wake it if this is the first one on queue */

  iopex -> ioop  = ioop;
  iopex -> devex = devex;
  iopex -> next  = NULL;								// this shall be last in the queue
  controller = devex -> ucb$ps_pb_idb;							// point to controller struct
  sts = OZ_DEVOFFLINE;									// assume the device has failed to init
  // fd  = oz_hw_smplock_wait (smplock_fd);						// lock its queue
  if (devex -> failed <= 0) {
    *(controller -> iopexqt) = iopex;							// link new request on end
    controller -> iopexqt = &(iopex -> next);
    sts = OZ_STARTED;									// it has been successfully queued
  }
  // oz_hw_smplock_clr (smplock_fd, fd);							// unlock the queue
  if (controller -> iopexqh == iopex) {							// wake if this is first in queue
    oz_knl_event_set (controller -> event, 1);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  This kernel thread processes requests for the floppy.  There is 	*/
/*  one kernel thread per controller.					*/
/*									*/
/************************************************************************/

static unsigned long floppy_thread (void *controllerv)

{
  char c, *q, unitdesc[OZ_DEVUNIT_DESCSIZE], unitname[OZ_DEVUNIT_NAMESIZE];
  struct _idb *controller;
  Devex *devex, **dp;
  int foundone, si;
  Iopex *iopex;
  OZ_Datebin now;
  struct _ucb *devunit;
  OZ_Event *event;
  unsigned char dor, drivesel;
  unsigned long dv, fd, sts;
  unsigned short ioportbase;

  controller = controllerv;
  ioportbase = controller -> idb$q_csr;

  si = oz_hw_cpu_setsoftint (0);

  /* Link it so interrupt routine can see it */

  // fd = oz_hw_smplock_wait (smplock_fd);
  controller -> next = controllers;
  controllers = controller;
  // oz_hw_smplock_clr (smplock_fd, fd);

  /* See if the controller is even there.  If not, delete the devices and exit. */

  printk ("oz_dev_floppy: resetting %s floppy controller (port %X)\n", controller -> ctrlname, ioportbase);
  if (!resetctrlr (controller, 0)) {
    printk ("oz_dev_floppy: %s controller didn't come ready\n", controller -> ctrlname);
    while (1) {
      dv = oz_hw_smplock_wait (&oz_s_smplock_dv);
      for (devex = controller -> devexs; devex != NULL; devex = devex -> next) if (devex -> failed <= 0) break;
      oz_hw_smplock_clr (&oz_s_smplock_dv, dv);
      if (devex == NULL) break;
      setdrivefailed (devex);
    }
    oz_hw_cpu_setsoftint (si);
    return (SS$_NORMAL);
  }

  /* Loop through each possible device to see if it is there by doing a recalibrate */

  foundone = 0;
  while (1) {
    dv = oz_hw_smplock_wait (&oz_s_smplock_dv);					// find one we haven't processed yet
    for (devex = controller -> devexs; devex != NULL; devex = devex -> next) if (devex -> failed == 0) break;
    oz_hw_smplock_clr (&oz_s_smplock_dv, dv);
    if (devex == NULL) break;							// we've done them all, exit loop
    printk ("oz_dev_floppy: recalibrating %s\n", devex -> unitname);	// announce we are going to check it
    if (!recalibrate (devex)) setdrivefailed (devex);				// check it, if failed, delete drive
    else {
      devex -> failed = -1;							// it's ok, mark it online
      setdrivestatus (devex, "online");
      printk ("oz_dev_floppy: %s available\n", devex -> unitname);	//   print a message
      foundone = 1;								//   don't exit the kernel thread
    }
    oz_hw_outb (DOR_RESET_, ioportbase + DOR);					// anyway, turn the motors all off
  }

  /* Init completed, good or bad      */
  /* If there aren't any drives, exit */

  if (!foundone) {
    printk ("oz_dev_floppy: no drives attached to %s controller\n", controller -> ctrlname);
    oz_hw_cpu_setsoftint (si);
    return (SS$_NORMAL);
  }

  /* Process I/O requests for those drives forever */

  while (1) {

    /* See if there are any requests to process */

    // fd = oz_hw_smplock_wait (smplock_fd);
    iopex = controller -> iopexqh;
    if (iopex != NULL) {
      controller -> iopexqh = iopex -> next;
      if (iopex -> next == NULL) controller -> iopexqt = &(controller -> iopexqh);
      controller -> iopexip = iopex;
    }
    // oz_hw_smplock_clr (smplock_fd, fd);

    /* If not, wait for something to do */

    if (iopex == NULL) {

      /* Meanwhile, maybe some motors need turning off                */
      /* Do this here instead of in timer ast so it gets synchronized */

      now = oz_hw_tod_getnow ();
      for (devex = controller -> devexs; devex != NULL; devex = devex -> next) {
        if ((devex -> motoroff != 0) && (now >= devex -> motoroff)) {
          if (devex -> volvalid) flushtrackbuffer (devex);	// make sure any pending writes are flushed first
          dor  = oz_hw_inb (ioportbase + DOR);			// now turn the motor off
          dor &= ~ (DOR_MOTEN0 << devex -> drivesel);
          oz_hw_outb (dor, ioportbase + DOR);
        }
      }

      /* Now we can wait */

      if (controller -> iopexqh == NULL) {			// flushtrackbuffer may have cleared event flag
								// just after a new request was queued so test again
        oz_knl_event_waitone (controller -> event);
        oz_knl_event_set (controller -> event, 0);
      }
    }

    /* Otherwise, process request */

    else {
      oz_knl_process_setcur (oz_knl_ioop_getprocess (iopex -> ioop));	// switch processes to access parameters and buffers
      sts = (*(iopex -> be)) (iopex);					// perform the I/O request
      controller -> iopexip = NULL;					// don't let floppy_abort see it now
      oz_knl_iodone (iopex -> ioop, sts, NULL, NULL, NULL);		// post completion
      oz_knl_process_setcur (oz_s_systemproc);				// switch back to neutral territory
    }
  }
}

/* The drive failed initialization tests, mark it as failed then abort any I/O requests that may have queued */
/* Upon return, the devex pointer is no longer valid, and the controller->devexs list might be modified      */

static void setdrivefailed (Devex *devex)

{
  struct _idb *controller;
  Iopex *iopex, **liopex;
  struct _ucb *devunit;
  unsigned long fd;

  /* Set the status string to note that it has failed to initialize */

  setdrivestatus (devex, "init failed");

  /* Abort any pending I/O requests */

  controller = devex -> ucb$ps_pb_idb;
check:
  // fd = oz_hw_smplock_wait (smplock_fd);			// lock this controller's i/o request queue
  devex -> failed = 1;					// mark it as failed so no new requests will queue
  for (liopex = &(controller -> iopexqh); (iopex = *liopex) != NULL; liopex = &(iopex -> next)) {
    if (iopex -> devex != devex) continue;		// skip request if it's not for this drive
    *liopex = iopex -> next;				// unlink the request
    if (*liopex == NULL) controller -> iopexqt = liopex;
    // oz_hw_smplock_clr (smplock_fd, fd);			// release lock and post request
    oz_knl_iodone (iopex -> ioop, OZ_DEVOFFLINE, NULL, NULL, NULL);
    goto check;						// re-scan list for more
  }
  // oz_hw_smplock_clr (smplock_fd, fd);			// none in there, release lock

  /* Call the clonedel routine to actually delete the devunit/devex structs when all channels are deassigned */

  oz_knl_devunit_increfc (devex -> devunit, -1);	// dec ref count, if/when it goes zero, call clonedel
}

/* Set the drive status string in the unit description */

static void setdrivestatus (Devex *devex, const char *status)

{
  char unitdesc[OZ_DEVUNIT_DESCSIZE];
  struct _idb *controller;

  controller = devex -> ucb$ps_pb_idb;
  sprintf (sizeof unitdesc, unitdesc, "(port %X) %s", controller -> idb$q_csr, status);
  oz_knl_devunit_rename (devex -> devunit, NULL, unitdesc);
}

/************************************************************************/
/*									*/
/*  Set volume valid I/O request					*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex -> writing = 0 : turn volume offline, flush track buffer	*/
/*	                   1 : turn volume online (if media is loaded)	*/
/*									*/
/************************************************************************/

static unsigned long be_setvolvalid (Iopex *iopex)

{
  Devex *devex;
  unsigned char dor;
  unsigned long sts;

  devex = iopex -> devex;

  sts = SS$_NORMAL;
  if (iopex -> writing) {					// see if turning it online
    devex -> volvalid = mediaonline (iopex);			// ... see if media is loaded
    if (!(devex -> volvalid)) sts = OZ_VOLNOTVALID;		// ... and maybe return error code
  } else if (devex -> volvalid) {				// turning offline, see if currently online
    sts = flushtrackbuffer (devex);				// if online, flush track buffer
    OZ_KNL_NPPFREE (devex -> trackbuffer);			// ... then free track buffer
    devex -> trackbuffer = NULL;
    devex -> volvalid = 0;					// ... and mark volume offline
    dor  = oz_hw_inb (devex -> ucb$ps_pb_idb -> idb$q_csr + DOR);	// ... and turn the motor off
    dor &= ~ (DOR_MOTEN0 << devex -> drivesel);
    oz_hw_outb (dor, devex -> ucb$ps_pb_idb -> idb$q_csr + DOR);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Write blocks of data to floppy disk					*/
/*									*/
/*    Input:								*/
/*									*/
/*	size  = number of bytes to write				*/
/*	phypages/byteoffs = address of bytes to write			*/
/*	slbn  = starting logical block number				*/
/*	devex = device extension identifying floppy drive to write to	*/
/*									*/
/*	smplock = softint						*/
/*									*/
/*    Output:								*/
/*									*/
/*	write_blocks = OZ_SUCCES : successful write			*/
/*	                    else : error status				*/
/*	data written to floppy						*/
/*									*/
/************************************************************************/

static unsigned long be_writeblocks (Iopex *iopex)

{
  unsigned char command[9], cylinder, msr, results[7], secintrk, sector, track;
  const unsigned char *p;
  Devex *devex;
  OZ_Dbn startblock;
  unsigned long direct, secpertrk, sts, writelen;
  unsigned short port;

  devex = iopex -> devex;

  /* Check volume valid bit */

  sts = chkvolvalid (devex);
  if (sts != SS$_NORMAL) return (sts);

  /* Check write-protect switch */

  if (!(devex -> writable)) return (OZ_WRITELOCKED);

  /* Turn mo-toron (if not already) */

  port = devex -> ucb$ps_pb_idb -> idb$q_csr;
  msr  = oz_hw_inb (port + DOR);
  oz_hw_outb ((msr & 0xFC) | DOR_RESET_ | DOR_DMAGATE | devex -> drivesel | (DOR_MOTEN0 << devex -> drivesel), port + DOR);

  /* Start writing */

  secpertrk = sectorspertrack[devex->sizecode];
  while (iopex -> size > 0) {

    /* If it doesn't start within current track buffer, flush current track buffer and start a new one */

    secintrk   = iopex -> slbn % secpertrk;	// get sector within track we are doing
    startblock = iopex -> slbn - secintrk;	// get block at beginning of track we are doing
    if (startblock != devex -> startblock) {	// see if it is in the track buffer
      sts = flushtrackbuffer (devex);		// if not, flush the track that's in there
      if (sts != SS$_NORMAL) goto rtn;
      devex -> startblock  = startblock;	// then say this track is in there now
      devex -> validblocks = 0;			// ... but none of it is valid
      devex -> dirtybeg    = secpertrk;		// ... and none of it is dirty
      devex -> dirtyend    = 0;
    }
    if (secintrk > devex -> validblocks) {	// see if there is a gap after what's valid and where we're starting
      sts = filltrackbuffer (devex);		// if so, read the rest of the track from the floppy to fill the gap
      if (sts != SS$_NORMAL) goto rtn;
    }

    /* Copy data into track buffer and make sure the dirty limits contain it */

    writelen = (secpertrk - secintrk) << L2DISK_BLOCK_SIZE;	// get how many bytes are left in the track from where we start
    if (writelen > iopex -> size) writelen = iopex -> size;	// ... but don't do more than the caller gave us to write
    oz_hw_phys_movetovirt (writelen, devex -> trackbuffer + (secintrk << L2DISK_BLOCK_SIZE), iopex -> phypages, iopex -> byteoffs);

    if (devex -> dirtybeg > secintrk) devex -> dirtybeg = secintrk;		// maybe move beg of dirty stuff back
    secintrk += writelen >> L2DISK_BLOCK_SIZE;					// get block number after where we wrote
    if (devex -> dirtyend < secintrk) devex -> dirtyend = secintrk;		// maybe move end of dirty stuff down
    if (devex -> validblocks < secintrk) devex -> validblocks = secintrk;	// maybe move end of valid stuff down

    /* Increment and continue */

    iopex -> size -= writelen;				// this fewer bytes to do
    iopex -> slbn += writelen >> L2DISK_BLOCK_SIZE;	// start at this block number next time
    iopex -> byteoffs += writelen;			// start at this offset in buffer next time

    /* Maybe it is being aborted */

    sts = OZ_ABORTED;
    if (iopex -> aborted) goto rtn;
  }

  /* If writethru mode, flush the track buffer out to the floppy */

  sts = SS$_NORMAL;
  if (iopex -> writethru) sts = flushtrackbuffer (devex);

rtn:
  motor_timer_start (devex);
  return (sts);
}

/************************************************************************/
/*									*/
/*  Read blocks of data from floppy disk				*/
/*									*/
/*    Input:								*/
/*									*/
/*	size  = number of bytes to read					*/
/*	slbn  = starting logical block number				*/
/*	devex = device extension identifying floppy drive to read from	*/
/*									*/
/*	smplock = softint						*/
/*									*/
/*    Output:								*/
/*									*/
/*	read_blocks = OZ_SUCCES : successful read			*/
/*	                   else : error status				*/
/*	phypages/byteoffs = filled with data read from floppy		*/
/*									*/
/************************************************************************/

static unsigned long be_readblocks (Iopex *iopex)

{
  unsigned char command[9], cylinder, endintrk, msr, results[7], secintrk, sector, track;
  Devex *devex;
  int retries;
  OZ_Dbn startblock;
  unsigned long direct, readlen, secpertrk, sts;
  unsigned short port;

  devex = iopex -> devex;

  /* Check volume valid bit */

  sts = chkvolvalid (devex);
  if (sts != SS$_NORMAL) return (sts);

  /* Turn motor on (if not already) */

  port = devex -> ucb$ps_pb_idb -> idb$q_csr;
  msr  = oz_hw_inb (port + DOR);
  oz_hw_outb ((msr & 0xFC) | DOR_RESET_ | DOR_DMAGATE | devex -> drivesel | (DOR_MOTEN0 << devex -> drivesel), port + DOR);

  /* Start reading */

  secpertrk = sectorspertrack[devex->sizecode];
  while (iopex -> size > 0) {

    /* If it doesn't start within current track buffer, flush current track buffer and start a new one */

    secintrk   = iopex -> slbn % secpertrk;			// get sector within track we are doing
    startblock = iopex -> slbn - secintrk;			// get block at beginning of track we are doing
    if (startblock != devex -> startblock) {			// see if it is in the track buffer
      sts = flushtrackbuffer (devex);				// if not, flush the track that's in there
      if (sts != SS$_NORMAL) goto rtn;
      devex -> startblock  = startblock;			// then say this track is in there now
      devex -> validblocks = 0;					// ... but none of it is valid
      devex -> dirtybeg    = secintrk;				// ... and none of it is dirty
      devex -> dirtyend    = 0;
    }
    readlen  = (secpertrk - secintrk) << L2DISK_BLOCK_SIZE;	// this is now many bytes to end of track
    if (readlen > iopex -> size) readlen = iopex -> size;	// if more than we want, read just enough
    endintrk = secintrk + (readlen >> L2DISK_BLOCK_SIZE);
    if (endintrk > devex -> validblocks) {			// see if there is a gap after what's valid and where we end reading
      sts = filltrackbuffer (devex);				// if so, read the rest of the track from the floppy to fill the gap
      if (sts != SS$_NORMAL) goto rtn;
    }

    /* Copy from track buffer, increment and continue */

    oz_hw_phys_movefromvirt (readlen, devex -> trackbuffer + (secintrk << L2DISK_BLOCK_SIZE), iopex -> phypages, iopex -> byteoffs);

    iopex -> size -= readlen;					// decrement what we have left to do
    iopex -> slbn += readlen >> L2DISK_BLOCK_SIZE;		// increment block number
    iopex -> byteoffs += readlen;				// increment where we put what's left

    /* Maybe it is being aborted */

    sts = OZ_ABORTED;
    if (iopex -> aborted) goto rtn;
  }

  sts = SS$_NORMAL;

rtn:
  motor_timer_start (devex);
  return (sts);
}

/************************************************************************/
/*									*/
/*  Format a floppy							*/
/*									*/
/*    Input:								*/
/*									*/
/*	size  = size code (0=128, 1=256, 2=512, ...)			*/
/*	devex = device extension identifying floppy drive to write to	*/
/*									*/
/*	smplock = softint						*/
/*									*/
/*    Output:								*/
/*									*/
/*	be_format = OZ_SUCCES : successful				*/
/*	                 else : error status				*/
/*									*/
/************************************************************************/

static unsigned long be_format (Iopex *iopex)

{
  struct _idb *controller;
  Devex *devex;
  int i;
  OZ_Datebin when;
  unsigned char command[6], cylinder, gaplength, msr, *p, results[7], sector, track;
  unsigned long secpertrk, sts, writelen;
  unsigned short port;

  devex = iopex -> devex;
  controller = devex -> ucb$ps_pb_idb;

  /* Turn mo-toron (if not already) */

  port = controller -> idb$q_csr;
  msr  = oz_hw_inb (port + DOR);
  oz_hw_outb ((msr & 0xFC) | DOR_RESET_ | DOR_DMAGATE | devex -> drivesel | (DOR_MOTEN0 << devex -> drivesel), port + DOR);

  /* Start formatting */

  devex -> sizecode = iopex -> size;
  gaplength  = iopex -> byteoffs;
  iopex -> byteoffs = 0;
  secpertrk = sectorspertrack[devex->sizecode];
  writelen  = secpertrk * 4;
  printk ("oz_dev_floppy_486: formatting %s, sector size %u, gap length %u\n", devex -> unitname, DISK_BLOCK_SIZE, gaplength);

  oz_hw_outb (0, port + CCR);					// set data rate

  for (cylinder = 0; cylinder < CYLINDERS; cylinder ++) {
    printk ("\roz_dev_floppy_486: formatting %s cylinder %2u ...       ", devex -> unitname, cylinder);

    /* Seek to the cylinder to be formatted */

    sts = OZ_IOFAILED;
    if (cylinder == 0) {
      command[0] = 0x07;					// command = RECALIBRATE
      command[1] = devex -> drivesel;				// drive select bits
      if (!send_command (2, command, controller, 1)) goto rtn;
    } else {
      command[0] = 0x0F;					// command = SEEK
      command[1] = devex -> drivesel;				// drive select bits
      command[2] = cylinder;					// cylinder number
      if (!send_command (3, command, controller, 1)) goto rtn;
    }
    command[0] = 0x08;						// command = sense interrupt status
    if (!send_command (1, command, controller, 0)) goto rtn;
    sts = recv_results (2, results, port, devex, 0x08);		// read seek results
    if (sts != SS$_NORMAL) goto rtn;
    if ((results[0] & 0xE0) != 0x20) {
      printk ("\noz_dev_floppy: %s seek to cyl %u did not complete, st0 %X\n", devex -> unitname, cylinder, results[0]);
      sts = OZ_IOFAILED;
      goto rtn;
    }
    if (results[1] != cylinder) {
      printk ("\noz_dev_floppy: %s seek went to cylinder %u instead of %u\n", devex -> unitname, results[1], cylinder);
      sts = OZ_IOFAILED;
      goto rtn;
    }

    /* Wait a settling time */

    when  = oz_hw_tod_getnow ();								// get what time it is now
    when += OZ_TIMER_RESOLUTION / 2;								// get what time it will be in 1/2 sec
    do {
      controller -> waitingforint = 0;								// we're just waiting for interrupt timeout timer
      oz_knl_timer_insert (controller -> inttimer, when, int_timeout, controller);		// put timer in queue
      while (controller -> waitingforint >= 0) {						// wait for it to time out
        oz_knl_event_waitone (controller -> event);
        oz_knl_event_set (controller -> event, 0);
      }
    } while (oz_hw_tod_getnow () < when);							// repeat if it woke too soon

    /* Do each track in the cylinder */

    for (track = 0; track < TRACKSPERCYLINDER; track ++) {

      /* Put formatting data in temp DMA buffer and set up dma registers    */
      /* phypages = NULL so dmapage_setup routine knows to use dmapage_buff */

      printk ("f");
      LOCKDMA ();
      dmapage_setup (DMA_MEM2DEV, secpertrk * 4, devex, 0, NULL, 0);
      p = dmapage_addr;
      for (sector = 1; sector <= secpertrk; sector ++) {
        *(p ++) = cylinder;
        *(p ++) = track;
        *(p ++) = sector;
        *(p ++) = devex -> sizecode;
      }

      /* Send a format track command to the controller */

      command[0] = 0x4D;					// command = MFM, FORMAT TRACK
      command[1] = (track << 2) + devex -> drivesel;		// drive select and head select bits
      command[2] = devex -> sizecode;				// 00=128 bytes per sector, 01=256 bytes per sector, ...
      command[3] = secpertrk;					// number of sectors in the track
      command[4] = gaplength;
      command[5] = 0x00;					// initial data fill byte value
      if (!send_command (6, command, controller, 1)) {
        UNLKDMA ();
        sts = OZ_IOFAILED;
        goto rtn;
      }

      /* Receive the results back */

      sts = recv_results (7, results, port, devex, 0x4D);
      UNLKDMA ();
      if (sts != SS$_NORMAL) goto rtn;
      if ((results[0] & 0xC0) != 0x00) {
        printk ("\n");
        print_rw_err ("format", devex, command, results, writelen);
        sts = OZ_IOFAILED;
        goto rtn;
      }

      /* Now write all zeroes to the track */

      printk ("w");
      LOCKDMA ();
      dmapage_setup (DMA_MEM2DEV, secpertrk * DISK_BLOCK_SIZE, devex, 0, NULL, 0);
      memset (dmapage_addr, 0x69, secpertrk * DISK_BLOCK_SIZE);

      command[0] = 0x45;				/* command = MFM, WRITE */
      command[1] = (track << 2) + devex -> drivesel;	/* drive select and head select bits */
      command[2] = cylinder;				/* cylinder number (starting at zero) */
      command[3] = track;				/* track number (head select, 0 or 1) */
      command[4] = 1;					/* sector number (starting at one) */
      command[5] = devex -> sizecode;			/* 00=128 bytes per sector, 01=256 bytes per sector, ... */
      command[6] = secpertrk;				/* last sector on track (inclusive) */
      command[7] = GPL_1;
      command[8] = 0xFF;				/* data transfer length = maximum */
      sts = OZ_IOFAILED;
      if (!send_command (9, command, devex -> ucb$ps_pb_idb, 1)) {
        UNLKDMA ();
        goto rtn;
      }

      sts = recv_results (7, results, devex -> ucb$ps_pb_idb -> idb$q_csr, devex, 0x45);
      UNLKDMA ();
      if (sts != SS$_NORMAL) goto rtn;
      if ((results[0] & 0xC0) != 0x00) {
        printk ("\n");
        print_rw_err ("write", devex, command, results, writelen);
        sts = OZ_IOFAILED;
        goto rtn;
      }

      /* Now read the whole track back */

      printk ("r");
      LOCKDMA ();
      dmapage_setup (DMA_DEV2MEM, secpertrk * DISK_BLOCK_SIZE, devex, 0, NULL, 0);

      command[0] = 0x42;				/* command = MFM, READ TRACK */
      command[1] = (track << 2) + devex -> drivesel;	/* drive select and head select bits */
      command[2] = cylinder;				/* cylinder number (starting at zero) */
      command[3] = track;				/* track number (head select, 0 or 1) */
      command[4] = 1;					/* sector number (starting at one) */
      command[5] = devex -> sizecode;			/* 00=128 bytes per sector, 01=256 bytes per sector, ... */
      command[6] = secpertrk;				/* last sector on track (inclusive) */
      command[7] = GPL_1;
      command[8] = 0xFF;				/* data transfer length = maximum */
      sts = OZ_IOFAILED;
      if (!send_command (9, command, devex -> ucb$ps_pb_idb, 1)) {
        UNLKDMA ();
        goto rtn;
      }

      sts = recv_results (7, results, devex -> ucb$ps_pb_idb -> idb$q_csr, devex, 0x42);
      UNLKDMA ();
      if (sts != SS$_NORMAL) goto rtn;
      if ((results[0] & 0xC0) != 0x00) {
        printk ("\n");
        print_rw_err ("read", devex, command, results, writelen);
        sts = OZ_IOFAILED;
        goto rtn;
      }

      /* Maybe it is being aborted */

      sts = OZ_ABORTED;
      if (iopex -> aborted) goto rtn;
    }
  }

  command[0] = 0x07;						// command = RECALIBRATE
  command[1] = devex -> drivesel;				// drive select bits
  send_command (2, command, controller, 1);			// retract the heads to cyl 0
  sts = SS$_NORMAL;						// we're done

rtn:
  if (sts == SS$_NORMAL) printk ("\noz_dev_floppy_486: format %s successful\n", devex -> unitname);
  else printk ("\noz_dev_floppy_486: error %u formatting %s\n", sts, devex -> unitname);
  motor_timer_start (devex);
  return (sts);
}

/************************************************************************/
/*									*/
/*  See if a volume is loaded in floppy drive and reset media changed 	*/
/*  flag								*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex = I/O requesting media online check			*/
/*	smplevel = softint						*/
/*									*/
/*    Output:								*/
/*									*/
/*	mediaonline = 0 : media not online				*/
/*	              1 : media is online				*/
/*									*/
/************************************************************************/

static int mediaonline (Iopex *iopex)

{
  struct _idb *controller;
  Devex *devex;
  int rc, retries;
  unsigned char command[3], dir, results[7];
  unsigned long byteoffs, pageoffs, sts, tracksize;
  unsigned short ioportbase;

  devex      = iopex -> devex;
  controller = devex -> ucb$ps_pb_idb;
  ioportbase = controller -> idb$q_csr;

  if (devex -> trackbuffer != NULL) {
    OZ_KNL_NPPFREE (devex -> trackbuffer);
    devex -> trackbuffer = NULL;
  }

  rc = 0;							/* assume we will fail to find media online */

  for (devex -> datarate = 0; devex -> datarate < 4; devex -> datarate ++) {
    if (iopex -> aborted) goto done;
    if (!resetctrlr (controller, devex -> datarate)) {		/* reset controller */
      printk ("oz_dev_floppy mediaonline: %s controller failed to reset\n", controller -> ctrlname);
      goto done;
    }
    if (iopex -> aborted) goto done;
    if (!recalibrate (devex)) goto done;			/* try to recalibrate drive */
    retries = 0;						/* DSKCHG flag reset retry counter */

retry:
    if (iopex -> aborted) goto done;
    command[0] = 0x0F;						/* command = seek */
    command[1] = devex -> drivesel;				/* drive select and head select bits */
    command[2] = retries + 1;					/* cylinder number (starting at zero) */
    if (!send_command (3, command, controller, 1)) goto done;
    if (!command_done (ioportbase, 0x0F)) goto done;
    driveidlewait (devex);					/* wait for drive to not be busy */
    if (iopex -> aborted) goto done;
    command[0] = 0x08;						/* send sense interrupt command */
    if (!send_command (1, command, controller, 0)) goto done;
    if (recv_results (2, results, ioportbase, devex, 0x08) != SS$_NORMAL) goto done;
    if (results[0] & 0xC0) {
      printk ("oz_dev_floppy: %s recal error st0=%x, pcn=%x\n", devex -> unitname, results[0], results[1]);
      goto done;
    }
								/* if it came ready in there, media is online */
    dir = oz_hw_inb (ioportbase + DIR);				/* make sure the DIR_DSKCHG_ bit is clear */
    if (dir & DIR_DSKCHG_) {
      if (++ retries < 3) goto retry;				/* if DIR_DSKCHG_ bit is set, seek again to clear it */
      printk ("oz_dev_floppy: %s media offline\n", devex -> unitname);
      goto done;
    }

    if (iopex -> aborted) goto done;
    devex -> writable = 0;					/* assume it is not write enabled */
    command[0] = 0x04;						/* 'sense drive status' command */
    command[1] = devex -> drivesel;				/* drive select and head select bits */
    if (!send_command (2, command, controller, 0)) goto done;	/* sense the drive's status */
    sts = recv_results (1, results, ioportbase, devex, 0x04);	/* read the results back */
    if (sts != SS$_NORMAL) goto done;
    if (!(results[0] & ST3_WP)) devex -> writable = 1;		/* it is write enabled */

    if (iopex -> aborted) goto done;
    command[0] = 0x4A;						/* read ID to see if data rate it correct and to get block size code */
    command[1] = devex -> drivesel;
    if (!send_command (2, command, controller, 1)) goto done;
    sts = recv_results (7, results, ioportbase, devex, 0x4A);
    if (sts != SS$_NORMAL) goto done;
    if ((results[0] & 0xC3) == devex -> drivesel) {		/* check the results */
      printk ("oz_dev_floppy: %s datarate %u seems to work\n", devex -> unitname, devex -> datarate);
      devex -> sizecode = results[6];
      if (results[6] >= MAXSIZECODE) printk ("oz_dev_floppy: %s size code %u too big\n", devex -> unitname, results[6]);
      else if (L2DISK_BLOCK_SIZE > OZ_HW_L2PAGESIZE) printk ("oz_dev_floppy: %s block size %u greater than page size\n", devex -> unitname, DISK_BLOCK_SIZE);
      else {
        rc = devex -> datarate + 1;				/* if everything worked, media is online */
        break;
      }
    } else {
      printk ("oz_dev_floppy: %s failed datarate %u, st0=%x, st1=%x, st2=%x\n", 
                     devex -> unitname, devex -> datarate, results[0], results[1], results[2]);
    }
  }

  /* If working datarate was found, allocate a track buffer */

  if (rc > 0) {
    devex -> datarate = rc - 1;
    printk ("oz_dev_floppy: %s using datarate %u\n", devex -> unitname, devex -> datarate);
    printk ("oz_dev_floppy: %s block size %u\n", devex -> unitname, DISK_BLOCK_SIZE);
    tracksize = sectorspertrack[devex->sizecode] << L2DISK_BLOCK_SIZE;
    devex -> trackbuffer = kmalloc (tracksize,GFP_KERNEL);	/* malloc a track buffer */
    rc = 0;							/* get its physical page array for DMAing */
    for (byteoffs = 0; byteoffs < tracksize; byteoffs += sts) {
      if (rc > sizeof devex -> trackbuf_phypages / sizeof devex -> trackbuf_phypages[0]) {
        panic ("oz_dev_floppy mediaonline: trackbuffer %p spans too many pages %d", devex -> trackbuffer, rc);
      }
      sts = oz_knl_misc_sva2pa (devex -> trackbuffer + byteoffs, devex -> trackbuf_phypages + rc, &pageoffs);
      if (rc == 0) devex -> trackbuf_pageoffs = pageoffs;
      else if (pageoffs != 0) {
        panic ("oz_dev_floppy mediaonline: trackbuffer %p pageoffs[%d] = %X", devex -> trackbuffer, rc, pageoffs);
      }
      rc ++;
    }
    devex -> startblock  = 0;					/* say track buffer is empty */
    devex -> validblocks = 0;
    devex -> dirtybeg    = sectorspertrack[devex->sizecode];
    devex -> dirtyend    = 0;
    rc = 1;
    printk ("oz_dev_floppy: %s media %s\n", devex -> unitname, devex -> writable ? "read/write" : "read-only");
  }

done:
  motor_timer_start (devex);					/* restart motor timer */
  return (rc);							/* return resultant status */
}

/************************************************************************/
/*									*/
/*  Flush dirty blocks from current track buffer to the floppy		*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex -> startblock  = starting block number of the track	*/
/*	devex -> dirtybeg    = starting block within the track that's	*/
/*	                       dirty (zero based)			*/
/*	devex -> dirtyend    = ending block (exclusive) that's dirty	*/
/*	devex -> trackbuffer = virtual address of track buffer		*/
/*									*/
/*    Output:								*/
/*									*/
/*	flushtrackbuffer = SS$_NORMAL : successful			*/
/*	                         else : write error status		*/
/*	devex -> dirtybeg = devex -> dirtyend				*/
/*									*/
/************************************************************************/

static unsigned long flushtrackbuffer (Devex *devex)

{
  unsigned char command[9], cylinder, results[7], secpertrk, sector, track;
  unsigned long direct, sts, writelen;

  /* Calculate cylinder, track and sector that we will start writing */

  secpertrk = sectorspertrack[devex->sizecode];
  track     = devex -> startblock / secpertrk;
  cylinder  = track / TRACKSPERCYLINDER;
  track    %= TRACKSPERCYLINDER;
  sector    = devex -> dirtybeg;

  /* Repeat as long as there are dirty blocks to process */

  while (devex -> dirtyend > sector) {

    /* Determine how much to write = to end of dirty blocks or end of dma buffer */

    writelen = (devex -> dirtyend - sector) << L2DISK_BLOCK_SIZE;
    if (writelen > dmapage_size) writelen = dmapage_size;

    /* Set up DMA registers, copy data to temp dma buffer if track buffer not directly accessible */

    LOCKDMA ();
    direct = dmapage_setup (DMA_MEM2DEV, writelen, devex, devex -> trackbuf_pageoffs + (sector << L2DISK_BLOCK_SIZE), 
                            devex -> trackbuf_phypages, devex -> startblock + sector);
    if (direct != 0) writelen = direct;
    else memcpy (dmapage_addr, devex -> trackbuffer + (sector << L2DISK_BLOCK_SIZE), writelen);

    /* Send a write command to the controller */

    command[0] = 0xC5;					/* command = MULTITRACK, MFM, WRITE */
    command[1] = (track << 2) + devex -> drivesel;	/* drive select and head select bits */
    command[2] = cylinder;				/* cylinder number (starting at zero) */
    command[3] = track;					/* track number (head select, 0 or 1) */
    command[4] = sector + 1;				/* sector number (starting at one) */
    command[5] = devex -> sizecode;			/* 00=128 bytes per sector, 01=256 bytes per sector, ... */
    command[6] = secpertrk;				/* last sector on track (inclusive) */
    command[7] = GPL_1;
    command[8] = 0xFF;					/* data transfer length = maximum */
    if (!send_command (9, command, devex -> ucb$ps_pb_idb, 1)) {
      UNLKDMA ();
      return (OZ_IOFAILED);
    }

    /* Receive the results back */

    sts = recv_results (7, results, devex -> ucb$ps_pb_idb -> idb$q_csr, devex, 0xC5);
    UNLKDMA ();
    if (sts != SS$_NORMAL) return (sts);
    if ((results[0] & 0xC0) != 0x00) {
      print_rw_err ("write", devex, command, results, writelen);
      return (OZ_IOFAILED);
    }

    /* Maybe there are more sectors in the track that are dirty */

    sector += writelen >> L2DISK_BLOCK_SIZE;
  }

  devex -> dirtybeg = sector;

  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Fill track buffer by reading track from floppy			*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex -> startblock  = starting block number of the track	*/
/*	devex -> validblocks = how many blocks in track are already valid
/*	devex -> trackbuffer = virtual address of track buffer		*/
/*									*/
/*    Output:								*/
/*									*/
/*	filltrackbuffer = SS$_NORMAL : successful			*/
/*	                        else : read error status		*/
/*	devex -> validblocks = the whole track				*/
/*									*/
/************************************************************************/

static unsigned long filltrackbuffer (Devex *devex)

{
  int retries;
  unsigned char command[9], cylinder, results[7], secpertrk, sector, track;
  unsigned long direct, readlen, sts;

  /* Calculate cylinder, track and sector that we will start reading */

  secpertrk = sectorspertrack[devex->sizecode];
  track     = devex -> startblock / secpertrk;
  cylinder  = track / TRACKSPERCYLINDER;
  track    %= TRACKSPERCYLINDER;
  sector    = devex -> validblocks;

  /* Keep reading until we have the whole track */

  while (sector < secpertrk) {

    /* Determine how much to read = to end of track or end of dma buffer */

    readlen = (secpertrk - sector) << L2DISK_BLOCK_SIZE;
    if (readlen > dmapage_size) readlen = dmapage_size;

    /* Set up DMA registers to transfer directly to track buffer */

    retries = 3;
retry:
    LOCKDMA ();
    direct = dmapage_setup (DMA_DEV2MEM, readlen, devex, devex -> trackbuf_pageoffs + (sector << L2DISK_BLOCK_SIZE), 
                            devex -> trackbuf_phypages, devex -> startblock + sector);
    if ((direct != 0) && (direct < readlen)) readlen = direct;

    /* Send a read command to the controller */

    command[0] = 0xE6;							// command = MULTITRACK, MFM, SKIP_DELETED, READ
    command[1] = (track << 2) + devex -> drivesel;			// drive select and head select bits
    command[2] = cylinder;						// cylinder number (starting at zero)
    command[3] = track;							// track number (head select, 0 or 1)
    command[4] = sector + 1;						// sector number (starting at one)
    command[5] = devex -> sizecode;					// 00=128 bytes per sector, 01=256 bytes per sector, ...
    command[6] = secpertrk;						// last sector on track (inclusive)
    command[7] = GPL_1;
    command[8] = 0xFF;							// data transfer length = maximum
    if (!send_command (9, command, devex -> ucb$ps_pb_idb, 1)) {		// send command, wait for interrupt
      UNLKDMA ();
      return (OZ_IOFAILED);
    }

    /* Receive the results back */

    sts = recv_results (7, results, devex -> ucb$ps_pb_idb -> idb$q_csr, devex, 0xE6);
    UNLKDMA ();
    if (sts != SS$_NORMAL) {
      readlen = 512;
      if (-- retries > 0) goto retry;
      return (sts);
    }
    if ((results[0] & 0xC0) != 0x00) {					// check for any error
      print_rw_err ("read", devex, command, results, readlen);
      readlen = 512;							// on retry, just do one block
      if (-- retries > 0) goto retry;
      return (OZ_IOFAILED);
    }

    /* If data is in temp dma buffer, copy to track buffer */

    if (direct == 0) memcpy (devex -> trackbuffer + (sector << L2DISK_BLOCK_SIZE), dmapage_addr, readlen);

    /* Maybe there are more sectors in the track */

    sector += readlen >> L2DISK_BLOCK_SIZE;
  }

  devex -> validblocks = sector;

  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Reset the controller then enable it					*/
/*									*/
/*    Input:								*/
/*									*/
/*	controller = controller to be reset				*/
/*	datarate   = what to program the data rate to			*/
/*									*/
/*    Output:								*/
/*									*/
/*	resetctrlr = 0 : controller never came ready			*/
/*	             1 : controller is ready				*/
/*									*/
/************************************************************************/

static int resetctrlr (struct _idb *controller, unsigned char datarate)

{
  unsigned char command[4];
  unsigned short ioportbase;

  ioportbase = controller -> idb$q_csr;

  /* Reset it */

  oz_hw_outb (0, ioportbase + DOR);				/* make reset line active */
  oz_hw_stl_microwait (2000, NULL, NULL);			/* wait at least 2mS */
  oz_hw_outb (DOR_RESET_ | DOR_DMAGATE, ioportbase + DOR);	/* release reset line */

  /* Wait for it to come ready */

  if (!oz_hw_stl_microwait (200000, resetctrlr_check, &ioportbase)) { /* wait up to 200mS for it to come ready */
    printk ("oz_dev_floppy resetctrlr: %s didn't come ready, msr %2.2X\n", controller -> ctrlname, oz_hw_inb (ioportbase + MSR));
    return (0);							/* never came ready, fail */
  }

  /* Configure it */

  oz_hw_outb (datarate, ioportbase + CCR);			/* set data rate */

#if 000 // screws up bochs
  command[0] = 0x12;						/* PERPENDICULAR command code */
  command[1] = 0;						/* disable perpendicular mode */
  if (!send_command (2, command, controller, 0)) return (0);
  if (!command_done (ioportbase, 0x12)) return (0);
#endif

  command[0] = 0x13;						/* CONFIGURE command code */
  command[1] = 0;
  command[2] = 0x4B;						/* - enable implied seek, enable fifo, fifo threshold=12 (0b+1) */
  command[3] = 0;
  if (!send_command (4, command, controller, 0)) return (0);
  if (!command_done (ioportbase, 0x13)) return (0);

  command[0] = 0x03;						/* SPECIFY command code */
  command[1] = 0;
  command[2] = 0;						/* - dma mode */
  if (!send_command (3, command, controller, 0)) return (0);
  if (!command_done (ioportbase, 0x03)) return (0);

  return (1);
}

static unsigned long resetctrlr_check (void *ioportbasev)

{
  unsigned char msr;
  unsigned short ioportbase;

  ioportbase = *(unsigned short *)ioportbasev;

  msr = oz_hw_inb (ioportbase + MSR);			// read the master status register
  return ((msr & (MSR_RQM | MSR_CB)) == MSR_RQM);	// success if it is ready to accept a command
}

/************************************************************************/
/*									*/
/*  Recalibrate floppy							*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = drive to recalibrate					*/
/*									*/
/*    Output:								*/
/*									*/
/*	recalibrate = 0 : failed					*/
/*	              1 : successful					*/
/*									*/
/************************************************************************/

static int recalibrate (Devex *devex)

{
  struct _idb *controller;
  unsigned char command[2], dor, results[2];
  unsigned short ioportbase;

  controller = devex -> ucb$ps_pb_idb;
  ioportbase = controller -> idb$q_csr;

  dor  = oz_hw_inb (ioportbase + DOR) & 0xFC;				/* turn on its motor */
  dor |= (DOR_MOTEN0 << devex -> drivesel) | devex -> drivesel | DOR_RESET_ | DOR_DMAGATE;
  oz_hw_outb (dor, ioportbase + DOR);
  command[0] = 0x07;							/* send recalibrate command to drive */
  command[1] = devex -> drivesel;
  if (!send_command (2, command, controller, 1)) return (0);
  driveidlewait (devex);						/* wait for drive to not be busy */
  command[0] = 0x08;							/* send sense interrupt command */
  if (!send_command (1, command, controller, 0)) return (0);
  if (recv_results (2, results, ioportbase, NULL, 0x08) != SS$_NORMAL) return (0);
  if (results[0] & 0xC0) {
    printk ("oz_dev_floppy: %s recal error st0=%x, pcn=%x\n", devex -> unitname, results[0], results[1]);
    return (0);
  }
  command[0] = 0x04;							/* send sense drive status command */
  command[1] = devex -> drivesel;
  if (!send_command (2, command, controller, 0)) return (0);
  if (recv_results (1, results, ioportbase, NULL, 0x04) != SS$_NORMAL) return (0);
  if (!(results[0] & ST3_TRK0)) {
    printk ("oz_dev_floppy: %s not at track 0 after recal (st3=%x)\n", devex -> unitname, results[0]);
    return (0);
  }

  return (1);
}

/************************************************************************/
/*									*/
/*  Wait up to 'DRIVE_RDY_TIME' seconds for drive busy bit to turn off	*/
/*									*/
/*  This routine waits 50mS before checking the bit for the first time 	*/
/*  and it waits 50mS after seeing the bit off before it returns.	*/
/*									*/
/************************************************************************/

static void driveidlewait (Devex *devex)

{
  struct _idb *controller;
  int i, j;
  OZ_Datebin when;
  unsigned char msr;

  controller = devex -> ucb$ps_pb_idb;
  j = 1;										// haven't seen it idle yet
  for (i = 20 * DRIVE_RDY_TIME; -- i >= 0;) {
    controller -> waitingforint = 0;							// we want to wait for the timeout
    when  = oz_hw_tod_getnow ();							// get current time
    when += OZ_TIMER_RESOLUTION / 20;							// calc when timeout should happen
    oz_knl_timer_insert (controller -> inttimer, when, int_timeout, controller);	// start the timer going
    while (controller -> waitingforint >= 0) {						// wait for 50mS
      oz_knl_event_waitone (controller -> event);
      oz_knl_event_set (controller -> event, 0);
    }
    msr = oz_hw_inb (controller -> idb$q_csr + MSR);					// check the drive's busy bit
    if (!(msr & (1 << devex -> drivesel)) && (-- j < 0)) break;				// if not busy twice, we're done
  }
}

/************************************************************************/
/*									*/
/*  Check the 'media changed' status bit and set datarate		*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = device extension pointer				*/
/*									*/
/*	smplock = softint, lock set					*/
/*									*/
/*    Output:								*/
/*									*/
/*	chkvolvalid = SS$_NORMAL : disk not changed			*/
/*	          OZ_VOLNOTVALID : disk was changed			*/
/*									*/
/************************************************************************/

static unsigned long chkvolvalid (Devex *devex)

{
  unsigned char dir;

  if (devex -> volvalid) {							// maybe it was invalidated by software
    dir = oz_hw_inb (devex -> ucb$ps_pb_idb -> idb$q_csr + DIR);			// now check to see if hardware invalidated it
    if (dir & DIR_DSKCHG_) {
      printk ("oz_dev_floppy: %s media changed\n", devex -> unitname);	// if so, print a message
      devex -> volvalid = 0;							// ... and software invalidate it, too
    } else {
      oz_hw_outb (devex -> datarate, devex -> ucb$ps_pb_idb -> idb$q_csr + CCR);	// ok, set the datarate for this drive
    }
  }
  return (devex -> volvalid ? SS$_NORMAL : OZ_VOLNOTVALID);
}

/************************************************************************/
/*									*/
/*  Restart motor timer for a drive					*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = pointer to device to start the timer for		*/
/*	smplock = softint						*/
/*									*/
/************************************************************************/

static void motor_timer_start (Devex *devex)

{
  OZ_Datebin now;

  now = oz_hw_tod_getnow ();
  devex -> motoroff = now + (MOTOR_TIMER_SECONDS * OZ_TIMER_RESOLUTION);
  oz_knl_timer_insert (devex -> motor_timer, devex -> motoroff, motor_timer_expired, devex);
}

/************************************************************************/
/*									*/
/*  This routine is called back (at softint level) by the timer when 	*/
/*  the motor timer expires						*/
/*									*/
/************************************************************************/

static void motor_timer_expired (void *devexv, OZ_Timer *timer)

{
  Devex *devex;
  OZ_Datebin now;

  devex = devexv;
  now   = oz_hw_tod_getnow ();
  if (now >= devex -> motoroff) oz_knl_event_set (devex -> ucb$ps_pb_idb -> event, 1);
  else oz_knl_timer_insert (timer, devex -> motoroff, motor_timer_expired, devex);
}

/************************************************************************/
/*									*/
/*  Send a command to the floppy disk controller and wait for 		*/
/*  corresponding interrupt (if any)					*/
/*									*/
/*    Input:								*/
/*									*/
/*	size = number of command bytes to send				*/
/*	buff = address of command bytes to send				*/
/*	controller = the controller to send the command to		*/
/*	interrupts = 0 : the command does not produce an interrupt	*/
/*	             1 : wait for the command to interrupt		*/
/*									*/
/*    Output:								*/
/*									*/
/*	send_command = 0: error sending command				*/
/*	               1: success					*/
/*	command sent to controller					*/
/*									*/
/************************************************************************/

static int send_command (int size, unsigned char *buff, struct _idb *controller, int interrupts)

{
  int i;
  OZ_Datebin when;
  unsigned char msr;
  unsigned short port;

  port = controller -> idb$q_csr;

  /* Send command to the controller, byte by byte */

  /* While sending the command, RQM should be set (indicating the controller is ready to accept command bytes or send result bytes) */
  /*                            DIO should be clear (indicating the controller is accepting command bytes now) */
  /*                            CB should be clear for the first byte (indicating the controller is ready for the first byte of a command) */
  /*                               then it should be set until we read the last result byte from it */

  for (i = 0; i < size; i ++) {
    msr = wait_for_rqm (port);
    if (!(msr & MSR_RQM)) return (0);
    if ((msr & MSR_DIO) || ((i > 0) && !(msr & MSR_CB))) {
      printk ("oz_dev_floppy: %s premature termination of %2.2X command after %d bytes, msr %X\n", 
                     controller -> ctrlname, buff[0], i, msr);
      return (0);
    }
    if (i == size - 1) controller -> waitingforint = interrupts;
    oz_hw_outb (buff[i], port + FIFO);
  }

  /* If the command produces an interrupt, wait for the interrupt.  The interrupt routine will clear */
  /* 'waitingforint' only when it sees msr<RQM> set.  The timeout routine sets watingforint = -1.    */

  if (controller -> waitingforint > 0) {
    when  = oz_hw_tod_getnow ();							// get current time
    when += INT_TIMEOUT * OZ_TIMER_RESOLUTION;						// calc when timeout should happen
    oz_knl_timer_insert (controller -> inttimer, when, int_timeout, controller);	// start the timer going
    while (controller -> waitingforint > 0) {						// wait for interrupt or timeout
      oz_knl_event_waitone (controller -> event);
      oz_knl_event_set (controller -> event, 0);
    }
    if (!oz_knl_timer_remove (controller -> inttimer)) {				// remove timer from queue
      while (controller -> waitingforint >= 0) {					// couldn't, wait for timeout
        oz_knl_event_waitone (controller -> event);
        oz_knl_event_set (controller -> event, 0);
      }
    }
  }

  return (1);
}

/* This routine is called at softint level when we have waited too long for an interrupt */

static void int_timeout (void *controllerv, OZ_Timer *timer)

{
  struct _idb *controller;

  controller = controllerv;
  controller -> waitingforint = -1;		// say we have executed
  oz_knl_event_set (controller -> event, 1);	// wake in case it is still waiting
}

/************************************************************************/
/*									*/
/*  Receive results from the floppy disk controller			*/
/*									*/
/*    Input:								*/
/*									*/
/*	size = number of command bytes to send				*/
/*	buff = address of command bytes to send				*/
/*	port = I/O port base (0x3F0 or 0x370)				*/
/*	devex = device extension data pointer (or NULL if none)		*/
/*									*/
/*    Output:								*/
/*									*/
/*	recv_results = SS$_NORMAL : success				*/
/*	                     else : error receiving results		*/
/*									*/
/************************************************************************/

static unsigned long recv_results (int size, unsigned char *buff, unsigned short port, Devex *devex, unsigned char command)

{
  unsigned char msr;
  int dtc, i;

  dtc = 0;
  for (i = 0; i < size;) {
    msr = wait_for_rqmdio (port);
    if (!(msr & MSR_RQM)) goto volnotvalid;
    if (msr & MSR_NONDMA) {
      if (dtc == 0) printk ("oz_dev_floppy: %s trying to read results but still in data transfer mode on port %x\n", 
	devex -> ucb$ps_pb_idb -> ctrlname, port);
      dtc ++;
      continue;
    }
    if (!(msr & MSR_DIO)) goto premature;
    if ((i < size - 1) && !(msr & MSR_CB)) goto busyclear;
    buff[i++] = oz_hw_inb (port + FIFO);
  }
  if (dtc != 0) {
    printk ("oz_dev_floppy: %s skipped %d data bytes before results\n", devex -> ucb$ps_pb_idb -> ctrlname, dtc);
  }
  return (SS$_NORMAL);

volnotvalid:
  if (devex != NULL) devex -> volvalid = 0;
  return (OZ_VOLNOTVALID);

premature:
  printk ("oz_dev_floppy: %s premature termination of command %2.2X results after %d of %d bytes, msr %2.2X\n", 
                 devex -> ucb$ps_pb_idb -> ctrlname, command, i, size, msr);
  return (OZ_IOFAILED);

busyclear:
  printk ("oz_dev_floppy: %s command %2.2X busy bit did not stay set after %d of %d results bytes, msr %2.2X\n", 
                 devex -> ucb$ps_pb_idb -> ctrlname, command, i, size, msr);
  return (OZ_IOFAILED);
}

/************************************************************************/
/*									*/
/*  Wait for the command to be done					*/
/*									*/
/*    Input:								*/
/*									*/
/*	port = I/O port base (0x3F0 or 0x370)				*/
/*									*/
/*    Output:								*/
/*									*/
/*	command done = 0 : timed out					*/
/*	            else : success					*/
/*									*/
/************************************************************************/

static int command_done (unsigned short port, unsigned char command)

{
  unsigned long rc;

  rc = oz_hw_stl_microwait (50000, command_done_check, &port);	/* give it up to 50mS to complete */
  if (!rc) printk ("oz_dev_floppy command_done: %X command %2.2X timed out, msr %2.2X\n", port, command, oz_hw_inb (port + MSR));
  return (rc);
}

static unsigned long command_done_check (void *portv)

{
  unsigned char msr;

  msr = oz_hw_inb (*(unsigned short *)portv + MSR);
  return (!(msr & MSR_CB));
}

/************************************************************************/
/*									*/
/*  Wait for Master Status Register's RQM bit to be set			*/
/*									*/
/*    Input:								*/
/*									*/
/*	port = I/O port base number (0x3F0 or 0x370)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	wait_for_rqm = resultant msr contents				*/
/*		if MSR_RQM is set, it succeeded				*/
/*		otherwise, it timed out					*/
/*									*/
/************************************************************************/

typedef struct { unsigned short port;
                 unsigned char msr;
               } Rqmprm;

static unsigned char wait_for_rqm (unsigned short port)

{
  Rqmprm rqmprm;

  rqmprm.port = port;

  if (!oz_hw_stl_microwait (MSR_RQM_TIMEOUT, check_for_rqm, &rqmprm)) {
    printk ("oz_dev_floppy: timed out waiting for MSR_RQM on port %x (msr=%x)\n", port, rqmprm.msr);
  }

  return (rqmprm.msr);
}

static unsigned long check_for_rqm (void *rqmprmv)

{
  unsigned char msr;

  ((Rqmprm *)rqmprmv) -> msr = msr = oz_hw_inb (((Rqmprm *)rqmprmv) -> port + MSR);
  return (msr & MSR_RQM);
}

static unsigned char wait_for_rqmdio (unsigned short port)

{
  Rqmprm rqmprm;

  rqmprm.port = port;

  if (!oz_hw_stl_microwait (MSR_RQM_TIMEOUT, check_for_rqmdio, &rqmprm)) {
    printk ("oz_dev_floppy: timed out waiting for MSR_RQM and _DIO on port %x (msr=%x)\n", port, rqmprm.msr);
  }

  return (rqmprm.msr);
}

static unsigned long check_for_rqmdio (void *rqmprmv)

{
  unsigned char msr;

  ((Rqmprm *)rqmprmv) -> msr = msr = oz_hw_inb (((Rqmprm *)rqmprmv) -> port + MSR);
  return ((msr & (MSR_RQM | MSR_DIO)) == (MSR_RQM | MSR_DIO));
}

/************************************************************************/
/*									*/
/*  Print error status bits						*/
/*									*/
/*    Input:								*/
/*									*/
/*	rw      = "read" or "write"					*/
/*	devex   = device that had error					*/
/*	command = read/write command buffer				*/
/*	results = read/write results buffer				*/
/*	len     = length of attempted transfer				*/
/*									*/
/*    Output:								*/
/*									*/
/*	error message printed on console				*/
/*									*/
/************************************************************************/

static void print_rw_err (const char *rw, Devex *devex, unsigned char *command, unsigned char *results, unsigned long len)

{
  static const char *term[4] = { "normal", "abnormal", "invalid", "polling" };

  printk ("oz_dev_floppy: %s %s error len %u, st0 %x, st1 %x, st2 %x\n", devex -> unitname, rw, len, results[0], results[1], results[2]);
  printk ("oz_dev_floppy: - %s termination%s%s%s%s%s%s%s%s%s%s%s%s\n", 
	term[results[0]>>6], 
	results[0] & 0x10 ? ", recal fail" : "", 
	results[1] & 0x80 ? ", end of cyl" : "", 
	results[1] & 0x20 ? ", data error" : "", 
	results[1] & 0x10 ? ", overrun" : "", 
	results[1] & 0x04 ? ", sector not found" : "", 
	results[1] & 0x02 ? ", write protect" : "", 
	results[1] & 0x01 ? ", missing AM" : "", 
	results[2] & 0x40 ? ", deleted data" : "", 
	results[2] & 0x20 ? ", data CRC error" : "", 
	results[2] & 0x10 ? ", wrong cylinder" : "", 
	results[2] & 0x02 ? ", bad cylinder" : "", 
	results[2] & 0x01 ? ", missing DAM" : "");
  printk ("oz_dev_floppy: - track/drive %x, cylinder %x, track %x, sector %x..%x\n", 
                 command[1], command[2], command[3], command[4], command[6]);
}

/************************************************************************/
/*									*/
/*  Set up a DMA operation						*/
/*									*/
/*    Input:								*/
/*									*/
/*	dmadir = 8 : memory to device (floppy write)			*/
/*	         4 : device to memory (floppy read)			*/
/*	transferlen = transfer length					*/
/*	iopex -> phypages = NULL : force temp buffer			*/
/*	                    else : points to physical page number array	*/
/*	         byteoffs = 0 : must be when using phypages = NULL	*/
/*	                 else : offset in first physical page to start	*/
/*									*/
/*    Output:								*/
/*									*/
/*	dmapage_setup = 0 : transfer set up using dmapage_addr buffer	*/
/*	             else : direct access to iopex buffer, this many bytes
/*									*/
/************************************************************************/

static unsigned long dmapage_setup (int dmadir, unsigned long transferlen, Devex *devex, unsigned long byteoffs, OZ_Mempage *phypages, OZ_Dbn slbn)

{
  OZ_Mempage npage;
  OZ_Phyaddr phyaddr;
  unsigned char secpertrk, starttrack, stoptrack;
  unsigned long direct;

  if (transferlen > dmapage_size) panic ("oz_dev_floppy_486 dmapage_setup: transferlen %u, dmapage_size %u\n", transferlen, dmapage_size);

  /* Make sure byteoffs fits within a physical page */

  phypages += byteoffs >> OZ_HW_L2PAGESIZE;
  byteoffs &= (1 << OZ_HW_L2PAGESIZE) - 1;

  /* See if we can directly access the caller's buffer                                */
  /* - we can do as much as is physically contiguous                                  */
  /* - if we can't do all that was requested, we just do to end of starting track     */
  /*   (so we don't waste a rev of the disk after we got a couple blocks from the     */
  /*   second side, we would have to wait for a rev to get the rest)                  */
  /* - if we can't even do to end of track, use temp buffer                           */
  /* In any case, we can't DMA to phys address > 16M, and we can't cross 64K boundary */

  direct  = 0;									// assume we're beyond 16M limit
  if (phypages != NULL) {							// make sure caller isn't forcing temp buffer
    phyaddr = (phypages[0] << OZ_HW_L2PAGESIZE) + byteoffs;			// get starting physical address
    if (phyaddr < 0x1000000) {							// make sure we at least start within 16M limit
      direct = (1 << OZ_HW_L2PAGESIZE) - byteoffs;				// see how much is left of start page
      while (direct < transferlen) {						// repeat as long as there is more to do
        npage = (direct + byteoffs) >> OZ_HW_L2PAGESIZE;			// - calculate new page index
        if (phypages[npage] != phypages[0] + npage) break;			// - stop if it is not contiguous
        direct += (1 << OZ_HW_L2PAGESIZE);					// - ok, we can do one more page of bytes
      }
      if (direct > transferlen) direct = transferlen;				// don't do more than caller asked for
      if (direct + phyaddr > 0x1000000) direct = 0x1000000 - phyaddr;		// stop at 16M limit
      if (((direct + phyaddr - 1) ^ phyaddr) & -65536) direct = 65536 - (phyaddr & 65535); // don't cross 64K boundary
      if (direct < transferlen) {						// see if we're doing less than caller asked for
        secpertrk  = sectorspertrack[devex->sizecode];
        starttrack = slbn / secpertrk;						// can't do it all, get track starting on
        stoptrack  = (slbn + (direct / DISK_BLOCK_SIZE)) / secpertrk;		// get track stopping on
        if (starttrack == stoptrack) direct = 0;				// if can't do to end of track, use temp buffer
        else direct = (secpertrk - (slbn % secpertrk)) * DISK_BLOCK_SIZE;	// can do to end of track, so just do that much
      }
    }
  }

  if (direct != 0) transferlen = direct;	// direct, this is how much of it we're going to do
  else phyaddr = (OZ_Phyaddr)dmapage_addr;	// not direct, do all requested bytes using temp buffer

  if (transferlen == 0) panic ("oz_dev_floppy dmapage_setup: transferlen 0");

  /* Set up controller to transfer 'transferlen' bytes starting at physical address 'phyaddr' */

  transferlen --;				// controller requires length - 1
  oz_hw_outb (0x04, DMA_MASK);			// turn off channel 2
  oz_hw_outb (0, DMA_CBP);			// make sure it is ready for low byte
  oz_hw_outb (phyaddr, DMA_ADDR2);		// write bits <00:07> of the physical address
  oz_hw_outb (phyaddr >> 8, DMA_ADDR2);		// write bits <08:15> of the physical address
  oz_hw_outb (phyaddr >> 16, DMA_PAGE2);	// write bits <16:23> of the physical address
  oz_hw_outb (transferlen, DMA_COUNT2);		// write bits <00:07> of the byte count - 1
  oz_hw_outb (transferlen >> 8, DMA_COUNT2);	// write bits <08:15> of the byte count - 1
  oz_hw_outb (0, DMA_CTRL);			// select defaults for everything in here
  oz_hw_outb (dmadir + 2, DMA_MODE);		// set to demand transfer mode, address increment, 
						// no auto-init, direction, channel 2
  oz_hw_outb (0, DMA_ACT);			// activate all four DMA channels

  return (direct);
}

/************************************************************************/
/*									*/
/*  Interrupt routine							*/
/*									*/
/*  If the thread is waiting for an interrupt and the controller is 	*/
/*  ready, wake the kernel thread					*/
/*									*/
/*  This routines runs with the smplock_fd set				*/
/*									*/
/************************************************************************/

//static int fd_interrupt (void *dummy, OZ_Mchargs *mchargs)
static void floppy_interrupt(int irq, void *dev_id, struct pt_regs * regs)

{
  struct _idb *controller;

  for (controller = controllers; controller != NULL; controller = controller -> next) {
    if (controller -> waitingforint > 0) {				// see if the thread is waiting for an interrupt
      controller -> waitingforint = 0;					// if so, say it is no longer waiting
      oz_knl_event_set (controller -> event, 1);			// ... then wake it up
    }
  }

  return (0);
}
#endif
