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
/*  PCI IDE disk controller driver using SFF 8038i DMA standard		*/
/*  Will also process generic IDE PIO interface				*/
/*									*/
/*  Creates devices named ide_CD					*/
/*									*/
/*	C = controller: p for primary, s for secondary			*/
/*	D = drive: m for master, s for slave				*/
/*									*/
/*  For ATA devices, the created device is a raw disk drive.		*/
/*									*/
/*  For ATAPI devices, the created device appears as a scsi controller	*/
/*  either processing only scsi-id 0 (master) or 1 (slave).		*/
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
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#if 0
#include "ozone.h"

#include "oz_dev_disk.h"
#include "oz_dev_pci.h"
#include "oz_dev_scsi.h"
#include "oz_dev_timer.h"
#include "oz_io_disk.h"
#include "oz_io_scsi.h"
#include "oz_knl_devio.h"
#include "oz_knl_hw.h"
#include "oz_knl_kmalloc.h"
#include "oz_knl_procmode.h"
#include "oz_knl_sdata.h"
#include "oz_knl_section.h"
#include "oz_knl_spte.h"
#include "oz_knl_status.h"
#include "oz_sys_dateconv.h"
#include "oz_sys_xprintf.h"
#endif

#define DISK_BLOCK_SIZE (512)
#define BUFFER_ALIGNMENT (3)
#define L2DISK_BLOCK_SIZE (9)
#define MAX_RW_RETRIES (8)
#define ATAPI_CMDMAX (16)
#define ATA_TIMEOUT 1		/* timeout (in seconds) - don't put parentheses on this */

#define MAX_SEC_COUNT (256)	/* max sectors that the controller can transfer at a time */

#define DMATBLMAX (((MAX_SEC_COUNT*DISK_BLOCK_SIZE)>>PAGE_SHIFT)+1)	// max number of descriptors allowed for a DMA transfer
#define DMATBLSIZ (DMATBLMAX * sizeof (Dmatbl))					// number of bytes required for the DMA table

#define FORTYEIGHTBIT(devex) ((devex) -> totalblocks > 0x0FFFFFFF)		// non-zero if 48-bit mode required for this drive
#define LBAMODE(devex) ((devex) -> ident[ATA_IDENT_W_CAPABILITIES] & 0x200)	// non-zero if drive cabable of LBA addressing

/* "IDENTIFY DRIVE" command return buffer (just stuff that we use) */
/* Note: these are word indicies, not byte offsets                 */

#define ATA_IDENT_W_DEFCYLINDERS   (1)
#define ATA_IDENT_W_DEFTRACKS      (3)
#define ATA_IDENT_W_DEFSECTORS     (6)
#define ATA_IDENT_W20_MODEL       (27)
#define ATA_IDENT_W_RWMULTIPLE    (47)
#define ATA_IDENT_W_CAPABILITIES  (49)
#define ATA_IDENT_W_CURCYLINDERS  (54) /* only use for non-lba mode, ie, IDENT_W2_TOTALBLOCKS is zero */
#define ATA_IDENT_W_CURTRACKS     (55)
#define ATA_IDENT_W_CURSECTORS    (56)
#define ATA_IDENT_W2_TOTALBLOCKS  (60) /* only valid if drive supports LBA, zero if it doesn't */
#define ATA_IDENT_W_MULTIWORDMA   (63)
#define ATA_IDENT_W_PIOMODES      (64)
#define ATA_IDENT_W_MWORDMACYCLE  (66)
#define ATA_IDENT_W_COMMANDSETS1  (83)
#define ATA_IDENT_W_UDMAMODES     (88)
#define ATA_IDENT_W4_TOTALBLOCKS (100)
#define ATA_IDENT__SIZE          (128)

#define ATAPI_IDENT_W20_MODEL    (27)

#define OZ_DEVUNIT_NAMESIZE (64)     /* max length of device unit name string (incl null) */

#define OZ_SMPLOCK_SIZE 2

#define OZ_DEVUNIT_DESCSIZE (64)

#define OZ_TIMER_RESOLUTION 10000000

/* Struct defs */

typedef struct Chnex Chnex;
typedef struct Ctrlr Ctrlr;
typedef struct Devex Devex;
typedef struct Iopex Iopex;

typedef struct { int index;
  unsigned long pcibus, pcidev, pcifunc;
} OZ_Dev_pci_conf_p;

typedef struct { unsigned char opaque[OZ_SMPLOCK_SIZE]; } OZ_Smplock;

typedef void OZ_Timer;

typedef struct { unsigned char opaque[4]; } OZ_Pagentry;

typedef void OZ_Mchargs;

struct OZ_Hw486_irq_many { 
  //OZ_Hw486_irq_many *next;                             
  // next in list (for use by oz_hw486_irq_many_add/_rem only)
  int (*entry) (void *param, OZ_Mchargs *mchargs);     
  // entrypoint of routine to call
  void *param;                                         
  // parameter to pass to routine
  const char *descr;                                   
  // description of handler
  OZ_Smplock smplock;                                  
  // smplock for the interrupt
};

typedef struct { unsigned long size;                    /* size (in bytes) to write (mul
						   tiple of block size) */
  const void *buff;              /* buffer address */
  unsigned long slbn;                   /* starting logical block number
				  */
  int inhretries;                /* inhibit retries */
  int writethru;                 /* write through to magnetic med
				    ia (bypassing drive's internal cache) */
} OZ_IO_disk_writeblocks;

/* Read blocks */

typedef struct { unsigned long size;                    /* size (in bytes) to read (mult
						   iple of block size) */
  void *buff;                    /* buffer address */
  unsigned long slbn;                   /* starting logical block number
				  */
  int inhretries;                /* inhibit retries */
} OZ_IO_disk_readblocks;


typedef struct OZ_Hw486_irq_many OZ_Hw486_irq_many;

typedef unsigned long OZ_Procmode;

typedef struct { unsigned long cmdlen;          /* command length */
  const unsigned char *cmdbuf;   /* command buffer */
  unsigned long datasize;        /* data buffer size */
  const unsigned long *dataphypages; /* pointer to array of physical
				     page numbers */
  unsigned long databyteoffs;    /* byte offset in the first physical pag
			    e to start at */
  unsigned long optflags;        /* option flags */
  unsigned char *cmpflags;       /* where to return completion flags */
  unsigned char *status;         /* where to return scsi status byte */
  unsigned long *datarlen;       /* where to return data length transferr
			    ed */
  unsigned long timeout;         /* timeout (0=none, else milliseconds) */
               } OZ_IO_scsi_doiopp;

#define OZ_HW_DATEBIN_ADD(__s,__a,__b) (__s) = (__a) + (__b)
#define OZ_HW_DATEBIN_CMP(__s,__m) ((__s) < (__m) ? -1 : ((__s) > (__m)))

typedef struct { unsigned short atacmd;
                 unsigned short atactl;
                 unsigned short bmiba;
                 int irqlevel;
                 int cablesel;
                 const char *suffix;
                 int chipidx;
                 OZ_Dev_pci_conf_p *pciconfp;

                 char unitname[OZ_DEVUNIT_NAMESIZE];
                 const char *cable;
                 Ctrlr *ctrlr;
                 Devex *devexs[2];
                 unsigned short seccount[2], tracount[2];
                 unsigned short secbuf[256];
               } Gibp;

/* Channel extension area (ATAPI channels only) */

struct Chnex { char drive_id;			/* -1: not open, 0: master, 1: slave */
             };

/* Controller data (one per cable) */

typedef struct { unsigned long physaddr;
                 unsigned long bytecount;
               } Dmatbl;

struct Ctrlr { OZ_Smplock *smplock;		/* pointer to irq level smp lock */
               int requestcount;		/* number of iopex's queued but not yet iodone'd (master only) */
               Iopex *iopex_qh;			/* requests waiting to be processed, NULL if none */
               Iopex **iopex_qt;
               Iopex *iopex_ip;			/* request currently being processed, NULL if none */
               OZ_Timer *timer;			/* interrupt timeout timer */
               int timerqueued;			/* set to 1 when timer request queued, set to 0 when timer request dequeued */
               unsigned long long timerwhen;		/* set to date/time when we want the actual timeout */
               unsigned short atacmd;			/* I/O address of ata command registers */
               unsigned short atactl;			/* I/O address of ata control registers */

               Dmatbl *dmatblva;		/* dma table virtual address (DMATBLMAX entries, doesn't cross 64K boundary) */
               unsigned long dmatblpa;		/* dma table physical address */
               unsigned short bmiba;			/* Bus Master Interface Base Address */
						/* or ZERO for generic PIO-style interface */
               OZ_Hw486_irq_many irq_many;	/* interrupt block */
               Dmatbl dmatblbf[DMATBLMAX*2+1];	/* dma table - but use dmatblva to quad align and in case it crosses 64K boundary */
             };

/* Device extension structure */

struct Devex { struct _ucb *devunit;		/* devunit pointer */
               const char *name;		/* device unit name (for messages) */
               Ctrlr *ctrlr;			/* pointer to controller struct */

			/* atapi device only (atapimsk != 0) */

               unsigned long atapimsk;			/* <0>: master is ATAPI; <1>: slave is ATAPI */
               unsigned long atapiopn;			/* which ATAPI device(s) are open */
               unsigned short atapiidentword0;		/* atapi ident word 0 */

			/* ata devices only (atapimsk == 0) */

               unsigned long secpertrk;			/* number of sectors in a track */
               unsigned long trkpercyl;			/* number of tracks per cylinder (number of heads) */
               unsigned long secpercyl;			/* number of sectors in a cylinder */
               unsigned long totalblocks;		/* total number of sectors on the drive */
               unsigned short ident[ATA_IDENT__SIZE];	/* 'Identify Drive' command results */
               unsigned char atacmd_read;		/* read/write command code bytes */
               unsigned char atacmd_read_inh;
               unsigned char atacmd_write;
               unsigned char multsize;			/* multiple sector transfer size (or 0 if disabled) */
               char drive_id;			/* 0: this is a master drive, 1: this is a slave drive */
             };

/* I/O operation extension structure */

struct Iopex { Iopex *next;			/* next in ctrlr -> iopex_qh/qt */
               struct _irp *ioop;			/* pointer to io operation node */
               OZ_Procmode procmode;		/* requestor's processor mode */
               Devex *devex;			/* device extension data pointer */
               int writedisk;			/* 0: data transfer from device to memory; 1: data transfer from memory to device */
               unsigned long status;			/* completion status */
               const unsigned long *phypages;	/* physical page array pointer */
               unsigned long byteoffs;			/* starting physical page byte offset */
               unsigned long amount_done;		/* amount already written to / read from disk within the whole request */
               unsigned long amount_xfrd;		/* amount transferred within this disk command */
               unsigned long amount_to_xfer;		/* total amount to transfer within this disk command */

			/* ATA style commands only */

               unsigned char atacmdcode;		/* ATA command code */
               unsigned long size;			/* buffer size */
               unsigned long seccount;			/* sector count for transfer */
               unsigned long slbn;			/* starting logical block number */
               int timedout;			/* set if request timed out somewhere */
               int retries;			/* retry counter */

			/* ATAPI style commands only */

               OZ_IO_scsi_doiopp doiopp;	/* scsi parameter block */
               unsigned long dmatblmx;			/* size of 'dmatblva' table */
               Dmatbl *dmatblva;		/* dma table virtual address (NULL to use controller's dmatbl) */
               unsigned long dmatblpa;		/* dma table physical address */
               unsigned char atapicmd[ATAPI_CMDMAX];	/* scsi command buffer */
               char drive_id;			/* drive number (0 or 1) */
               unsigned char atapists;			/* scsi status byte (0=success, 2=error) */
             };

/* Function tables - one for ATA devices, one for ATAPI devices */

static int shutdown (struct _ucb *devunit, void *devexv);
#if 0
static unsigned long ata_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode, 
                        struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap);
#endif
#if 0
static const OZ_Devfunc ata_functable = { sizeof (Devex), 0, sizeof (Iopex), 0, shutdown, NULL, NULL, 
                                          NULL, NULL, NULL, ata_start, NULL };
#endif
static unsigned long atapi_assign (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode);
static int atapi_deassign (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv);
static unsigned long atapi_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode, 
                          struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap);
#if 0
static const OZ_Devfunc atapi_functable = { sizeof (Devex), sizeof (Chnex), sizeof (Iopex), 0, shutdown, NULL, NULL, 
                                            atapi_assign, atapi_deassign, NULL, atapi_start, NULL };
#endif

/* Internal static data */

static int initialized = 0;
#if 0
static OZ_Devclass  *devclass_disk,  *devclass_scsi;
static OZ_Devdriver *devdriver_disk, *devdriver_scsi;
#endif

static Devex *crash_devex = NULL;
static int crash_inprog = 0;
static Iopex crash_iopex;
static struct _ucb *crash_devunit = NULL;

/* A block of zeroes to use when writing less than a full block */
/* (Reading partial blocks goes to phypage zero)                */
/* Hopefully using unsigned long will longword align it                 */

static const unsigned long zeroes[DISK_BLOCK_SIZE/4];

/* Internal routines */

static int config (unsigned short atacmd, unsigned short atactl, unsigned short bmiba, int irqlevel, int cablesel, const char *suffix, int chipidx, OZ_Dev_pci_conf_p *pciconfp);
static int probe_atadrive (Gibp *gibp, int drive_id);
static void init_atadrive (Gibp *gibp, int drive_id);
static int probe_atapidrv (Gibp *gibp, int drive_id);
static void init_atapidrv (Gibp *gibp, int drive_id);
static void setup_piix4 (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *master, Devex *slave);
static void setup_piix4_drive (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *devex);
static void setup_amd768 (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *master, Devex *slave);
static void setup_amd768_drive (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *devex);
static unsigned long waitfornotbusy (void *atactl_p);
static unsigned long getidentbuff (void *gibpv);
static Ctrlr *ctrlr_create (Gibp *gibp);
static int shutdown (struct _ucb *devunit, void *devexv);
static unsigned long ata_crash (void *dummy, unsigned long lbn, unsigned long size, unsigned long phypage, unsigned long offset);
static unsigned long ata_queuereq (unsigned long size, unsigned long slbn, const unsigned long *phypages, unsigned long byteoffs, Iopex *iopex);
static unsigned long atapi_queuereq (Iopex *iopex);
static void queuereq (Iopex *iopex);
static void startreq (Ctrlr *ctrlr);
static void startdma (Iopex *iopex, unsigned long xfersize, unsigned long blocksize);
static void recalibrate (Devex *devex);
static unsigned long checkrecaldone (void *ctrlrv);
static void reqtimedout (void *ctrlrv, OZ_Timer *timer);
static int dma_intserv (void *ctrlrv, OZ_Mchargs *mchargs);
static int pio_intserv (void *ctrlrv, OZ_Mchargs *mchargs);
static void pio_ata_intserv (Iopex *iopex, unsigned char status);
static void pio_atapi_intserv (Iopex *iopex, unsigned char status);
static int atapi_command_packet (Iopex *iopex, unsigned char status);
static void do_pio_transfer (int nbytes, Iopex *iopex);
static void ata_finish (Iopex *iopex, unsigned char status);
static void atapi_finish (Iopex *iopex, unsigned char status);
static void reqdone (Iopex *iopex);
static void atapi_reqdone (void *iopexv, int finok, unsigned long *status_r);
static void validaterequestcount (Ctrlr *ctrlr, int line);

/************************************************************************/
/*									*/
/*  I/O space registers							*/
/*									*/
/************************************************************************/

	/* These are biased by Ctrlr field atacmd */

#define ATACMD_W_DATA      (0)	/* RW: data */
#define ATACMD_BR_ERROR    (1)	/* RO: error */
				/*     <0> :  AMNF - address mark not found */
				/*     <1> : TK0NF - track zero not found (recal command) */
				/*     <2> :  ABRT - command aborted due to drive status error */
				/*     <3> :   MCR - media change requested */
				/*     <4> :  IDNF - requested sector's ID field cont not be found */
				/*     <5> :    MC - media changed */
				/*     <6> :   UNC - uncorrectable data error */
				/*     <7> :   BBK - bad block detected */
#define ATACMD_BW_FEATURES (1)	/* WO: features */
#define ATACMD_B_SECCOUNT  (2)	/* RW: sector count */
#define ATACMD_B_LBALOW    (3)	/* RW: sector number / LBA <00:07> */
#define ATACMD_B_SECNUM    (3)
#define ATACMD_B_LBAMID    (4)	/* RW: cylno <00:07> / LBA <08:15> */
#define ATACMD_B_CYL_LO    (4)
#define ATACMD_B_LBAHIGH   (5)	/* RW: cylno <08:15> / LBA <16:23> */
#define ATACMD_B_CYL_HI    (5)
#define ATACMD_B_DRHEAD    (6)	/* RW: */
				/*     <0:3> : head select */
				/*       <4> : drive select */
				/*       <5> : 1 */
				/*       <6> : 0 = CHS mode */
				/*             1 = LBA mode */
				/*       <7> : 1 */
#define ATACMD_BR_STATUS   (7)	/* RO: status */
				/*     <0> :  ERR - an error occurred during the command */
				/*     <1> :  IDX - set once per revolution */
				/*     <2> : CORR - correctable data error occurred */
				/*     <3> :  DRQ - ready to transfer a word of data */
				/*     <4> :  DSC - drive seek complete */
				/*     <5> :  DWF - drive write fault */
				/*     <6> : DRDY - drive ready - able to respond to a command */
				/*     <7> :  BSY - controller busy */
#define ATACMD_BW_COMMAND (7)	/* WO: command */

	/* These are biased by Ctrlr field atactl */

#define ATACTL_BR_ALTSTS (2)
#define ATACTL_BW_DEVCTL (2)
				/*     <0> : must be zero */
				/*     <1> : 0: interrupts enabled; 1: interrupts disabled */
				/*     <2> : software reset when set */
				/*     <3> : must be one */
				/*     <4> : must be zero */
				/*     <5> : must be zero */
				/*     <6> : must be zero */
				/*     <7> : must be zero */

	/* These are biased by Ctrlr field bmiba */

#define BMIBA_B_BMICX   (0)	/* bus master ide command register */
#define BMIBA_B_BMISX   (2)	/* bus master ide status register */
#define BMIBA_L_BMIDTPX (4)	/* bus master ide descriptor table pointer register */

/************************************************************************/
/*									*/
/*  Boot-time initialization routine					*/
/*									*/
/************************************************************************/

static const struct { unsigned long didvid;
                      int pcifunc;
                      const char *name;
                      void (*setup) (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *master, Devex *slave);
                    } chips[] = { 0x71118086, 1, "Intel PIIX4", setup_piix4, 
                                  0x74411022, 1, "AMD 768",     setup_amd768, 
                                           0, 0, NULL };

void oz_dev_ide8038i_init ()

{
  char suffix[16];
  int defpriused, defsecused, index, init, priirqnm, secirqnm, started;
  OZ_Dev_pci_conf_p pciconfp;
  unsigned long bmiba, pricmdba, prictlba, progintf, seccmdba, secctlba;

  if (initialized) return;

  printk ("oz_dev_ide8038i_init\n");
  initialized = 1;
#if 0
  devclass_disk  = oz_knl_devclass_create (OZ_IO_DISK_CLASSNAME, OZ_IO_DISK_BASE, OZ_IO_DISK_MASK, "ide8038i_486_ata");
  devdriver_disk = oz_knl_devdriver_create (devclass_disk, "ide8038i_486_ata");
  devclass_scsi  = oz_knl_devclass_create (OZ_IO_SCSI_CLASSNAME, OZ_IO_SCSI_BASE, OZ_IO_SCSI_MASK, "ide8038i_486_atapi");
  devdriver_scsi = oz_knl_devdriver_create (devclass_scsi, "ide8038i_486_atapi");
#endif

  /* Scan for PCI IDE controller chips */

  defpriused = 0;
  defsecused = 0;

#if 0
  for (index = 0; chips[index].didvid != 0; index ++) {
    for (init = 1; oz_dev_pci_conf_scan_didvid (&pciconfp, init, chips[index].didvid); init = 0) {
      if (pciconfp.pcifunc != chips[index].pcifunc) continue;			// only bother checking this function
      printk ("oz_dev_ide8038i: found %s ide controller at bus/dev/func %u/%u/%u\n", 
	chips[index].name, pciconfp.pcibus, pciconfp.pcidev, pciconfp.pcifunc);

      /* Get primary/secondary command/control register base addresses */

      pricmdba  = 0x01F0;							// set up default port numbers
      prictlba  = 0x03F4;
      priirqnm  = 14;
      seccmdba  = 0x0170;
      secctlba  = 0x0374;
      secirqnm  = 15;
      suffix[0] = 0;
      progintf  = oz_dev_pci_conf_inl (&pciconfp, 0x08);
      if (progintf & 0x100) {							// see if there are explicit primary ports
										// - piix4 is hardwired to zero
										// - amd768 is programmable
        pricmdba = oz_dev_pci_conf_inl (&pciconfp, 0x10);			// if so, read them
        prictlba = oz_dev_pci_conf_inl (&pciconfp, 0x14);
        if (((pricmdba & 0xFFFF0003) != 1) || ((prictlba & 0xFFFF0003) != 1)) {	// they must be in I/O space
          printk ("oz_dev_ide8038i: bad primary reg bus address %X/%X\n", pricmdba, prictlba);
          continue;
        }
        pricmdba --;								// clear the I/O space bit
        prictlba --;
        sprintf (suffix, "_%u_%u", pciconfp.pcibus, pciconfp.pcidev);
      } else {
        if (defpriused) {
          printk ("oz_dev_ide8038i: default primary already defined\n");
          continue;
        }
        defpriused = 1;
      }
      if (progintf & 0x400) {							// see if there are explicit secondary ports
										// - piix4 is hardwired to zero
										// - amd768 is programmable
        seccmdba = oz_dev_pci_conf_inl (&pciconfp, 0x18);			// if so, read them
        secctlba = oz_dev_pci_conf_inl (&pciconfp, 0x1C);
        if (((seccmdba & 0xFFFF0003) != 1) || ((secctlba & 0xFFFF0003) != 1)) {	// they must be in I/O space
          printk ("oz_dev_ide8038i: bad secondary reg bus address %X/%X\n", seccmdba, secctlba);
          continue;
        }
        seccmdba --;								// clear the I/O space bit
        secctlba --;
        sprintf (suffix, "_%u_%u", pciconfp.pcibus, pciconfp.pcidev);
      } else {
        if (defsecused) {
          printk ("oz_dev_ide8038i: default secondary already defined\n");
          continue;
        }
        defsecused = 1;
      }

      bmiba = oz_dev_pci_conf_inl (&pciconfp, 0x20);				// get dma controller base I/O address
										// - same on piix4 and amd768
      if ((bmiba & 0xFFFF000F) != 1) {						// verify the bits we assume to be as indicated really are
										// bmiba<00> = 1 : means the bmiba stuff is in I/O space
        printk ("oz_dev_ide8038i: bad bus master reg bus address %X\n", bmiba);
        continue;
      }

      started  = config (pricmdba, prictlba, bmiba - 1, priirqnm, 0, suffix, index, &pciconfp);
      started |= config (seccmdba, secctlba, bmiba + 7, secirqnm, 1, suffix, index, &pciconfp);
      if (started) {
        oz_dev_pci_conf_outw (0x0005, &pciconfp, PCI_COMMAND);	// set bus master enable to enable dma
										// set I/O space enable to access dma registers
        oz_dev_pci_conf_outw (0x3800, &pciconfp, PCI_STATUS);	// reset error status bits
      }
    }
  }
#endif

  /* If defaults not found, try generic PIO-style controller */

  if (!defpriused) config (0x01F0, 0x03F4, 0, 14, 0, "", -1, NULL);		// try config primary controller
  if (!defsecused) config (0x0170, 0x0374, 0, 15, 1, "", -1, NULL);		// try config secondary controller
}

/************************************************************************/
/*									*/
/*  Try to configure drives connected to a particular cable		*/
/*									*/
/*    Input:								*/
/*									*/
/*	atacmd   = command register base address			*/
/*	atactl   = control register base address			*/
/*	bmiba    = 0 : non-DMA controller				*/
/*	        else : dma register base address			*/
/*	irqlevel = irq level number					*/
/*	cablesel = 0 : primary cable					*/
/*	           1 : secondary cable					*/
/*	suffix   = device name suffix or "" for standard device names	*/
/*	chipidx  = index in 'chips[]' array for this controller		*/
/*	pciconfp = NULL : generic non-DMA controller			*/
/*	           else : controller's PCI config space			*/
/*									*/
/************************************************************************/

static int config (unsigned short atacmd, unsigned short atactl, unsigned short bmiba, int irqlevel, int cablesel, const char *suffix, int chipidx, OZ_Dev_pci_conf_p *pciconfp)

{
  Gibp *gibp;
  int drive_id;

  gibp = kmalloc (sizeof *gibp, GFP_KERNEL);
  memset (gibp, 0, sizeof *gibp);
  gibp -> atacmd   = atacmd;
  gibp -> atactl   = atactl;
  gibp -> bmiba    = bmiba;
  gibp -> irqlevel = irqlevel;
  gibp -> cablesel = cablesel;
  gibp -> suffix   = suffix;
  gibp -> chipidx  = chipidx;
  gibp -> pciconfp = pciconfp;

  gibp -> cable    = "primary";
  if (cablesel) gibp -> cable = "secondary";

  printk ("oz_dev_ide8038i: probing %s drives (%X/%X/%X)\n", gibp -> cable, atacmd, atactl, bmiba);

  /* Probe master (drive_id 0) then slave (drive_id 1) drive */

  for (drive_id = 0; drive_id < 2; drive_id ++) {

    /* Make up unit name string -                */
    /* unitname = ide_<cable><drive><suffix>     */
    /*    cable = p for primary, s for secondary */
    /*    drive = m for master, s for slave      */
    /*   suffix = _bus_dev if non-standard       */

    strcpy (gibp -> unitname, "ide_xx");
    gibp -> unitname[4] = gibp -> cable[0];
    gibp -> unitname[5] = drive_id ? 's' : 'm';
    strcat (gibp -> unitname, suffix);

    printk ("oz_dev_ide8038i: identifying %s %s drive\n", gibp -> cable, drive_id ? "slave" : "master");

    if (!probe_atadrive (gibp, drive_id)) probe_atapidrv (gibp, drive_id);
  }

  /* If we found any drives, enable them */

  drive_id = 0;									// assume we didn't find anything
  outb_p (0x0E, atactl + ATACTL_BW_DEVCTL);					// software reset the drives, disable interrupts
  // if (gibp -> ctrlr != NULL) {
  //  oz_hw_stl_nanowait (50000000);						// wait 50mS
    outb_p (0x08, atactl + ATACTL_BW_DEVCTL);				// release the drives, enable interrupts
    // oz_hw_stl_nanowait (50000000);						// wait 50mS

    /* Set up data transfer mode and other controller stuff */

    if (chipidx >= 0) {
      (*(chips[chipidx].setup)) (pciconfp, cablesel, gibp -> ctrlr, gibp -> devexs[0], gibp -> devexs[1]);
    }

    /* Enable the drives */

    for (drive_id = 0; drive_id < 2; drive_id ++) {
      if (gibp -> devexs[drive_id] == NULL) continue;				// see if we found anything in the scan
      outb_p ((drive_id << 4) | 0xA0, atacmd + ATACMD_B_DRHEAD);		// select the drive to be initialized
      // oz_hw_stl_nanowait (400);
      if (gibp -> devexs[drive_id] -> atapimsk == 0) init_atadrive (gibp, drive_id);
                                                else init_atapidrv (gibp, drive_id);
    }
  }

  kfree(gibp);							// free temp param buffer

  return (drive_id != 0);							// return whether or not we found something
}

/************************************************************************/
/*									*/
/*  Probe for an ATA drive						*/
/*									*/
/************************************************************************/

static int probe_atadrive (Gibp *gibp, int drive_id)

{
  char *p, unitdesc[OZ_DEVUNIT_DESCSIZE];
  Devex *devex;
  int i;
  struct _ucb *devunit;
  unsigned char status;
  unsigned long sts;

  /* Reset both drives on cable so they can't be screwed up by previous probing */

  outb_p (0x0E, gibp -> atactl + ATACTL_BW_DEVCTL);				// software reset the drives
// oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb_p (0x0A, gibp -> atactl + ATACTL_BW_DEVCTL);				// release the drives
//oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb_p ((drive_id << 4) | 0xA0, gibp -> atacmd + ATACMD_B_DRHEAD);	// select the drive to be identified
//oz_hw_stl_nanowait (400);

  /* Recal to see if there is an ATA drive there */

  outb_p (0x10, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to recalibrate
//oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
  sts = oz_hw_stl_microwait (50000, waitfornotbusy, &(gibp -> atactl));		// give it 50mS to finish
  if (sts == 0) {
    printk ("oz_dev_ide8038i: - timed out recalibrating\n");
    return (0);
  }
  status = sts;									// check for error
  if ((status & 0xD9) != 0x50) {
    printk ("oz_dev_ide8038i: - recalibrate status 0x%X\n", status);
    if (status & 0x01) {
      status = inb (gibp -> atacmd + ATACMD_BR_ERROR);
      printk ("oz_dev_ide8038i: - error code 0x%X\n", status);
    }
    return (0);
  }

  /* Now find out about the drive */

  outb_p (0xEC, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
//oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
  memset (gibp -> secbuf, 0, sizeof gibp -> secbuf);				// zero fill secbuf
  sts = oz_hw_stl_microwait (50000, getidentbuff, gibp);			// give it 50mS to finish identifying itself
  if (sts == 0) {
    printk ("oz_dev_ide8038i: - timed out reading identification\n");
    return (0);
  }
  status = sts;									// check for error
  if ((status & 0xD9) != 0x50) {
    printk ("oz_dev_ide8038i: - identify status 0x%X\n", status);
    if (status & 0x01) {
      status = inb (gibp -> atacmd + ATACMD_BR_ERROR);
      printk ("oz_dev_ide8038i: - error code 0x%X\n", status);
    }
    return (0);
  }

  /* Fix up the model name string */

  for (i = ATA_IDENT_W20_MODEL; i < ATA_IDENT_W20_MODEL + 20; i ++) {
    gibp -> secbuf[i] = (gibp -> secbuf[i] << 8) | (gibp -> secbuf[i] >> 8);
  }
  for (p = (char *)(gibp -> secbuf + ATA_IDENT_W20_MODEL + 20); *(-- p) == ' ';) {}
  *(++ p) = 0;
  printk ("oz_dev_ide8038i: - model %s\n", gibp -> secbuf + ATA_IDENT_W20_MODEL);

  /* If DMA controller, insist on a DMA drive (this is the 21st century) */

  if ((gibp -> bmiba != 0) && !(gibp -> secbuf[ATA_IDENT_W_CAPABILITIES] & 0x0100)) {
    printk ("oz_dev_ide8038i: - not capable of DMA\n");
    return (1);
  }

  /* Get and validate drive init params */

  gibp -> seccount[drive_id] = gibp -> secbuf[ATA_IDENT_W_DEFSECTORS];
  if ((gibp -> seccount[drive_id] == 0) || (gibp -> seccount[drive_id] > 256)) {
    printk ("oz_dev_ide8038i: - can't init drive params because default sector count is %u\n", gibp -> seccount[drive_id]);
    return (1);
  }
  gibp -> tracount[drive_id] = gibp -> secbuf[ATA_IDENT_W_DEFTRACKS];
  if ((gibp -> tracount[drive_id] == 0) || (gibp -> tracount[drive_id] > 16)) {
    printk ("oz_dev_ide8038i: - can't init drive params because default track count is %u\n", gibp -> tracount[drive_id]);
    return (1);
  }

  /* Unit description is the drive model */

  strncpy (unitdesc, (char *)(gibp -> secbuf + ATA_IDENT_W20_MODEL), sizeof unitdesc);

  /* Create the device unit struct */

#if 0
  devunit = oz_knl_devunit_create (devdriver_disk, gibp -> unitname, unitdesc, &ata_functable, 0, oz_s_secattr_sysdev);
  if (devunit == NULL) return (1);
#endif

  /* Fill in the device unit extension info */

  gibp -> devexs[drive_id] = devex = oz_knl_devunit_ex (devunit);
  memset (devex, 0, sizeof *devex);
  if (gibp -> ctrlr == NULL) gibp -> ctrlr = ctrlr_create (gibp);
  devex -> devunit     = devunit;
  devex -> name        = oz_knl_devunit_devname (devunit);
  devex -> ctrlr       = gibp -> ctrlr;
  memcpy (devex -> ident, gibp -> secbuf, sizeof devex -> ident);
  devex -> secpertrk   = devex -> ident[ATA_IDENT_W_CURSECTORS];
  devex -> trkpercyl   = devex -> ident[ATA_IDENT_W_CURTRACKS];
  devex -> secpercyl   = (unsigned long)(devex -> ident[ATA_IDENT_W_CURSECTORS]) * (unsigned long)(devex -> ident[ATA_IDENT_W_CURTRACKS]);
  devex -> totalblocks = (unsigned long)(devex -> ident[ATA_IDENT_W_CURCYLINDERS]) * devex -> secpercyl;;
  if (LBAMODE (devex)) {
    devex -> totalblocks = devex -> ident[ATA_IDENT_W2_TOTALBLOCKS+0] + (devex -> ident[ATA_IDENT_W2_TOTALBLOCKS+1] << 16);
    if ((devex -> ident[ATA_IDENT_W_COMMANDSETS1] & 0xC400) == 0x4400) {
      devex -> totalblocks = 0xFFFFFFFF;
      if ((devex -> ident[ATA_IDENT_W4_TOTALBLOCKS+2] == 0) && (devex -> ident[ATA_IDENT_W4_TOTALBLOCKS+3] == 0)) {
        devex -> totalblocks = devex -> ident[ATA_IDENT_W4_TOTALBLOCKS+0] + (devex -> ident[ATA_IDENT_W4_TOTALBLOCKS+1] << 16);
      }
    }
  }
  printk ("oz_dev_ide8038i: - sec/trk %u, trk/cyl %u, totalblocks %u\n", devex -> secpertrk, devex -> trkpercyl, devex -> totalblocks);
  return (1);
}

/************************************************************************/
/*									*/
/*  Initialize ATA drive						*/
/*									*/
/************************************************************************/

static void init_atadrive (Gibp *gibp, int drive_id)

{
  Devex *devex;
  int i;
  unsigned char status;
  unsigned long sts;

  devex = gibp -> devexs[drive_id];

  outb ((drive_id << 4) | 0xA0 | (gibp -> tracount[drive_id] - 1), gibp -> atacmd + ATACMD_B_DRHEAD); // set number of heads
  outb (gibp -> seccount[drive_id], gibp -> atacmd + ATACMD_B_SECCOUNT);		// set number of sectors
  outb_p (0x91, gibp -> atacmd + ATACMD_BW_COMMAND);				// initialize drive parameters
  //oz_hw_stl_nanowait (400);								// give the drive 400nS to start processing the command
  oz_hw_stl_microwait (50000, waitfornotbusy, &(gibp -> atactl));			// give it 50mS to finish
  status = inb (gibp -> atacmd + ATACMD_BR_STATUS);
  if ((status & 0xD9) != 0x50) {							// check for error
    printk ("oz_dev_ide8038i: - initialize drive parameters %X %X status 0x%X\n", (drive_id << 4) | 0xA0 | (gibp -> tracount[drive_id] - 1), gibp -> seccount[drive_id], status);
    if (status & 0x01) {
      status = inb (gibp -> atacmd + ATACMD_BR_ERROR);
      printk ("oz_dev_ide8038i: - error code 0x%X\n", status);
    }
    return;
  }

  /* Maybe it is DMA */

  if (gibp -> bmiba != 0) {
    devex -> atacmd_read     = 0xC8;
    devex -> atacmd_read_inh = 0xC9;
    devex -> atacmd_write    = 0xCA;
    if (FORTYEIGHTBIT (devex)) {
      devex -> atacmd_read     = 0x25;
      devex -> atacmd_read_inh = 0x25;
      devex -> atacmd_write    = 0x35;
    }
  }

  /* Else, try to set up multiple sector count = one page (for cache transfers) */

  else {
    devex -> multsize        = 0;
    devex -> atacmd_read     = 0x20;
    devex -> atacmd_read_inh = 0x21;
    devex -> atacmd_write    = 0x30;
    if (FORTYEIGHTBIT (devex)) {
      devex -> atacmd_read     = 0x24;
      devex -> atacmd_read_inh = 0x24;
      devex -> atacmd_write    = 0x34;
    }
    i = (1 << PAGE_SHIFT) / DISK_BLOCK_SIZE;
    if ((devex -> ident[ATA_IDENT_W_RWMULTIPLE] & 0xFF) < i) i = devex -> ident[ATA_IDENT_W_RWMULTIPLE] & 0xFF;
    if (i > 1) {
      outb ((drive_id << 4) | 0xA0, gibp -> atacmd + ATACMD_B_DRHEAD);	/* select the drive to be set up */
      outb ((1 << PAGE_SHIFT) / DISK_BLOCK_SIZE, gibp -> atacmd + ATACMD_B_SECCOUNT); /* set up number of sectors we want to do at a time */
      outb_p (0xC6, gibp -> atacmd + ATACMD_BW_COMMAND);			/* tell it to set the multiple block count */
      //oz_hw_stl_nanowait (400);							/* give the drive 400nS to start processing the command */
      sts = oz_hw_stl_microwait (50000, waitfornotbusy, &(gibp -> atactl));	/* wait for it to execute (max 50mS) */
      if (sts == 0) sts = inb (gibp -> atactl + ATACTL_BR_ALTSTS);
      if ((sts & 0xC9) == 0x40) {
        devex -> multsize        = (1 << PAGE_SHIFT) / DISK_BLOCK_SIZE;
        devex -> atacmd_read     = 0xC4;
        devex -> atacmd_read_inh = 0xC4;
        devex -> atacmd_write    = 0xC5;
        if (FORTYEIGHTBIT (devex)) {
          devex -> atacmd_read     = 0x29;
          devex -> atacmd_read_inh = 0x29;
          devex -> atacmd_write    = 0x39;
        }
      } else {
        printk ("oz_dev_ide8038i: - status %2.2X error %2.2X setting multiple sector transfers\n", 
		sts & 0xFF, inb (gibp -> atacmd + ATACMD_BR_ERROR));
      }
    }
  }

  /* Check for partitions */

  //oz_knl_devunit_autogen (devex -> devunit, oz_dev_disk_auto, NULL);
}

/************************************************************************/
/*									*/
/*  Probe for an ATAPI drive						*/
/*									*/
/************************************************************************/

static int probe_atapidrv (Gibp *gibp, int drive_id)

{
  char *p, unitdesc[OZ_DEVUNIT_DESCSIZE];
  Devex *devex;
  int i;
  struct _ucb *devunit;
  unsigned char status;
  unsigned long sts;

  /* Reset both drives on cable so they can't be screwed up by previous probing */

  outb_p (0x0E, gibp -> atactl + ATACTL_BW_DEVCTL);				// software reset the drives
  //  oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb_p (0x0A, gibp -> atactl + ATACTL_BW_DEVCTL);				// release the drives
  //oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb_p ((drive_id << 4) | 0xA0, gibp -> atacmd + ATACMD_B_DRHEAD);	// select the drive to be identified
  //oz_hw_stl_nanowait (400);

  /* Try to tell ATAPI drive to identify itself */

  outb_p (0xA1, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
  //oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
  memset (gibp -> secbuf, 0, sizeof gibp -> secbuf);				// zero fill secbuf
  status = oz_hw_stl_microwait (50000, getidentbuff, gibp);			// read it from drive
  if ((status & 0xC9) != 0x40) {
    printk ("oz_dev_ide8038i: - atapi ident status 0x%X\n", status);
    if (status & 0x01) printk ("oz_dev_ide8038i: - error code 0x%X\n", inb (gibp -> atacmd + ATACMD_BR_ERROR));
    return (0);
  }
  if ((gibp -> secbuf[0] & 0xC002) != 0x8000) {
    printk ("oz_dev_ide8038i: - atapi ident word[0] %4.4x not supported\n", gibp -> secbuf[0]);
    return (0);
  }

  /* We got something back, so assume it is an ATAPI device -         */
  /* So we set up an device that looks like an scsi controller        */
  /* but it will only process scsi id 0 or 1.  Then we call the scsi  */
  /* routines to create the appropriate class device for the unit.    */

  for (i = ATAPI_IDENT_W20_MODEL; i < ATAPI_IDENT_W20_MODEL + 20; i ++) {
    gibp -> secbuf[i] = (gibp -> secbuf[i] << 8) | (gibp -> secbuf[i] >> 8);
  }
  for (p = (char *)(gibp -> secbuf + ATAPI_IDENT_W20_MODEL + 20); *(-- p) == ' ';) {}
  *(++ p) = 0;
  printk ("oz_dev_ide8038i: - atapi %s\n", gibp -> secbuf + ATAPI_IDENT_W20_MODEL);
  sprintf (unitdesc, "access via %s.%u", gibp -> unitname, drive_id);
#if 0
  devunit = oz_knl_devunit_create (devdriver_scsi, gibp -> unitname, unitdesc, &atapi_functable, 0, oz_s_secattr_sysdev);
#endif
  gibp -> devexs[drive_id] = devex = oz_knl_devunit_ex (devunit);
  memset (devex, 0, sizeof *devex);
  if (gibp -> ctrlr == NULL) gibp -> ctrlr = ctrlr_create (gibp);
  devex -> devunit  = devunit;
  devex -> name     = oz_knl_devunit_devname (devunit);
  devex -> ctrlr    = gibp -> ctrlr;
  devex -> atapimsk = 1 << drive_id;
  devex -> atapiidentword0 = gibp -> secbuf[0];
  return (1);
}

/************************************************************************/
/*									*/
/*  Initialize ATAPI drive						*/
/*									*/
/************************************************************************/

static void init_atapidrv (Gibp *gibp, int drive_id)

{
  Devex *devex;
  unsigned char status;

  devex = gibp -> devexs[drive_id];

  /* Try to tell ATAPI drive to identify itself again after the reset */
  /* Otherwise, its status is funny                                   */

  outb_p (0xA1, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
  //oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
  memset (gibp -> secbuf, 0, sizeof gibp -> secbuf);				// zero fill secbuf
  status = oz_hw_stl_microwait (50000, getidentbuff, gibp);			// read it from drive
  if ((status & 0xC9) != 0x40) {
    printk ("oz_dev_ide8038i: %s atapi ident status 0x%X\n", devex -> name, status);
    if (status & 0x01) printk ("oz_dev_ide8038i: - error code 0x%X\n", inb (gibp -> atacmd + ATACMD_BR_ERROR));
    return;
  }
  if ((gibp -> secbuf[0] & 0xC022) != 0x8000) {
    printk ("oz_dev_ide8038i: %s atapi ident word[0] %4.4X not supported\n", devex -> name, gibp -> secbuf[0]);
    return;
  }

  /* Set up autogen routine.  When someone tries to access the <unitname>.<driveid> device, this will create it. */
  /* Can't create it directly via oz_dev_scsi_scan1 now because the scsi class driver probably isn't there yet.  */

  //oz_knl_devunit_autogen (devex -> devunit, oz_dev_scsi_auto, NULL);
}

/************************************************************************/
/*									*/
/*  Controller specific setup routines					*/
/*									*/
/************************************************************************/

	/* PIIX4 */

static void setup_piix4 (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *master, Devex *slave)

{
  if (master != NULL) setup_piix4_drive (pciconfp, cablesel, ctrlr, master);
  if (slave  != NULL) setup_piix4_drive (pciconfp, cablesel, ctrlr, slave);
}

static const unsigned char setup_piix4_udmatiming[6] = { 0xC2, 0xC1, 0xC0, 0xC4, 0xC5, 0xC6 };

static void setup_piix4_drive (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *devex)

{
  int offset, udmamode;
  unsigned char status;
  unsigned short atacmd;

  atacmd = ctrlr -> atacmd;

  offset = cablesel * 2 + devex -> drive_id;	// 0=primary_master; 1=primary_slave; 2=secondary_master; 3=secondary_slave

  /* If drive is capable of UDMA, set up UDMA mode */

  if ((devex -> ident[ATA_IDENT_W_UDMAMODES] & 0xFF) != 0) {
    for (udmamode = 8; !(devex -> ident[ATA_IDENT_W_UDMAMODES] & (1 << (-- udmamode)));) {}
    printk ("oz_dev_ide8038i: %s capable of UDMA mode %d\n", devex -> name, udmamode);
    if (udmamode > 2) {
      udmamode = 2;								// we don't do .gt. UDMA mode 2 (ATA33)
      printk ("oz_dev_ide8038i: - but fastest this chip can do is 2 (ATA33)\n");
    }
    outb ((devex -> drive_id << 4) | 0xA0, atacmd + ATACMD_B_DRHEAD);	// select the drive to be set
    outb (0x03, atacmd + ATACMD_BW_FEATURES);				// set subcommand = set transfer mode
    outb (0x40 + udmamode, atacmd + ATACMD_B_SECCOUNT);			// set parameter = UDMA mode 'udmamode'
    outb_p (0xEF, atacmd + ATACMD_BW_COMMAND);				// start setting the features
    //  oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing
    oz_hw_stl_microwait (50000, waitfornotbusy, &(ctrlr -> atactl));		// give it 50mS to finish
    status = inb (atacmd + ATACMD_BR_STATUS);
    if (status & 0x81) {
      printk ("oz_dev_ide8038i: - set UDMA mode %d status 0x%X\n", udmamode, status);
      if (status & 0x01) {
        status = inb (atacmd + ATACMD_BR_ERROR);
        printk ("oz_dev_ide8038i: - error code 0x%X\n", status);
      }
    } else {
      atacmd  = oz_dev_pci_conf_inb (pciconfp, 0x4A);				// set drive's UDMA mode (0, 1, 2)
      atacmd &= ~(3 << (offset * 4));
      atacmd |= udmamode << (offset * 4);
      oz_dev_pci_conf_outw (atacmd, pciconfp, 0x4A);
      status  = oz_dev_pci_conf_inb (pciconfp, 0x48);				// set drive's UDMA enable bit
      status |= 1 << offset;
      oz_dev_pci_conf_outb (status, pciconfp, 0x48);
    }
  }
}

	/* AMD 768 */

static void setup_amd768 (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *master, Devex *slave)

{
  if (master != NULL) setup_amd768_drive (pciconfp, cablesel, ctrlr, master);
  if (slave  != NULL) setup_amd768_drive (pciconfp, cablesel, ctrlr, slave);
}

static const unsigned char setup_amd768_udmatiming[6] = { 0xC2, 0xC1, 0xC0, 0xC4, 0xC5, 0xC6 };

static void setup_amd768_drive (OZ_Dev_pci_conf_p *pciconfp, int cablesel, Ctrlr *ctrlr, Devex *devex)

{
  int offset, udmamode;
  unsigned char status;
  unsigned short atacmd;

  atacmd = ctrlr -> atacmd;

  offset = cablesel * 2 + devex -> drive_id;	// 0=primary_master; 1=primary_slave; 2=secondary_master; 3=secondary_slave

  /* If drive is capable of UDMA, set up UDMA mode */

  if ((devex -> ident[ATA_IDENT_W_UDMAMODES] & 0xFF) != 0) {
    for (udmamode = 8; !(devex -> ident[ATA_IDENT_W_UDMAMODES] & (1 << (-- udmamode)));) {}
    printk ("oz_dev_ide8038i: %s capable of UDMA mode %d\n", devex -> name, udmamode);
    if (udmamode > 5) {
      udmamode = 5;								// we don't do .gt. UDMA mode 5 (ATA100)
      printk ("oz_dev_ide8038i: - but fastest this chip can do is 5 (ATA100)\n");
    }
    if (udmamode > 2) {								// can't do .gt. mode 2 (ATA33) with slow cable
      status = oz_dev_pci_conf_inb (pciconfp, 0x42);
      if (!(status & (1 << offset))) {
        udmamode = 2;
        printk ("oz_dev_ide8038i: - but using mode 2 (ATA33) because fast cable not present\n");
      }
    }
    outb ((devex -> drive_id << 4) | 0xA0, atacmd + ATACMD_B_DRHEAD);	// select the drive to be set
    outb (0x03, atacmd + ATACMD_BW_FEATURES);				// set subcommand = set transfer mode
    outb (0x40 + udmamode, atacmd + ATACMD_B_SECCOUNT);			// set parameter = UDMA mode 'udmamode'
    outb_p (0xEF, atacmd + ATACMD_BW_COMMAND);				// start setting the features
    //   oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing
    oz_hw_stl_microwait (50000, waitfornotbusy, &(ctrlr -> atactl));		// give it 50mS to finish
    status = inb (atacmd + ATACMD_BR_STATUS);
    if (status & 0x81) {
      printk ("oz_dev_ide8038i: - set UDMA mode %d status 0x%X\n", udmamode, status);
      if (status & 0x01) {
        status = inb (atacmd + ATACMD_BR_ERROR);
        printk ("oz_dev_ide8038i: - error code 0x%X\n", status);
      }
    } else {
      oz_dev_pci_conf_outb (setup_amd768_udmatiming[udmamode], pciconfp, 0x53 - offset); // ok, put controller in UDMA mode
    }
  }
}


/* Routine used by oz_hw_stl_microwait to wait for not busy */

static unsigned long waitfornotbusy (void *atactl_p)

{
  unsigned char status;
  unsigned short atactl;

  atactl = *(unsigned short *)atactl_p;					// get control port number
  status = inb_p (atactl + ATACTL_BR_ALTSTS);		// read status w/out acking interrupt
  if (status & 0x80) return (0);				// if still bussy, continue waiting
  //  oz_hw_stl_nanowait (400);					// give it a chance to update status
  status = inb (atactl + ATACTL_BR_ALTSTS);		// get updated status
  return (((unsigned long)status) | 0x100);				// return non-bussy status
}

/* Routine used by oz_hw_stl_microwait to read the ident buffer */

static unsigned long getidentbuff (void *gibpv)

{
  unsigned char status;
  Gibp *gibp;

  gibp = gibpv;

  status = inb (gibp -> atacmd + ATACMD_BR_STATUS);	// get current status
  if (status & 0x80) return (0);				// if busy, nothing else is valid
  oz_hw_stl_nanowait (400);					// give it a chance to update status
  status = inb (gibp -> atacmd + ATACMD_BR_STATUS);	// get updated status
  if (status & 0x08) {						// see if it has data for us
    insw (256, gibp -> atacmd + ATACMD_W_DATA, gibp -> secbuf); // if so, get it
    oz_hw_stl_nanowait (400);					// give it a chance to update status
    status = inb (gibp -> atacmd + ATACMD_BR_STATUS);	// get updated status
    if (status & 0x80) return (0);				// if still bussy, keep waiting
  }
  return (((unsigned long)status) | 0x100);				// not bussy, all done
}

/* Routine to create a ctrlr struct */

static Ctrlr *ctrlr_create (Gibp *gibp)

{
  Ctrlr *ctrlr;
  Dmatbl *dmatblva;
  unsigned long ppn;
  unsigned long len, ppo;

  ctrlr = (sizeof *ctrlr, GFP_KERNEL);					// malloc the struct
										// phys contig for scatter/gather map on end
  memset (ctrlr, 0, sizeof *ctrlr);						// clear it out
  ctrlr -> iopex_qt = &(ctrlr -> iopex_qh);
  ctrlr -> atacmd   = gibp -> atacmd;						// save i/o addresses
  ctrlr -> atactl   = gibp -> atactl;
  ctrlr -> bmiba    = gibp -> bmiba;
  ctrlr -> irq_many.entry = (gibp -> bmiba != 0) ? dma_intserv : pio_intserv;	// connect to the interrupt vector
  ctrlr -> irq_many.param = ctrlr;
  ctrlr -> irq_many.descr = "oz_dev_ide8038i";
  ctrlr -> smplock  = oz_hw486_irq_many_add (gibp -> irqlevel, &(ctrlr -> irq_many));
  ctrlr -> timer    = 1;//oz_knl_timer_alloc ();					// allocate a timer for interrupt timeouts

  ctrlr -> dmatblva = NULL;							// don't bother with DMA crap if not a DMA controller
  if (gibp -> bmiba != 0) {
    ctrlr -> dmatblva = (void *)(((unsigned long)(ctrlr -> dmatblbf) + 7) & -8);	// get table pointer, check quadword alignment
    len = oz_knl_misc_sva2pa (ctrlr -> dmatblva, &ppn, &ppo);			// get phys address of dma scatter/gather map
    if (len < DMATBLSIZ*2) panic ("oz_dev_ide8038i init: dmatbl %p len %u", ctrlr -> dmatblva, len);
    ctrlr -> dmatblpa = (ppn << PAGE_SHIFT) + ppo;
    if (((ctrlr -> dmatblpa + DMATBLSIZ - 1) & 0xFFFF0000) != (ctrlr -> dmatblpa & 0xFFFF0000)) { // see if 1st half crosses 64K boundary
      ctrlr -> dmatblva += DMATBLMAX;						// if so, switch to the second half
      ctrlr -> dmatblpa += DMATBLSIZ;
      if (((ctrlr -> dmatblpa + DMATBLSIZ - 1) & 0xFFFF0000) != (ctrlr -> dmatblpa & 0xFFFF0000)) {
        panic ("oz_dev_ide8038i init: dmatbl %p both halves cross 64K boundary", ctrlr -> dmatblbf);
      }
    }
  }

  return (ctrlr);
}

/************************************************************************/
/*									*/
/*  Shutdown device (ATA or ATAPI)					*/
/*									*/
/************************************************************************/

static int shutdown (struct _ucb *devunit, void *devexv)

{
  Ctrlr *ctrlr;
  Devex *devex;

  devex = devexv;
  ctrlr = devex -> ctrlr;

  if (ctrlr -> timer != NULL) exe$cantim (ctrlr);	/* cancel any interrupt timer */
  outb (0x0E, ctrlr -> atactl + ATACTL_BW_DEVCTL);		/* disable interrupts for the drives on this cable and reset drives */
  if (ctrlr -> bmiba != 0) outb (0, ctrlr -> bmiba + BMIBA_B_BMICX); /* abort any dma transfer */
  return (1);
}

/************************************************************************/
/*									*/
/*  Start performing an ATA-style disk i/o function			*/
/*									*/
/************************************************************************/

#if 0
static unsigned long ata_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode, 
                        struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap)
#endif
     
     static unsigned long startio (struct _irp * i, struct _ucb * u)

{
  struct _ucb *devunit=u;
  void *devexv;
  struct _ccb   *iochan=i->irp$l_ccb;
  void *chnexv;
  OZ_Procmode procmode;
  struct _irp *ioop=i;
  void *iopexv;
  unsigned long funcode=i->irp$v_fcode;
  unsigned long as;
  void *ap;


  Devex *devex;
  Iopex *iopex;
  unsigned long sts;

  devex = devexv;
  iopex = iopexv;

  iopex -> ioop     = ioop;
  iopex -> devex    = devex;
  iopex -> procmode = procmode;

  iopex -> dmatblva = NULL;
  iopex -> dmatblpa = 0;

  /* Process individual functions */

  switch (funcode) {

    /* Set volume valid bit one way or the other (noop for us) */

#if 0
    case OZ_IO_DISK_SETVOLVALID: {
	/* ?? have it process disk changed status bit */
      return (SS$_NORMAL);
    }
#endif

    /* Write blocks to the disk from virtual memory */

    case IO$_WRITEPBLK: {
      const unsigned long *phypages;
      OZ_IO_disk_writeblocks disk_writeblocks;
      unsigned long byteoffs;

      /* Lock I/O buffer in memory */

      // movc4 (as, ap, sizeof disk_writeblocks, &disk_writeblocks);
      // sts = oz_knl_ioop_lockr (ioop, disk_writeblocks.size, disk_writeblocks.buff, &phypages, NULL, &byteoffs);
      sts = SS$_NORMAL;

      /* If that was successful, queue the request to the drive for processing */

      iopex -> atacmdcode = devex -> atacmd_write;
      iopex -> writedisk  = 1;
      if (sts == SS$_NORMAL) sts = ata_queuereq (disk_writeblocks.size, disk_writeblocks.slbn, phypages, byteoffs, iopex);
      sts = ata_queuereq (i->irp$l_qio_p2, i->irp$l_qio_p3, phypages, byteoffs, iopex);
      return (sts);
    }

    /* Read blocks from the disk into virtual memory */

    case IO$_READPBLK: {
      const unsigned long *phypages;
      OZ_IO_disk_readblocks disk_readblocks;
      unsigned long byteoffs;

      /* Lock I/O buffer in memory */

      // movc4 (as, ap, sizeof disk_readblocks, &disk_readblocks);
      // sts = oz_knl_ioop_lockw (ioop, disk_readblocks.size, disk_readblocks.buff, &phypages, NULL, &byteoffs);
      sts = SS$_NORMAL;

      /* If that was successful, queue the request to the drive for processing */

      iopex -> atacmdcode = devex -> atacmd_read_inh;//devex -> atacmd_read;
      iopex -> writedisk  = 0;
      sts = ata_queuereq (i->irp$l_qio_p2, i->irp$l_qio_p3, phypages, byteoffs, iopex);
      return (sts);
    }

    /* Get info part 1 */
#if 0
    case OZ_IO_DISK_GETINFO1: {
      OZ_IO_disk_getinfo1 disk_getinfo1;

      if (!OZ_HW_WRITABLE (as, ap, procmode)) return (SS$_ACCVIO);
      memset (&disk_getinfo1, 0, sizeof disk_getinfo1);
      disk_getinfo1.blocksize   = DISK_BLOCK_SIZE;
      disk_getinfo1.totalblocks = devex -> totalblocks;
      disk_getinfo1.secpertrk   = devex -> secpertrk;
      disk_getinfo1.trkpercyl   = devex -> trkpercyl;
      disk_getinfo1.cylinders   = devex -> totalblocks / devex -> secpercyl;
      disk_getinfo1.bufalign    = BUFFER_ALIGNMENT;
      // movc4 (sizeof disk_getinfo1, &disk_getinfo1, as, ap);
      return (SS$_NORMAL);
    }

    /* Write blocks to the disk from physical pages (kernel only) */

    case OZ_IO_DISK_WRITEPAGES: {
      OZ_IO_disk_writepages disk_writepages;

      if (procmode != MODE_K_KERNEL) return (SS$_IVMODE);
      // movc4 (as, ap, sizeof disk_writepages, &disk_writepages);

      /* Queue the request to the drive for processing */

      iopex -> atacmdcode = devex -> atacmd_write;
      iopex -> writedisk  = 1;
      sts = ata_queuereq (disk_writepages.size, disk_writepages.slbn, disk_writepages.pages, disk_writepages.offset, iopex);
      return (sts);
    }

    /* Read blocks from the disk into physical pages (kernel only) */

    case OZ_IO_DISK_READPAGES: {
      OZ_IO_disk_readpages disk_readpages;

      if (procmode != MODE_K_KERNEL) return (SS$_IVMODE);
      // movc4 (as, ap, sizeof disk_readpages, &disk_readpages);

      /* Queue the request to the drive for processing */

      iopex -> atacmdcode = (disk_readpages.inhretries & 1) ? devex -> atacmd_read_inh : devex -> atacmd_read;
      iopex -> writedisk  = 0;
      sts = ata_queuereq (disk_readpages.size, disk_readpages.slbn, disk_readpages.pages, disk_readpages.offset, iopex);
      return (sts);
    }

    /* Set crash dump device */

    case OZ_IO_DISK_CRASH: {
      if (procmode != MODE_K_KERNEL) return (SS$_IVMODE);		/* caller must be in kernel mode */
      if (crash_devunit != NULL) {					/* get rid of old crash stuff, if any */
        oz_knl_devunit_increfc (crash_devunit, -1);
        crash_devex   = NULL;
        crash_devunit = NULL;
      }
      if (ap != NULL) {
        if (as != sizeof (OZ_IO_disk_crash)) return (OZ_BADBUFFERSIZE);	/* param block must be exact size */
        ((OZ_IO_disk_crash *)ap) -> crashentry = ata_crash;		/* return pointer to crash routine */
        ((OZ_IO_disk_crash *)ap) -> crashparam = NULL;			/* we don't require a parameter */
        ((OZ_IO_disk_crash *)ap) -> blocksize  = DISK_BLOCK_SIZE;	/* tell them our blocksize */
        crash_devex   = devex;						/* save the device we will write to */
        crash_devunit = devunit;
        oz_knl_devunit_increfc (crash_devunit, 1);			/* make sure it doesn't get deleted */
      }
      return (SS$_NORMAL);
    }
#endif

    /* Who knows what */

    default: {
      return (SS$_BADPARAM);
    }
  }
}

/************************************************************************/
/*									*/
/*  ATA-style crash dump routine - write logical blocks with 		*/
/*  interrupts disabled							*/
/*									*/
/*    Input:								*/
/*									*/
/*	lbn     = block to start writing at				*/
/*	size    = number of bytes to write (multiple of blocksize)	*/
/*	phypage = physical page to start writing from			*/
/*	offset  = offset in first physical page				*/
/*									*/
/*    Output:								*/
/*									*/
/*	ata_crash = SS$_NORMAL : successful				*/
/*	                  else : error status				*/
/*									*/
/************************************************************************/

static unsigned long ata_crash (void *dummy, unsigned long lbn, unsigned long size, unsigned long phypage, unsigned long offset)

{
  Ctrlr *ctrlr;
  unsigned long long now;
  unsigned char status;
  unsigned long sts, wsize;

  if ((size | offset) & BUFFER_ALIGNMENT) return (SS$_BUFNOTALIGN);

  ctrlr = crash_devex -> ctrlr;

  if (!crash_inprog) {
    recalibrate (crash_devex);		/* reset drive */
    crash_inprog = 1;			/* ... the first time only */
  }

  ctrlr -> iopex_ip    = NULL;		/* make sure the 'inprogress' queue is empty */
  ctrlr -> iopex_qh    = NULL;		/* make sure the 'pending' queue is empty */
  ctrlr -> iopex_qt    = &(ctrlr -> iopex_qh);
  ctrlr -> smplock     = NULL;		/* we can't use the smp lock anymore */
  ctrlr -> timer       = NULL;		/* make sure we don't use the timer anymore */
  ctrlr -> timerqueued = 0;

  /* Repeat as long as there is stuff to write */

  while (size > 0) {

    /* See how much we can write (up to end of current physical page) */

    wsize = (1 << PAGE_SHIFT) - offset;
    if (wsize > size) wsize = size;

    /* Queue write request and start processing it - since the queue is empty, it should start right away */

    ctrlr -> requestcount  = 0;
    memset (&crash_iopex, 0, sizeof crash_iopex);
    crash_iopex.procmode   = MODE_K_KERNEL;
    crash_iopex.devex      = crash_devex;
    crash_iopex.status     = SS$_QUEUED;
    crash_iopex.writedisk  = 1;
    crash_iopex.atacmdcode = crash_devex -> atacmd_write;
    sts = ata_queuereq (wsize, lbn, &phypage, offset, &crash_iopex);
    if (sts == SS$_ALLSTARTED) {

      /* Now keep calling the interrupt service routine until the request completes */

      while (ctrlr -> requestcount != 0) {
        status = inb (ctrlr -> atacmd + ATACMD_BR_STATUS);	/* get drive's status byte */
        if ((status & 0x89) ^ 0x80) {					/* if it is no longer busy or it wants data ... */
          (*(ctrlr -> irq_many.entry)) (ctrlr, NULL);			/* ... call interrupt service routine */
          continue;
        }
        now = oz_hw_tod_getnow ();					/* still busy, get current time */
        if (OZ_HW_DATEBIN_CMP (ctrlr -> timerwhen, now) <= 0) {		/* see if timer has expired */
          ctrlr -> timerqueued = 0;					/* if so, say fake timer no longer queued */
          (*(ctrlr -> irq_many.entry)) (ctrlr, NULL);			/* ... and call interrupt service routine */
        }
      }
      sts = crash_iopex.status;						/* get completion status */
    }

    /* Check the completion status */

    if (sts != SS$_NORMAL) {
      printk ("oz_dev_ide8038i crash: error %u writing to lbn %u\n", sts, lbn);
      return (sts);
    }

    /* Ok, on to next physical page */

    size    -= wsize;
    offset  += wsize;
    phypage += offset >> PAGE_SHIFT;
    offset  &= (1 << PAGE_SHIFT) - 1;
    lbn     += wsize / DISK_BLOCK_SIZE;
  }

  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Queue ATA-style I/O request						*/
/*									*/
/*    Input:								*/
/*									*/
/*	size = size of transfer in bytes				*/
/*	slbn = starting logical block number				*/
/*	phypages = pointer to array of physical page numbers		*/
/*	byteoffs = byte offset in first physical page			*/
/*	iopex = iopex block to use for operation			*/
/*	iopex -> writedisk  = set if writing to disk			*/
/*	iopex -> atacmdcode = read/write ATA command code byte		*/
/*									*/
/*    Output:								*/
/*									*/
/*	ata_queuereq = SS$_ALLSTARTED : requeust queued to disk drive and 	*/
/*	                           drive started if it was idle		*/
/*	                    else : error status				*/
/*									*/
/************************************************************************/

static unsigned long ata_queuereq (unsigned long size, unsigned long slbn, const unsigned long *phypages, unsigned long byteoffs, Iopex *iopex)

{
  Ctrlr *ctrlr;
  Devex *devex;
  unsigned long hd;
  unsigned long elbn;

  devex = iopex -> devex;
  ctrlr = devex -> ctrlr;

  /* If no buffer, instant success */

  if (size == 0) return (SS$_NORMAL);

  /* The buffer must be long aligned for DMA since it is done a long at a time */

  if ((size | byteoffs) & BUFFER_ALIGNMENT) return (SS$_BUFNOTALIGN);

  /* Make sure request doesn't run off end of disk */

  elbn = (size + DISK_BLOCK_SIZE - 1) / DISK_BLOCK_SIZE + slbn;
  if (elbn < slbn) return (SS$_BADPARAM);
  if (elbn > devex -> totalblocks) return (SS$_BADPARAM);

  /* Make up a read/write request struct */

  iopex -> size        = size;				/* save buffer size */
  iopex -> slbn        = slbn;				/* save starting logical block number */
  iopex -> phypages    = phypages;			/* save physical page array pointer */
  iopex -> byteoffs    = byteoffs;			/* save starting physical page byte offset */
  iopex -> timedout    = 0;				/* hasn't timed out yet */
  iopex -> amount_done = 0;				/* nothing has been transferred yet */
  iopex -> drive_id    = devex -> drive_id;		/* set up drive id number */
  iopex -> retries     = MAX_RW_RETRIES;		/* init retry counter */
  if ((iopex -> writedisk | iopex -> atacmdcode) & 1) iopex -> retries = 1; /* (no retries for writes or if told so) */

  hd = oz_hw_smplock_wait (ctrlr -> smplock);		/* inhibit hardware interrupts */
  validaterequestcount (ctrlr, __LINE__);		/* make sure queues are ok as they stand */
  ctrlr -> requestcount ++;				/* ok, there is now one more request */
  queuereq (iopex);					/* queue the request to the controller */
  validaterequestcount (ctrlr, __LINE__);		/* make sure queues are ok before releasing lock */
  oz_hw_smplock_clr (ctrlr -> smplock, hd);		/* restore hardware interrupts */

  return (SS$_ALLSTARTED);					/* the request has been started */
}

/************************************************************************/
/*									*/
/*  Assign and deassign channel to ATAPI controller			*/
/*									*/
/************************************************************************/

static unsigned long atapi_assign (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode)

{
  ((Chnex *)chnexv) -> drive_id = -1;						// init chnex saying no scsi device opened
  return (SS$_NORMAL);								// always successful
}

static int atapi_deassign (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv)

{
  char drive_id;
  Chnex *chnex;

  chnex = chnexv;

  drive_id = chnex -> drive_id;							// see if anything open on channel
  if (drive_id >= 0) {
    chnex -> drive_id = -1;							// if so, say it is closed now
    oz_hw_atomic_and_long (&(((Devex *)devexv) -> atapiopn), ~ (1 << drive_id)); // let someone else open it now
  }
  return (0);
}

/************************************************************************/
/*									*/
/*  Start performing an ATAPI i/o function				*/
/*									*/
/************************************************************************/

static unsigned long atapi_start (struct _ucb *devunit, void *devexv, struct _ccb *iochan, void *chnexv, OZ_Procmode procmode, 
                          struct _irp *ioop, void *iopexv, unsigned long funcode, unsigned long as, void *ap)

{
  Chnex *chnex;
  Devex *devex;
  Iopex *iopex;
  unsigned long sts;

  chnex = chnexv;
  devex = devexv;
  iopex = iopexv;

  iopex -> ioop     = ioop;
  iopex -> devex    = devex;
  iopex -> procmode = procmode;

  iopex -> dmatblva = NULL;
  iopex -> dmatblpa = 0;

  /* Process individual functions */

  switch (funcode) {

    /* Open one of the drives on this channel */

#if 0
  case IO$_READPBLK:
      long drive_id_mask;
      OZ_IO_scsi_open scsi_open;
      unsigned char drive_id;

      // movc4 (as, ap, sizeof scsi_open, &scsi_open);
      drive_id = scsi_open.scsi_id;					// get drive id (0=master, 1=slave)
      if (drive_id > 1) return (OZ_BADSCSIID);				// allow only 0 or 1
      drive_id_mask = (1 << drive_id);					// make a mask for it
      if (!(devex -> atapimsk & drive_id_mask)) return (OZ_BADSCSIID);	// make sure that it was found to be ATAPI in init routine
      if (chnex -> drive_id >= 0) return (OZ_FILEALREADYOPEN);		// make sure nothing already open on this channel
      if (oz_hw_atomic_or_long (&(devex -> atapiopn), drive_id_mask) & drive_id_mask) return (OZ_ACCONFLICT); // mark it open now
      chnex -> drive_id = drive_id;					// remember which one we opened on this channel
      return (SS$_NORMAL);						// successful
    }

    /* Perform an scsi command on the drive, with virtual buffer address */

    case OZ_IO_SCSI_DOIO: {
      OZ_IO_scsi_doio scsi_doio;
      unsigned long sts;

      // movc4 (as, ap, sizeof scsi_doio, &scsi_doio);
      sts = oz_dev_scsi_cvtdoio2pp (ioop, procmode, &scsi_doio, &(iopex -> doiopp));
      iopex -> drive_id = chnex -> drive_id;
      if (sts == SS$_NORMAL) sts = atapi_queuereq (iopex);
      return (sts);
    }

    /* Perform an scsi command on the drive, with physical buffer address (kernel mode only) */

    case OZ_IO_SCSI_DOIOPP: {
      if (procmode != MODE_K_KERNEL) return (SS$_IVMODE);
      // movc4 (as, ap, sizeof iopex -> doiopp, &(iopex -> doiopp));
      iopex -> drive_id = chnex -> drive_id;
      sts = atapi_queuereq (iopex);
      return (sts);
    }

    /* Get scsi controller info */

    case OZ_IO_SCSI_GETINFO1: {
      OZ_IO_scsi_getinfo1 scsi_getinfo1;

      if (!OZ_HW_WRITABLE (as, ap, procmode)) return (SS$_ACCVIO);
      memset (&scsi_getinfo1, 0, sizeof scsi_getinfo1);		// clear stuff we don't know about
      scsi_getinfo1.max_scsi_id  = 2;				// allow only scsi id 0 and 1
      scsi_getinfo1.ctrl_scsi_id = devex -> atapimsk & 1;	// controller is whichever the device isn't
								//   atapimsk 1 -> ctrl_scsi_id 1
								//   atapimsk 2 -> ctrl_scsi_id 0
      scsi_getinfo1.open_scsi_id = chnex -> drive_id;		// tell caller what scsi-id is open on channel (-1 if closed)
      scsi_getinfo1.open_width   = 1;				// we're always 16-bits wide
      // movc4 (sizeof scsi_getinfo1, &scsi_getinfo1, as, ap);	// copy info back to caller
      return (SS$_NORMAL);
    }

    /* We don't support these, so let them fall through to the error */

    case OZ_IO_SCSI_RESET:
    case OZ_IO_SCSI_CRASH:

    /* Who knows what */
#endif

    default: {
      return (SS$_BADPARAM);
    }
  }
}

/************************************************************************/
/*									*/
/*  Queue ATAPI-style request to the controller				*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex = request to be queued					*/
/*	iopex -> ioop     = request's ioop				*/
/*	iopex -> procmode = requestor's processor mode			*/
/*	iopex -> devex    = device (atapi controller device)		*/
/*	iopex -> doiopp   = function parameter block			*/
/*	iopex -> drive_id = 0: master, 1: slave				*/
/*									*/
/*    Output:								*/
/*									*/
/*	atapi_queuereq = queuing status					*/
/*									*/
/************************************************************************/

static unsigned long atapi_queuereq (Iopex *iopex)

{
  Ctrlr *ctrlr;
  Devex *devex;
  unsigned long ppn;
  unsigned long cl, hd, ppo, sts;

  devex = iopex -> devex;
  ctrlr = devex -> ctrlr;

//  if (iopex -> drive_id > 1) return (OZ_BADSCSIID);

  /* Copy command into atapicmd and zero pad */

  cl = iopex -> doiopp.cmdlen;									// get length of supplied command
  hd = 12;											// maybe device takes 12 byte packets
  if (devex -> atapiidentword0 & 1) hd = 16;							// maybe device takes 16 byte packets
  if (cl > hd) return (SS$_BUFFEROVF);								// if command too long, return error
//sts = oz_knl_section_uget (iopex -> procmode, cl, iopex -> doiopp.cmdbuf, iopex -> atapicmd);	// ok, copy in the command
  if (sts != SS$_NORMAL) return (sts);
  memset (iopex -> atapicmd + cl, 0, hd - cl);							// zero pad it

  /* The data buffer must be long aligned for DMA since it is done a long at a time */

  if ((iopex -> doiopp.datasize | iopex -> doiopp.databyteoffs) & BUFFER_ALIGNMENT) return (SS$_BUFNOTALIGN);

  /* If transfer request is large, maybe we need a big dmatbl */

  iopex -> doiopp.dataphypages += iopex -> doiopp.databyteoffs >> PAGE_SHIFT;
  iopex -> doiopp.databyteoffs %= 1 << PAGE_SHIFT;

  iopex -> dmatblmx = ((iopex -> doiopp.datasize + iopex -> doiopp.databyteoffs) >> PAGE_SHIFT) + 1;
  iopex -> dmatblva = NULL;
  iopex -> dmatblpa = 0;
  if ((ctrlr -> bmiba != 0) && (iopex -> dmatblmx >= DMATBLMAX)) {
    iopex -> dmatblva = kmalloc (iopex -> dmatblmx * sizeof *(iopex -> dmatblva), GFP_KERNEL);
    if (iopex -> dmatblva == NULL) return (SS$_EXQUOTA);
// cl = oz_knl_misc_sva2pa (iopex -> dmatblva, &ppn, &ppo);
    if (cl < iopex -> dmatblmx * sizeof *(iopex -> dmatblva)) panic ("oz_dev_ide8038i atapi_queuereq: dmatbl not phys contig");
    iopex -> dmatblpa = (ppn << PAGE_SHIFT) + ppo;
  }

  /* Put request on queue */

//  iopex -> writedisk   = ((iopex -> doiopp.optflags & OZ_IO_SCSI_OPTFLAG_WRITE) != 0);		// set whether or not we are writing to device
  iopex -> timedout    = 0;									// hasn't timed out yet
  iopex -> retries     = 0;									// ATAPI's never retry   
  iopex -> atapists    = 0;									// assume successful
  iopex -> size        = iopex -> doiopp.datasize;						// for timeout's error message

  iopex -> byteoffs    = iopex -> doiopp.databyteoffs;						// get offset in first page for data transfer
  iopex -> phypages    = iopex -> doiopp.dataphypages;						// get pointer to array of physical page numbers
  iopex -> amount_xfrd = 0;									// haven't transferred anything yet
  iopex -> amount_done = 0;
  iopex -> slbn        = 0;									// make all ATAPI request go in order received

  hd = oz_hw_smplock_wait (ctrlr -> smplock);							// inhibit hardware interrupts
  validaterequestcount (ctrlr, __LINE__);							// make sure queues are ok as they stand
  ctrlr -> requestcount ++;									// ok, there is now one more request
  queuereq (iopex);										// queue the request to the controller
  validaterequestcount (ctrlr, __LINE__);							// make sure queues are ok before releasing lock
  oz_hw_smplock_clr (ctrlr -> smplock, hd);							// restore hardware interrupts

  return (SS$_ALLSTARTED);										// the request has been started
}

/************************************************************************/
/*									*/
/*  This routine (re-)queues a request to the controller		*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex = pointer to request to be queued				*/
/*	smplock = drives irq smplock					*/
/*									*/
/*    Output:								*/
/*									*/
/*	request queued to controller					*/
/*	drive started if not already busy				*/
/*									*/
/************************************************************************/

static void queuereq (Iopex *iopex)

{
  Ctrlr *ctrlr;
  Iopex **liopex, *xiopex;

  iopex -> status = SS$_QUEUED;									/* reset status (for crash routine) */

  /* Point to the controller struct */

  ctrlr = iopex -> devex -> ctrlr;

  /* Link it on to end of controller's request queue */

  *(ctrlr -> iopex_qt) = iopex;
  iopex -> next = NULL;
  ctrlr -> iopex_qt = &(iopex -> next);

  validaterequestcount (ctrlr, __LINE__);

  /* If there is no request being processed, start it going */

  if (ctrlr -> iopex_ip == NULL) startreq (ctrlr);
}

/************************************************************************/
/*									*/
/*  Start processing the request that's on top of queue			*/
/*									*/
/*    Input:								*/
/*									*/
/*	ctrlr = pointer to controller					*/
/*	ctrlr -> iopex_qh/qt = read/write request queue			*/
/*	ctrlr -> iopex_ip = assumed to be NULL				*/
/*									*/
/*	smplock = device's irq smp lock					*/
/*									*/
/*    Output:								*/
/*									*/
/*	ctrlr -> iopex_ip = filled in with top request			*/
/*	ctrlr -> iopex_qh/qt = top request removed			*/
/*	disk operation started						*/
/*									*/
/************************************************************************/

static void startreq (Ctrlr *ctrlr)

{
  const unsigned long *phypages;
  Devex *devex;
  int i, j;
  Iopex *iopex;
  unsigned long long now, delta;
  unsigned long lba;
  unsigned char status;
  unsigned long bytecount, bytelimit, byteoffs, bytesindma, physaddr, seccount, sts;

  /* Dequeue the request that's on the top */

dequeue:
  validaterequestcount (ctrlr, __LINE__);
  iopex = ctrlr -> iopex_qh;							/* get pointer to top request */
  if (iopex == NULL) return;							/* just return if queue was empty */
  ctrlr -> iopex_ip = iopex;							/* got one, mark it 'in progress' */
  if ((ctrlr -> iopex_qh = iopex -> next) == NULL) ctrlr -> iopex_qt = &(ctrlr -> iopex_qh); /* unlink it from queue */
  devex = iopex -> devex;							/* get which device the request is for */
  validaterequestcount (ctrlr, __LINE__);

  /* Make sure the drive is ready to accept a new command */

  if (ctrlr -> bmiba != 0) outb (0, ctrlr -> bmiba + BMIBA_B_BMICX);	/* shut off any old dma stuff */
  outb ((iopex -> drive_id << 4) | 0xE0, ctrlr -> atacmd + ATACMD_B_DRHEAD); /* select the drive and LBA mode */
  status = inb (ctrlr -> atacmd + ATACMD_BR_STATUS);			/* make sure drive is ready and not requesting data */
  if ((status & 0xC8) != 0x40) {
    oz_hw_stl_nanowait (400);
    status = inb (ctrlr -> atacmd + ATACMD_BR_STATUS);			/* give it a second chance (to clear error bits) */
    if ((status & 0xC8) != 0x40) {
      printk ("oz_dev_ide8038i startreq: status 0x%x, error 0x%x\n", status, inb (ctrlr -> atacmd + ATACMD_BR_ERROR));
      iopex -> status = SS$_SSFAIL;						/* something goofy with drive */
      reqdone (iopex);
      goto dequeue;
    }
  }

  /* If ATAPI request, do it differently */

  if (devex -> atapimsk != 0) goto start_atapi;

  /*********************/
  /* Start ATA request */
  /*********************/

  /* Determine number of sectors we have yet to transfer */

  if ((iopex -> amount_done % DISK_BLOCK_SIZE) != 0) panic ("oz_dev_ide8038i_486 startreq: amount_done %u not multiple of block size", iopex -> amount_done);
  seccount = (iopex -> size - iopex -> amount_done + DISK_BLOCK_SIZE - 1) / DISK_BLOCK_SIZE;	/* determine number of sectors to process */
  if (seccount > MAX_SEC_COUNT) seccount = MAX_SEC_COUNT;					/* max number of sectors at a time */
  iopex -> seccount = seccount;
  iopex -> amount_to_xfer = seccount * DISK_BLOCK_SIZE;

  /* Calculate starting lbn */

  lba = iopex -> slbn + iopex -> amount_done / DISK_BLOCK_SIZE;

  /* Start dma controller */

  iopex -> amount_xfrd = 0;							/* haven't transferred anything yet (in case of PIO) */
  if (ctrlr -> bmiba != 0) startdma (iopex, iopex -> amount_to_xfer, DISK_BLOCK_SIZE); /* set up dma to transfer the whole thing */

  /* Fill in device registers to start the request */

  if (FORTYEIGHTBIT (devex)) {
    outb ((devex -> drive_id << 4) | 0xE0, ctrlr -> atacmd + ATACMD_B_DRHEAD); /* select the drive, LBA mode */
    outb (seccount >> 8, ctrlr -> atacmd + ATACMD_B_SECCOUNT);		/* store sector count<15:08> */
    outb     (lba >> 24, ctrlr -> atacmd + ATACMD_B_LBALOW);		/* store lba<31:24> */
    outb             (0, ctrlr -> atacmd + ATACMD_B_LBAMID);		/* store lba<39:32> */
    outb             (0, ctrlr -> atacmd + ATACMD_B_LBAHIGH);		/* store lba<47:40> */
    outb      (seccount, ctrlr -> atacmd + ATACMD_B_SECCOUNT);		/* store sector count<07:00> */
    outb           (lba, ctrlr -> atacmd + ATACMD_B_LBALOW);		/* store lba<07:00> */
    outb      (lba >> 8, ctrlr -> atacmd + ATACMD_B_LBAMID);		/* store lba<15:08> */
    outb     (lba >> 16, ctrlr -> atacmd + ATACMD_B_LBAHIGH);		/* store lba<23:16> */
  } else if (LBAMODE (devex)) {
    outb      (0x08, ctrlr -> atactl + ATACTL_BW_DEVCTL);		//??
    outb         (0, ctrlr -> atacmd + ATACMD_BW_FEATURES);	//??
    outb  (seccount, ctrlr -> atacmd + ATACMD_B_SECCOUNT);		/* store sector count (let it wrap to 0 for 256) */
    outb       (lba, ctrlr -> atacmd + ATACMD_B_LBALOW);			/* store lba<07:00> */
    outb  (lba >> 8, ctrlr -> atacmd + ATACMD_B_LBAMID);			/* store lba<15:08> */
    outb (lba >> 16, ctrlr -> atacmd + ATACMD_B_LBAHIGH);			/* store lba<23:16> */
    outb ((devex -> drive_id << 4) | (lba >> 24) | 0xE0, ctrlr -> atacmd + ATACMD_B_DRHEAD); /* select the drive, lba<27:24>, LBA mode */
  } else {
    unsigned long cylinder, sector, track;

    cylinder =  lba / devex -> secpercyl;
    track    = (lba / devex -> secpertrk) % devex -> trkpercyl;
    sector   = (lba % devex -> secpertrk) + 1;

    outb ((devex -> drive_id << 4) | track | 0xA0, ctrlr -> atacmd + ATACMD_B_DRHEAD); /* select the drive, lba<27:24>, LBA mode */
    outb      (seccount, ctrlr -> atacmd + ATACMD_B_SECCOUNT);		/* store sector count (let it wrap to 0 for 256) */
    outb      (cylinder, ctrlr -> atacmd + ATACMD_B_CYL_LO);		/* store lba<07:00> */
    outb (cylinder >> 8, ctrlr -> atacmd + ATACMD_B_CYL_HI);		/* store lba<15:08> */
    outb        (sector, ctrlr -> atacmd + ATACMD_B_SECNUM);		/* store sector number */
  }
  outb (iopex -> atacmdcode, ctrlr -> atacmd + ATACMD_BW_COMMAND);	/* start the request */

  delta = OZ_TIMER_RESOLUTION * ATA_TIMEOUT;
  goto finishup;

  /***********************/
  /* Start ATAPI request */
  /***********************/

start_atapi:

  /* Start dma stuff going */

  iopex -> amount_to_xfer = iopex -> doiopp.datasize;
  if ((ctrlr -> bmiba != 0) && (iopex -> amount_to_xfer != 0)) {
    startdma (iopex, iopex -> doiopp.datasize, 1);
    if (iopex -> amount_xfrd != iopex -> doiopp.datasize) {
      printk ("oz_dev_ide8038i startreq: dma can only handle %u out of %u byte ATAPI data transfer\n", iopex -> amount_xfrd, iopex -> doiopp.datasize);
      //      iopex -> status = OZ_BADBUFFERSIZE;
      reqdone (iopex);
      goto dequeue;
    }
  }

  /* Fill in registers to start command (drive was already selected above) */

  if (ctrlr -> bmiba != 0) {
    outb (1, ctrlr -> atacmd + ATACMD_BW_FEATURES);			// we're doing DMA data transfer
    outb (0xFE, ctrlr -> atacmd + ATACMD_B_CYL_LO);			// these only apply for PIO mode supposedly
    outb (0xFF, ctrlr -> atacmd + ATACMD_B_CYL_HI);
  } else {
    bytecount = iopex -> doiopp.datasize;					// get amount to be PIO'd
    if (bytecount > 0xFFFE) bytecount = 0xFFFE;					// ... but it can only do this much at once
    outb (0, ctrlr -> atacmd + ATACMD_BW_FEATURES);			// we're doing PIO data transfer
    outb (bytecount, ctrlr -> atacmd + ATACMD_B_CYL_LO);			// set bytecount to transfer
    outb (bytecount >> 8, ctrlr -> atacmd + ATACMD_B_CYL_HI);
  }
  outb (0, ctrlr -> atacmd + ATACMD_B_SECCOUNT);				// no command queuing tag
  outb (0xA0, ctrlr -> atacmd + ATACMD_BW_COMMAND);			// start the ATAPI request

  /* The command packet always gets sent via PIO */

  oz_hw_stl_nanowait (400);							/* wait 400nS before checking BUSY & DRQ */

  if (!(devex -> atapiidentword0 & 0x20)) {					/* if set, it interrupts us to get command */
    j = 3000;									/* else, we have to wait up to 3mS for some old clunkers */
    if (devex -> atapiidentword0 & 0x40) j = 50;				/* wait up to 50uS for modren clunkers to be ready */
    sts = oz_hw_stl_microwait (j, waitfornotbusy, &(ctrlr -> atactl));
    if (sts == 0) sts = inb (ctrlr -> atactl + ATACTL_BR_ALTSTS);
    if ((sts & 0x89) != 0x08) {							/* BUSY<7> off, DRQ<3> on, ERR<0> off */
      printk ("oz_dev_ide8038i: %s status %2.2X (error %2.2X) waiting to send command packet\n", 
	devex -> name, (unsigned char)sts, inb (ctrlr -> atacmd + ATACMD_BR_ERROR));
      iopex -> status = SS$_SSFAIL;
      reqdone (iopex);
      goto dequeue;
    }

    j = inb (ctrlr -> atacmd + ATACMD_B_SECCOUNT);			/* it should have the low bit set indicating command transfer */
    if (!(j & 1)) {
      printk ("oz_dev_ide8038i: %s didn't enter command transfer state (seccount %2.2X)\n", devex -> name, j);
      iopex -> status = SS$_SSFAIL;
      reqdone (iopex);
      goto dequeue;
    }

    j = 6;									/* some drives take 12-byte command packets */
    if (devex -> atapiidentword0 & 1) j = 8;					/* some drives take 16-byte command packets */
    outsw (j, iopex -> atapicmd, ctrlr -> atacmd + ATACMD_W_DATA);	/* copy data out to drive */
  }

  /* Determine timeout required */

  ctrlr -> timerwhen = -1LL;
  if (iopex -> doiopp.timeout == 0) goto finishup_notimeout;
  delta = iopex -> doiopp.timeout * (OZ_TIMER_RESOLUTION / 1000);

  /*****************************************/
  /* Common (both ATA and ATAPI) finish up */
  /*****************************************/

finishup:
  now = oz_hw_tod_getnow ();							/* get current date/time */
  OZ_HW_DATEBIN_ADD (ctrlr -> timerwhen, delta, now);				/* add the timeout delta value */
  if (ctrlr -> timer != NULL) {							/* (fake it if crash dumping) */
    if (!(ctrlr -> timerqueued) || exe$cantim (ctrlr)) {
      ctrlr -> timerqueued = 1;
      //oz_knl_timer_insert (ctrlr -> timer, ctrlr -> timerwhen, reqtimedout, ctrlr); /* stick new request in queue */
      exe$setimr(0,&ctrlr -> timerwhen, reqtimedout, ctrlr);
    }
  }

  /* If PIO write, start by writing the first chunk */

finishup_notimeout:
  if (iopex -> writedisk && (ctrlr -> bmiba == 0)) {				// see if a PIO write operation
    oz_hw_stl_nanowait (400);							// wait 400nS before checking BUSY & DRQ
    if (oz_hw_stl_microwait (5, waitfornotbusy, &(ctrlr -> atactl)) != 0) {	// wait up to 5uS for it to ask for data
      pio_intserv (ctrlr, NULL);						// write a chunk of data to the drive
    }
  }
}

/************************************************************************/
/*									*/
/*  Fill in a controller's dma table for a request or set up PIO stuff	*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex -> phypages = physical page number array			*/
/*	iopex -> byteoffs = byte offset in first physical page		*/
/*	iopex -> amount_done = amount of buffer previously completed	*/
/*	iopex -> size = total buffer length				*/
/*	iopex -> writedisk & 1 = 0 : disk-to-memory transfer		*/
/*	                         1 : memory-to-disk transfer		*/
/*	xfersize  = number of bytes to transfer this time		*/
/*	blocksize = pad to multiple of this size			*/
/*	ctrlr -> bmiba    = dma controller I/O port base address	*/
/*	ctrlr -> dmatblva = dma pointer table virtual address		*/
/*									*/
/*    Output:								*/
/*									*/
/*	*(ctrlr -> dmatblva) = table all set up for transfer		*/
/*	iopex -> amount_xfrd = number of bytes set up to transfer	*/
/*	dma controller started						*/
/*									*/
/************************************************************************/

static void startdma (Iopex *iopex, unsigned long xfersize, unsigned long blocksize)

{
  const unsigned long *phypages;
  Ctrlr *ctrlr;
  Dmatbl *dmatblbas, *dmatblpnt;
  unsigned long dmatblphy;
  unsigned long bytecount, bytelimit, byteoffs, bytesindma, dmatblmax, physaddr, sts;

  ctrlr = iopex -> devex -> ctrlr;

  /* Fill in pointer table */

  dmatblpnt = ctrlr -> dmatblva;								/* point to dma scatter/gather table */
  dmatblmax = DMATBLMAX;
  dmatblphy = ctrlr -> dmatblpa;
  if (iopex -> dmatblva != NULL) {
    dmatblpnt = iopex -> dmatblva;								/* transfer too big for standard table */
    dmatblmax = iopex -> dmatblmx;								/* ... use the temp one */
    dmatblphy = iopex -> dmatblpa;
  }
  dmatblbas = dmatblpnt;

  byteoffs  = iopex -> byteoffs;								/* this is the offset in the first phys page */
  phypages  = iopex -> phypages;								/* this is pointer to physical page array */

  byteoffs += iopex -> amount_done;								/* offset by how much we did last time through */
  phypages += byteoffs >> PAGE_SHIFT;
  byteoffs &= (1 << PAGE_SHIFT) - 1;

  bytelimit = xfersize;										/* this is the number of bytes to transfer */
  if (bytelimit > iopex -> size - iopex -> amount_done) bytelimit = iopex -> size - iopex -> amount_done; /* slightly less if partial sector transfer */
  iopex -> amount_xfrd = bytelimit;								/* save how much we are doing this time */

#if PAGE_SHIFT >= 16
  error : below code assumes page size < 64k to prevent entries from being 64k or larger and to prevent 64k wrap-around
#endif

  bytesindma = 0;
  while (bytelimit != 0) {
    physaddr  = (*(phypages ++) << PAGE_SHIFT) + byteoffs;				/* generate physical address to start at */
    bytecount = (1 << PAGE_SHIFT) - byteoffs;						/* this is how many bytes we can transfer on that page */
    byteoffs  = 0;										/* start at offset zero in next physpage */
    if (bytecount > bytelimit) bytecount = bytelimit;						/* if more left on that page than we want, chop it off */
    dmatblpnt -> physaddr  = physaddr;								/* save the physical address */
    dmatblpnt -> bytecount = bytecount;								/* save the transfer bytecount */
    dmatblpnt ++;										/* increment table entry pointer */
    bytelimit  -= bytecount;									/* reduce number of bytes we have to go */
    bytesindma += bytecount;									/* accumulate total number of bytes processed */
  }
  bytesindma &= blocksize - 1;									/* see if partial block transfer */
  if (bytesindma != 0) {
    dmatblpnt -> physaddr = 0;									/* if so, for reads, read into phypage 0 */
    if (iopex -> writedisk & 1) dmatblpnt -> physaddr = (unsigned long)zeroes;				/*        for write, use buffer full of zeroes */
    dmatblpnt -> bytecount = blocksize - bytesindma;
    dmatblpnt ++;
  }
  dmatblpnt[-1].bytecount |= 0x80000000;							/* done, terminate table */

  if (dmatblpnt - dmatblbas > dmatblmax) {							/* there should always be room */
    panic ("oz_dev_8038i startdma: used %d dmatbl entries, max %d", dmatblpnt - dmatblbas, dmatblmax);
  }

  /* Start the DMA controller */

  outb (0x66, ctrlr -> bmiba + BMIBA_B_BMISX);						/* reset dma error status bits */
  outl (dmatblphy, ctrlr -> bmiba + BMIBA_L_BMIDTPX);					/* set descriptor table base address */
  outb ((((iopex -> writedisk & 1) ^ 1) << 3) | 0x01, ctrlr -> bmiba + BMIBA_B_BMICX);	/* start the dma controller */
}

/* Process recalibrate command - ATA drives only */

static void recalibrate (Devex *devex)

{
  Ctrlr *ctrlr;
  unsigned char status;

  ctrlr = devex -> ctrlr;

  outb ((devex -> drive_id << 4) | 0xE0, ctrlr -> atacmd + ATACMD_B_DRHEAD); /* store drive number */

  outb (0x10, ctrlr -> atacmd + ATACMD_BW_COMMAND);			/* start the recalibrate */
  status = oz_hw_stl_microwait (200000, checkrecaldone, ctrlr);			/* give it up to 200mS to finish */
  if (status == 0) {
    printk ("oz_dev_ide8038i recalibrate: status 0x%x, error 0x%x\n", inb (ctrlr -> atacmd + ATACMD_BR_STATUS), inb (ctrlr -> atacmd + ATACMD_BR_ERROR));
  }
}

/* Check to see if recalibrate command has finished */

static unsigned long checkrecaldone (void *ctrlrv)

{
  unsigned char status;

  status = inb (((Ctrlr *)ctrlrv) -> atacmd + ATACMD_BR_STATUS);
  if ((status & 0xC0) != 0x40) status = 0;
  return (status);
}

/************************************************************************/
/*									*/
/*  Timer ran out waiting for interrupt					*/
/*									*/
/*  If there is a request going, call the interrupt service routine 	*/
/*  and restart timer							*/
/*									*/
/************************************************************************/

static void reqtimedout (void *ctrlrv, OZ_Timer *timer)

{
  Ctrlr *ctrlr;
  Devex *devex;
  Iopex *iopex;
  unsigned long long now;
  unsigned long hd;

  ctrlr = ctrlrv;

  hd = oz_hw_smplock_wait (ctrlr -> smplock);			/* set interrupt lock */
  ctrlr -> timerqueued = 0;					/* say the timer is no longer queued */
  iopex = ctrlr -> iopex_ip;					/* make sure a request is in progress */
  if (iopex != NULL) {
    now = oz_hw_tod_getnow ();					/* get current date/time */
    if (OZ_HW_DATEBIN_CMP (ctrlr -> timerwhen, now) <= 0) {	/* make sure it's really up (it may have been bumped since it was queued) */
      devex = iopex -> devex;
      if (devex -> atapimsk != 0) {
        printk ("oz_dev_ide8038i: %s atapi command %2.2X timed out, datasize %u, transferred %u\n", 
                       devex -> name, iopex -> atapicmd[0], iopex -> size, iopex -> amount_xfrd);
      } else {
        printk ("oz_dev_ide8038i: %s atacmd %2.2X timed out, lbn %u, size %u\n", devex -> name, iopex -> atacmdcode, iopex -> slbn, iopex -> size);
        printk ("            amount_done %u, amount_xfrd %u\n", iopex -> amount_done, iopex -> amount_xfrd);
      }
      iopex -> timedout = 1;					/* remember it got a timeout */
      (*(ctrlr -> irq_many.entry)) (ctrlr, NULL);		/* call the interrupt routine to process whatever is left of request */
      if (!(ctrlr -> timerqueued) && (ctrlr -> iopex_ip != NULL)) {
        ctrlr -> timerqueued = 1;				/* restart timer if there is still a request in progress */
        ctrlr -> timerwhen   = now + (OZ_TIMER_RESOLUTION * ATA_TIMEOUT);
        oz_hw_smplock_clr (ctrlr -> smplock, hd);
        //oz_knl_timer_insert (ctrlr -> timer, ctrlr -> timerwhen, reqtimedout, ctrlr);
	exe$setimr(0,&ctrlr -> timerwhen, reqtimedout, ctrlr);
        return;
      }
    } else {
      ctrlr -> timerqueued = 1;					/* it got bumped since it was queued, re-queue at new date/time */
      oz_hw_smplock_clr (ctrlr -> smplock, hd);			/* release interrupt lock */
      //oz_knl_timer_insert (ctrlr -> timer, ctrlr -> timerwhen, reqtimedout, ctrlr);
      exe$setimr(0,&ctrlr -> timerwhen, reqtimedout, ctrlr);
      return;
    }
  }
  oz_hw_smplock_clr (ctrlr -> smplock, hd);
}

/************************************************************************/
/*									*/
/*  This is the DMA interrupt routine - it is called at irq level when 	*/
/*  the drive completes an operation					*/
/*									*/
/*  It is also called when the timeout happens				*/
/*									*/
/*    Input:								*/
/*									*/
/*	ctrlrv  = pointer to controller that interrupted		*/
/*	smplock = the associated irq's smplock is set			*/
/*									*/
/************************************************************************/

static int dma_intserv (void *ctrlrv, OZ_Mchargs *mchargs)

{
  Ctrlr *ctrlr;
  Devex *devex;
  Iopex *iopex;
  unsigned char bmisx, status;
  unsigned short atacmd;

  ctrlr  = ctrlrv;
  atacmd = ctrlr -> atacmd;

  validaterequestcount (ctrlr, __LINE__);

  /* Clear the interrupt bit in the DMA controller */

  bmisx = inb (ctrlr -> bmiba + BMIBA_B_BMISX);
  outb (0x64, ctrlr -> bmiba + BMIBA_B_BMISX);

  /* Read the status and clear interrupt */

  status = inb (atacmd + ATACMD_BR_STATUS);

  /* Get current request pointer */

  iopex  = ctrlr -> iopex_ip;
  if (iopex == NULL) return (0);
  devex  = iopex -> devex;

  /* Check for ATAPI command delivery - some drives interrupt when ready to receive command packet */

  if (atapi_command_packet (iopex, status)) return (0);

  /* Make sure drive is no longer busy - if so and timer is still going, let it interrupt again */
  /* If timer is no longer going, it means it has timed out, so abort the request               */

  if ((status & 0x80) || (!(status & 0x01) && (status & 0x08) && !(bmisx & 0x02))) {
    printk ("oz_dev_ide8038i: %s busy or drq still set (status 0x%x, bmisx 0x%x)\n", devex -> name, status, bmisx);
    if (ctrlr -> timerqueued) {
      validaterequestcount (ctrlr, __LINE__);
      return (0);
    }
  }

  /* Make sure dma controller no longer busy */

  if (bmisx & 0x01) {
    printk ("oz_dev_ide8038i: %s dma busy still set (0x%x)\n", devex -> name, bmisx);
    printk ("oz_dev_ide8038i: - transfer size %u\n", iopex -> amount_xfrd);
    outb (0, ctrlr -> bmiba + BMIBA_B_BMICX);
  }

  /* Process the interrupt */

  if (devex -> atapimsk == 0) ata_finish (iopex, status);
  else atapi_finish (iopex, status);

  return (0);
}

/************************************************************************/
/*									*/
/*  This is the PIO interrupt routine - it is called at irq level when 	*/
/*  the drive completes an operation or wants to transfer data		*/
/*									*/
/*  It is also called when the timeout happens				*/
/*									*/
/*    Input:								*/
/*									*/
/*	ctrlrv  = pointer to controller that interrupted		*/
/*	smplock = the associated irq's smplock is set			*/
/*									*/
/************************************************************************/

static int pio_intserv (void *ctrlrv, OZ_Mchargs *mchargs)

{
  Ctrlr *ctrlr;
  Devex *devex;
  Iopex *iopex;
  unsigned char status;

  ctrlr  = ctrlrv;

  validaterequestcount (ctrlr, __LINE__);

  /* Read the status and clear interrupt */

  status = inb (ctrlr -> atacmd + ATACMD_BR_STATUS);

  /* Get current request pointer */

  iopex = ctrlr -> iopex_ip;
  if (iopex == NULL) return (0);
  devex = iopex -> devex;

  /* Process the interrupt */

  if (devex -> atapimsk == 0) pio_ata_intserv (iopex, status);
  else pio_atapi_intserv (iopex, status);

  return (0);
}

static void pio_ata_intserv (Iopex *iopex, unsigned char status)

{
  Ctrlr *ctrlr;
  Devex *devex;
  int j;
  unsigned char error;
  unsigned short atacmd;

  devex  = iopex -> devex;
  ctrlr  = devex -> ctrlr;
  atacmd = ctrlr -> atacmd;

  /* Maybe it needs some data - ATA devices get a block */

  while ((status & 0x89) == 0x08) {							/* BUSY<7> off, DRQ<3> on, ERR<0> off */

    if (((iopex -> atacmdcode & 0xFE) == 0xC4) && (devex -> multsize != 0)) {		/* ATA read/write multiple : do the group of blocks */
      j = devex -> multsize * DISK_BLOCK_SIZE;
      if (j + iopex -> amount_xfrd > iopex -> seccount * DISK_BLOCK_SIZE) {		// ... or maybe just to the end of the transfer size
        j = iopex -> seccount * DISK_BLOCK_SIZE - iopex -> amount_xfrd;
      }
    } else {
      j = DISK_BLOCK_SIZE;								/* standard ATA requests : do exactly one block */
    }
    do_pio_transfer (j, iopex);								// transfer to/from caller's buffer
    if ((iopex -> writedisk) || (iopex -> amount_xfrd < iopex -> amount_to_xfer)) return;
											/* note: we must return after transferring just one block of data */
											/*       if we try to be efficient by looping, we get timeout errors */
    inb (ctrlr -> atactl + ATACTL_BR_ALTSTS);					/* give controller time to come up with post-transfer status */
    status = inb (atacmd + ATACMD_BR_STATUS);					/* re-check the status */
    if (status & 0x80) return;								/* wait for another interrupt if it's busy again */
  }

  ata_finish (iopex, status);
}

static void pio_atapi_intserv (Iopex *iopex, unsigned char status)

{
  int j;
  unsigned short atacmd, atactl;

  atacmd = iopex -> devex -> ctrlr -> atacmd;
  atactl = iopex -> devex -> ctrlr -> atactl;

  /* Check for ATAPI command delivery - some drives interrupt when ready to receive command packet */

  if (atapi_command_packet (iopex, status)) {
    inb (atactl + ATACTL_BR_ALTSTS);						/* give controller time to come up with post-transfer status */
    status = inb (atacmd + ATACMD_BR_STATUS);					/* re-check the status */
    if (status & 0x80) return;								/* wait for another interrupt if it's busy again */
  }

  /* Maybe it needs some data - ATAPI devices get bytecount in cyllo/cylhi registers */

  while ((status & 0x89) == 0x08) {							/* BUSY<7> off, DRQ<3> on, ERR<0> off */
    j  = inb (atacmd + ATACMD_B_CYL_HI) << 8;					// get transfer bytecount
    j += inb (atacmd + ATACMD_B_CYL_LO);
    do_pio_transfer (j, iopex);								// transfer to/from caller's buffer
    inb (atactl + ATACTL_BR_ALTSTS);						/* give controller time to come up with post-transfer status */
    status = inb (atacmd + ATACMD_BR_STATUS);					/* re-check the status */
    if (status & 0x80) return;								/* wait for another interrupt if it's busy again */
  }

  atapi_finish (iopex, status);
}

static int atapi_command_packet (Iopex *iopex, unsigned char status)

{
  Devex *devex;
  int cmdlen;

  devex = iopex -> devex;

  if ((devex -> atapiidentword0 & 0x60) != 0x20) return (0);	// only these drives interrupt for command packet transfer
  if ((status & 0x89) != 0x08) return (0);			// BUSY<7> off, DRQ<3> on, ERR<0> off
  status = inb (devex -> ctrlr -> atacmd + ATACMD_B_SECCOUNT); // it has the low bit set indicating command transfer
  if (!(status & 1)) return (0);

  cmdlen = 6;							// some drives take 12-byte command packets
  if (devex -> atapiidentword0 & 1) cmdlen = 8;			// some drives take 16-byte command packets
  outsw (cmdlen, iopex -> atapicmd, devex -> ctrlr -> atacmd + ATACMD_W_DATA); // copy data out to drive
  return (1);							// tell caller we sent command out
}

/************************************************************************/
/*									*/
/*  Do a PIO transfer							*/
/*									*/
/*    Input:								*/
/*									*/
/*	nbytes = number of bytes to transfer				*/
/*	iopex  = I/O request being processed				*/
/*	      -> byteoffs = byte offset in original requestor's buffer	*/
/*	      -> phypages = physical page array of original reqestor's buffer
/*	      -> amount_done = amount of whole original transfer done by previous commands
/*	      -> amount_xfrd = amount previously done for this command	*/
/*									*/
/*    Output:								*/
/*									*/
/*	data transferred						*/
/*	iopex -> amount_xfrd = incremented				*/
/*									*/
/************************************************************************/

static void do_pio_transfer (int nbytes, Iopex *iopex)

{
  const unsigned long *phypages;
  int i, pad;
  OZ_Pagentry oldpte;
  unsigned long byteoffs;
  unsigned short atacmd_w_data, *vad;

  atacmd_w_data = iopex -> devex -> ctrlr -> atacmd + ATACMD_W_DATA;

  byteoffs  = iopex -> byteoffs;					/* get offset of first byte in original page */
  phypages  = iopex -> phypages;					/* get pointer to first element of physical page array */
  byteoffs += iopex -> amount_done + iopex -> amount_xfrd;		/* increment offset in first page by the amount already done and transferred */
  phypages += (byteoffs >> PAGE_SHIFT);				/* normalize with physical page array pointer */
  byteoffs %= 1 << PAGE_SHIFT;
  vad = oz_hw_phys_mappage (*phypages, &oldpte);			/* map physical page to virtual address */
  (unsigned long)vad += byteoffs;

  i    = iopex -> size - iopex -> amount_done - iopex -> amount_xfrd;	/* how many bytes left in the whole request to process */
  pad  = nbytes - i;							/* any left over is padding */
  if (pad > 0) nbytes = i;						/* just transfer this much to/from data buffer */
  pad /= 2;								/* make pad a word count */

  if (iopex -> writedisk) {
    while (1) {
      i = (1 << PAGE_SHIFT) - byteoffs;				/* get number of bytes to end of page */
      if (i > nbytes) i = nbytes;					/* but don't do more than drive wants */
      outsw (i / 2, vad, atacmd_w_data);				/* give it the data */
      iopex -> amount_xfrd += i;					/* this much more has been transferred */
      nbytes -= i;							/* this much less to do */
      if (nbytes == 0) break;						/* stop if all done */
      byteoffs += i;							/* more to transfer, get offset to it */
      phypages += (byteoffs >> PAGE_SHIFT);			/* normalize with physical page array pointer */
      byteoffs %= 1 << PAGE_SHIFT;
      vad = oz_hw_phys_mappage (*phypages, NULL);			/* map physical page to virtual address */
      (unsigned long)vad += byteoffs;
    }
    while (-- pad >= 0) outw (0, atacmd_w_data);			/* zero fill padding */
  } else {
    while (1) {
      i = (1 << PAGE_SHIFT) - byteoffs;				/* get number of bytes to end of page */
      if (i > nbytes) i = nbytes;					/* but don't do more than drive has */
      insw (i / 2, atacmd_w_data, vad);				/* get the data */
      iopex -> amount_xfrd += i;					/* this much more has been transferred */
      nbytes -= i;							/* this much less to do */
      if (nbytes == 0) break;						/* stop if all done */
      byteoffs += i;							/* more to transfer, get offset to it */
      phypages += (byteoffs >> PAGE_SHIFT);			/* normalize with physical page array pointer */
      byteoffs %= 1 << PAGE_SHIFT;
      vad = oz_hw_phys_mappage (*phypages, NULL);			/* map physical page to virtual address */
      (unsigned long)vad += byteoffs;
    }
    while (-- pad >= 0) inw (atacmd_w_data);			/* discard padding */
  }

  oz_hw_phys_unmappage (oldpte);					/* unmap the physical page */
}

/************************************************************************/
/*									*/
/*  Finish up an (PIO or DMA) ATA request				*/
/*									*/
/************************************************************************/

static void ata_finish (Iopex *iopex, unsigned char status)

{
  Ctrlr *ctrlr;
  Devex *devex;
  int j;
  unsigned char error;
  unsigned short atacmd;

  devex  = iopex -> devex;
  ctrlr  = devex -> ctrlr;
  atacmd = ctrlr -> atacmd;

  /* Make sure drive is no longer busy - if so and timer is still going, let it interrupt again */
  /* If timer is no longer going, it means it has timed out, so abort the request               */

  if ((status & 0x88) && !(status & 0x01)) {
    printk ("oz_dev_ide pio_intserv: busy or drq still set (status 0x%x)\n", status);
    if (ctrlr -> timerqueued) {
      validaterequestcount (ctrlr, __LINE__);
      return;
    }
  }

  /* Request is complete, check the status bits for error */

  iopex -> status = SS$_NORMAL;
  if (status & 0xA1) {
    iopex -> status = SS$_NOWRT;
    if (status & 0x01) {
      error  = inb (atacmd + ATACMD_BR_ERROR);
      printk ("oz_dev_ide intserv: status 0x%x, error 0x%x\n", status, error);
      iopex -> status = SS$_SSFAIL;
      if (error & 0x80) iopex -> status = SS$_BADMEDIA;
    }
  }

  /* If IOFAILED or BADMEDIA, maybe retry */

  if (((iopex -> status == SS$_SSFAIL) || (iopex -> status == SS$_BADMEDIA)) && (-- (iopex -> retries) > 0)) goto requeue;

  /* If successful, accumulate amount transferred so far.  Then if not complete, re-queue it. */

  if ((iopex -> status == SS$_NORMAL) && (devex -> atapimsk == 0)) {
    iopex -> amount_done += iopex -> amount_xfrd;			/* if successful, increment amount done */
    if (iopex -> amount_done > iopex -> size) iopex -> amount_done = iopex -> size; /* (padded out a short block) */
    if (iopex -> amount_done < iopex -> size) goto requeue;		/* if more to go, re-queue request to finish up */
  }

  /* It is complete (good or bad) */

  reqdone (iopex);							/* put completed request on completion queue */
  startreq (ctrlr);							/* start another request */
  validaterequestcount (ctrlr, __LINE__);
  return;

  /* Either continue or re-try the request */

requeue:
  ctrlr -> iopex_ip = NULL;
  queuereq (iopex);
  validaterequestcount (ctrlr, __LINE__);
}

/************************************************************************/
/*									*/
/*  Finish up an (PIO or DMA) ATAPI request				*/
/*									*/
/************************************************************************/

static void atapi_finish (Iopex *iopex, unsigned char status)

{
  Ctrlr *ctrlr;
  Devex *devex;
  unsigned char error;

  devex = iopex -> devex;
  ctrlr = devex -> ctrlr;

  /* Make sure drive is no longer busy - if so and timer is still going, let it interrupt again */
  /* If timer is no longer going, it means it has timed out, so abort the request               */

  if (status & 0x80) {
    printk ("oz_dev_ide pio_intserv: %s busy (status 0x%X)\n", devex -> name, status);
    if (ctrlr -> timerqueued) {
      validaterequestcount (ctrlr, __LINE__);
      return;
    }
  }

  /* Request is complete, check the status bits for error */

  iopex -> atapists = 0;
  iopex -> status = SS$_NORMAL;
  if ((status & 0xC9) != 0x40) {				// BSY=0; DRDY=1; DRQ=0; ERR=0
    if ((status & 0xC9) == 0x41) {
      error  = inb (ctrlr -> atacmd + ATACMD_BR_ERROR);
      if (error & 0x04) {
        printk ("oz_dev_ide intserv: %s status 0x%X, error 0x%X\n", devex -> name, status, error);
        iopex -> status = SS$_SSFAIL;
      } else {
        iopex -> atapists = error >> 4;
      }
    } else {
      printk ("oz_dev_ide intserv: %s status 0x%X\n", devex -> name, status);
      iopex -> status = SS$_SSFAIL;
    }
  }

  /* It is complete (good or bad) */

  reqdone (iopex);							/* put completed request on completion queue */
  startreq (ctrlr);							/* start another request */
  validaterequestcount (ctrlr, __LINE__);
}

/************************************************************************/
/*									*/
/*  An request is done							*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopex   = the completed request with status filled in		*/
/*	smplock = irq level						*/
/*									*/
/*    Output:								*/
/*									*/
/*	request posted for completion					*/
/*									*/
/************************************************************************/

static void reqdone (Iopex *iopex)

{
  Ctrlr *ctrlr;
  Devex *devex;

  devex = iopex -> devex;
  ctrlr = devex -> ctrlr;
  ctrlr -> iopex_ip = NULL;
  ctrlr -> requestcount --;
  if (iopex -> timedout) printk ("oz_dev_ide8038i: %s request complete, status %u\n", iopex -> devex -> name, iopex -> status);
  if (iopex -> ioop != NULL) {
    if ((devex -> atapimsk != 0) && ((iopex -> doiopp.datarlen != NULL) 
                                  || (iopex -> doiopp.status != NULL) 
                                  || (iopex -> dmatblva != NULL))) {
      oz_knl_iodonehi (iopex -> ioop, iopex -> status, NULL, atapi_reqdone, iopex);
    }
    else oz_knl_iodonehi (iopex -> ioop, iopex -> status, NULL, NULL, NULL);
  }
}

/* Back in requestor's memory space and requestor wants the ATAPI data result   */
/* length and/or status byte returned, or there is a temp dma table to free off */

static void atapi_reqdone (void *iopexv, int finok, unsigned long *status_r)

{
  Iopex *iopex;
  unsigned long sts;

  iopex = iopexv;

  //if (iopex -> dmatblva != NULL) OZ_KNL_NPPFREE (iopex -> dmatblva);

  if (finok) {
    if (iopex -> doiopp.datarlen != NULL) {
      //sts = oz_knl_section_uput (iopex -> procmode, sizeof *(iopex -> doiopp.datarlen), &(iopex -> amount_xfrd), iopex -> doiopp.datarlen);
      if (*status_r == SS$_NORMAL) *status_r = sts;
    }
    if (iopex -> doiopp.status != NULL) {
      //sts = oz_knl_section_uput (iopex -> procmode, sizeof *(iopex -> doiopp.status), &(iopex -> atapists), iopex -> doiopp.status);
      if (*status_r == SS$_NORMAL) *status_r = sts;
    }
  }
}

/************************************************************************/
/*									*/
/*  Make sure we haven't lost an request somewhere			*/
/*									*/
/*    Input:								*/
/*									*/
/*	ctrlr -> requestcount = number of pending I/O requests		*/
/*	smplevel = hd							*/
/*									*/
/************************************************************************/

static void validaterequestcount (Ctrlr *ctrlr, int line)

{
  int reqcount;
  Iopex *iopex;

  reqcount = 0;
  if (ctrlr -> iopex_ip != NULL) reqcount ++;

  for (iopex = ctrlr -> iopex_qh; iopex != NULL; iopex = iopex -> next) reqcount ++;

  if (reqcount != ctrlr -> requestcount) {
    panic ("oz_dev_ide8038i validaterequestcount: found %u, should have %u requests", reqcount, ctrlr -> requestcount);
  }
}

// my stuff here

static struct _irp * globali;
static struct _ucb * globalu;

void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

void  startio2 (struct _irp * i, struct _ucb * u) { 
  u->ucb$l_fpc=startio3;
  exe$iofork(i,u);
  return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
  struct _irp * i;
  struct _ucb * u;
  void (*func)();

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  ubd_handler();
  /* have to do this until we get things more in order */
  i=globali;
  u=globalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

static struct _fdt fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
static void  startio ();
static void  unsolint (void) { };
static void  functb (void) { };
static void  cancel (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
static void  altstart (void) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

struct _ddt ddt = {
  ddt$l_start: startio,
  ddt$l_unsolint: unsolint,
  ddt$l_functb: functb,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: altstart,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

static struct _ddb ddb;
static struct _dpt dpt;

int acp_std$readblk();

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

int ide_vmsinit(void) {
  unsigned short chan;
  $DESCRIPTOR(u0,"hda0");
  struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda0","hddriver");
  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&fdt,IO$_READLBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt,IO$_READPBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt,IO$_READVBLK,acp_std$readblk,1);
  exe$assign(&u0,&chan,0,0,0);
  registerdevchan(MKDEV(IDE0_MAJOR,0),chan);
  return chan;
}

/*
 *  linux/drivers/ide/ide.c		Version 6.31	June 9, 2000
 *
 *  Copyright (C) 1994-1998  Linus Torvalds & authors (see below)
 */

/*
 *  Mostly written by Mark Lord  <mlord@pobox.com>
 *                and Gadi Oxman <gadio@netvision.net.il>
 *                and Andre Hedrick <andre@linux-ide.org>
 *
 *  See linux/MAINTAINERS for address of current maintainer.
 *
 * This is the multiple IDE interface driver, as evolved from hd.c.
 * It supports up to MAX_HWIFS IDE interfaces, on one or more IRQs (usually 14 & 15).
 * There can be up to two drives per interface, as per the ATA-2 spec.
 *
 * Primary:    ide0, port 0x1f0; major=3;  hda is minor=0; hdb is minor=64
 * Secondary:  ide1, port 0x170; major=22; hdc is minor=0; hdd is minor=64
 * Tertiary:   ide2, port 0x???; major=33; hde is minor=0; hdf is minor=64
 * Quaternary: ide3, port 0x???; major=34; hdg is minor=0; hdh is minor=64
 * ...
 *
 *  From hd.c:
 *  |
 *  | It traverses the request-list, using interrupts to jump between functions.
 *  | As nearly all functions can be called within interrupts, we may not sleep.
 *  | Special care is recommended.  Have Fun!
 *  |
 *  | modified by Drew Eckhardt to check nr of hd's from the CMOS.
 *  |
 *  | Thanks to Branko Lankester, lankeste@fwi.uva.nl, who found a bug
 *  | in the early extended-partition checks and added DM partitions.
 *  |
 *  | Early work on error handling by Mika Liljeberg (liljeber@cs.Helsinki.FI).
 *  |
 *  | IRQ-unmask, drive-id, multiple-mode, support for ">16 heads",
 *  | and general streamlining by Mark Lord (mlord@pobox.com).
 *
 *  October, 1994 -- Complete line-by-line overhaul for linux 1.1.x, by:
 *
 *	Mark Lord	(mlord@pobox.com)		(IDE Perf.Pkg)
 *	Delman Lee	(delman@ieee.org)		("Mr. atdisk2")
 *	Scott Snyder	(snyder@fnald0.fnal.gov)	(ATAPI IDE cd-rom)
 *
 *  This was a rewrite of just about everything from hd.c, though some original
 *  code is still sprinkled about.  Think of it as a major evolution, with
 *  inspiration from lots of linux users, esp.  hamish@zot.apana.org.au
 *
 *  Version 1.0 ALPHA	initial code, primary i/f working okay
 *  Version 1.3 BETA	dual i/f on shared irq tested & working!
 *  Version 1.4 BETA	added auto probing for irq(s)
 *  Version 1.5 BETA	added ALPHA (untested) support for IDE cd-roms,
 *  ...
 * Version 5.50		allow values as small as 20 for idebus=
 * Version 5.51		force non io_32bit in drive_cmd_intr()
 *			change delay_10ms() to delay_50ms() to fix problems
 * Version 5.52		fix incorrect invalidation of removable devices
 *			add "hdx=slow" command line option
 * Version 5.60		start to modularize the driver; the disk and ATAPI
 *			 drivers can be compiled as loadable modules.
 *			move IDE probe code to ide-probe.c
 *			move IDE disk code to ide-disk.c
 *			add support for generic IDE device subdrivers
 *			add m68k code from Geert Uytterhoeven
 *			probe all interfaces by default
 *			add ioctl to (re)probe an interface
 * Version 6.00		use per device request queues
 *			attempt to optimize shared hwgroup performance
 *			add ioctl to manually adjust bandwidth algorithms
 *			add kerneld support for the probe module
 *			fix bug in ide_error()
 *			fix bug in the first ide_get_lock() call for Atari
 *			don't flush leftover data for ATAPI devices
 * Version 6.01		clear hwgroup->active while the hwgroup sleeps
 *			support HDIO_GETGEO for floppies
 * Version 6.02		fix ide_ack_intr() call
 *			check partition table on floppies
 * Version 6.03		handle bad status bit sequencing in ide_wait_stat()
 * Version 6.10		deleted old entries from this list of updates
 *			replaced triton.c with ide-dma.c generic PCI DMA
 *			added support for BIOS-enabled UltraDMA
 *			rename all "promise" things to "pdc4030"
 *			fix EZ-DRIVE handling on small disks
 * Version 6.11		fix probe error in ide_scan_devices()
 *			fix ancient "jiffies" polling bugs
 *			mask all hwgroup interrupts on each irq entry
 * Version 6.12		integrate ioctl and proc interfaces
 *			fix parsing of "idex=" command line parameter
 * Version 6.13		add support for ide4/ide5 courtesy rjones@orchestream.com
 * Version 6.14		fixed IRQ sharing among PCI devices
 * Version 6.15		added SMP awareness to IDE drivers
 * Version 6.16		fixed various bugs; even more SMP friendly
 * Version 6.17		fix for newest EZ-Drive problem
 * Version 6.18		default unpartitioned-disk translation now "BIOS LBA"
 * Version 6.19		Re-design for a UNIFORM driver for all platforms,
 *			  model based on suggestions from Russell King and
 *			  Geert Uytterhoeven
 *			Promise DC4030VL now supported.
 *			add support for ide6/ide7
 *			delay_50ms() changed to ide_delay_50ms() and exported.
 * Version 6.20		Added/Fixed Generic ATA-66 support and hwif detection.
 *			Added hdx=flash to allow for second flash disk
 *			  detection w/o the hang loop.
 *			Added support for ide8/ide9
 *			Added idex=ata66 for the quirky chipsets that are
 *			  ATA-66 compliant, but have yet to determine a method
 *			  of verification of the 80c cable presence.
 *			  Specifically Promise's PDC20262 chipset.
 * Version 6.21		Fixing/Fixed SMP spinlock issue with insight from an old
 *			  hat that clarified original low level driver design.
 * Version 6.30		Added SMP support; fixed multmode issues.  -ml
 * Version 6.31		Debug Share INTR's and request queue streaming
 *			Native ATA-100 support
 *			Prep for Cascades Project
 *
 *  Some additional driver compile-time options are in ./include/linux/ide.h
 *
 *  To do, in likely order of completion:
 *	- modify kernel to obtain BIOS geometry for drives on 2nd/3rd/4th i/f
 *
 */

#define	REVISION	"Revision: 6.31"
#define	VERSION		"Id: ide.c 6.31 2000/06/09"

#undef REALLY_SLOW_IO		/* most systems can safely undef this */

#define _IDE_C			/* Tell ide.h it's really us */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/genhd.h>
#include <linux/blkpg.h>
#include <linux/slab.h>
#ifndef MODULE
#include <linux/init.h>
#endif /* MODULE */
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/completion.h>

#include <asm/byteorder.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/bitops.h>

#include "ide_modes.h"

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif /* CONFIG_KMOD */

/* default maximum number of failures */
#define IDE_DEFAULT_MAX_FAILURES 	1

static const byte ide_hwif_to_major[] = { IDE0_MAJOR, IDE1_MAJOR, IDE2_MAJOR, IDE3_MAJOR, IDE4_MAJOR, IDE5_MAJOR, IDE6_MAJOR, IDE7_MAJOR, IDE8_MAJOR, IDE9_MAJOR };

static int	idebus_parameter; /* holds the "idebus=" parameter */
static int	system_bus_speed; /* holds what we think is VESA/PCI bus speed */
static int	initializing;     /* set while initializing built-in drivers */

#ifdef CONFIG_BLK_DEV_IDEPCI
static int	ide_scan_direction;	/* THIS was formerly 2.2.x pci=reverse */
#endif /* CONFIG_BLK_DEV_IDEPCI */

#if defined(__mc68000__) || defined(CONFIG_APUS)
/*
 * ide_lock is used by the Atari code to obtain access to the IDE interrupt,
 * which is shared between several drivers.
 */
static int	ide_lock;
#endif /* __mc68000__ || CONFIG_APUS */

int noautodma = 0;

/*
 * ide_modules keeps track of the available IDE chipset/probe/driver modules.
 */
ide_module_t *ide_modules;
ide_module_t *ide_probe;

/*
 * This is declared extern in ide.h, for access by other IDE modules:
 */
ide_hwif_t	ide_hwifs[MAX_HWIFS];	/* master data repository */

#if (DISK_RECOVERY_TIME > 0)
/*
 * For really screwy hardware (hey, at least it *can* be used with Linux)
 * we can enforce a minimum delay time between successive operations.
 */
static unsigned long read_timer (void)
{
	unsigned long t, flags;
	int i;

	__save_flags(flags);	/* local CPU only */
	__cli();		/* local CPU only */
	t = jiffies * 11932;
    	outb_p(0, 0x43);
	i = inb_p(0x40);
	i |= inb(0x40) << 8;
	__restore_flags(flags);	/* local CPU only */
	return (t - i);
}
#endif /* DISK_RECOVERY_TIME */

static inline void set_recovery_timer (ide_hwif_t *hwif)
{
#if (DISK_RECOVERY_TIME > 0)
	hwif->last_time = read_timer();
#endif /* DISK_RECOVERY_TIME */
}

/*
 * Do not even *think* about calling this!
 */
static void init_hwif_data (unsigned int index)
{
	unsigned int unit;
	hw_regs_t hw;
	ide_hwif_t *hwif = &ide_hwifs[index];

	/* bulk initialize hwif & drive info with zeros */
	memset(hwif, 0, sizeof(ide_hwif_t));
	memset(&hw, 0, sizeof(hw_regs_t));

	/* fill in any non-zero initial values */
	hwif->index     = index;
	ide_init_hwif_ports(&hw, ide_default_io_base(index), 0, &hwif->irq);
	memcpy(&hwif->hw, &hw, sizeof(hw));
	memcpy(hwif->io_ports, hw.io_ports, sizeof(hw.io_ports));
	hwif->noprobe	= !hwif->io_ports[IDE_DATA_OFFSET];
#ifdef CONFIG_BLK_DEV_HD
	if (hwif->io_ports[IDE_DATA_OFFSET] == HD_DATA)
		hwif->noprobe = 1; /* may be overridden by ide_setup() */
#endif /* CONFIG_BLK_DEV_HD */
	hwif->major	= ide_hwif_to_major[index];
	hwif->name[0]	= 'i';
	hwif->name[1]	= 'd';
	hwif->name[2]	= 'e';
	hwif->name[3]	= '0' + index;
	hwif->bus_state = BUSSTATE_ON;
	for (unit = 0; unit < MAX_DRIVES; ++unit) {
		ide_drive_t *drive = &hwif->drives[unit];

		drive->media			= ide_disk;
		drive->select.all		= (unit<<4)|0xa0;
		drive->hwif			= hwif;
		drive->ctl			= 0x08;
		drive->ready_stat		= READY_STAT;
		drive->bad_wstat		= BAD_W_STAT;
		drive->special.b.recalibrate	= 1;
		drive->special.b.set_geometry	= 1;
		drive->name[0]			= 'h';
		drive->name[1]			= 'd';
		drive->name[2]			= 'a' + (index * MAX_DRIVES) + unit;
		drive->max_failures		= IDE_DEFAULT_MAX_FAILURES;
		init_waitqueue_head(&drive->wqueue);
	}
}

/*
 * init_ide_data() sets reasonable default values into all fields
 * of all instances of the hwifs and drives, but only on the first call.
 * Subsequent calls have no effect (they don't wipe out anything).
 *
 * This routine is normally called at driver initialization time,
 * but may also be called MUCH earlier during kernel "command-line"
 * parameter processing.  As such, we cannot depend on any other parts
 * of the kernel (such as memory allocation) to be functioning yet.
 *
 * This is too bad, as otherwise we could dynamically allocate the
 * ide_drive_t structs as needed, rather than always consuming memory
 * for the max possible number (MAX_HWIFS * MAX_DRIVES) of them.
 */
#define MAGIC_COOKIE 0x12345678
static void __init init_ide_data (void)
{
	unsigned int index;
	static unsigned long magic_cookie = MAGIC_COOKIE;

	if (magic_cookie != MAGIC_COOKIE)
		return;		/* already initialized */
	magic_cookie = 0;

	/* Initialise all interface structures */
	for (index = 0; index < MAX_HWIFS; ++index)
		init_hwif_data(index);

	/* Add default hw interfaces */
	ide_init_default_hwifs();

	idebus_parameter = 0;
	system_bus_speed = 0;
}

/*
 * CompactFlash cards and their brethern pretend to be removable hard disks, except:
 *	(1) they never have a slave unit, and
 *	(2) they don't have doorlock mechanisms.
 * This test catches them, and is invoked elsewhere when setting appropriate config bits.
 *
 * FIXME: This treatment is probably applicable for *all* PCMCIA (PC CARD) devices,
 * so in linux 2.3.x we should change this to just treat all PCMCIA drives this way,
 * and get rid of the model-name tests below (too big of an interface change for 2.2.x).
 * At that time, we might also consider parameterizing the timeouts and retries,
 * since these are MUCH faster than mechanical drives.	-M.Lord
 */
int drive_is_flashcard (ide_drive_t *drive)
{
	struct hd_driveid *id = drive->id;

	if (drive->removable && id != NULL) {
		if (id->config == 0x848a) return 1;	/* CompactFlash */
		if (!strncmp(id->model, "KODAK ATA_FLASH", 15)	/* Kodak */
		 || !strncmp(id->model, "Hitachi CV", 10)	/* Hitachi */
		 || !strncmp(id->model, "SunDisk SDCFB", 13)	/* SunDisk */
		 || !strncmp(id->model, "HAGIWARA HPC", 12)	/* Hagiwara */
		 || !strncmp(id->model, "LEXAR ATA_FLASH", 15)	/* Lexar */
		 || !strncmp(id->model, "ATA_FLASH", 9))	/* Simple Tech */
		{
			return 1;	/* yes, it is a flash memory card */
		}
	}
	return 0;	/* no, it is not a flash memory card */
}

/*
 * ide_system_bus_speed() returns what we think is the system VESA/PCI
 * bus speed (in MHz).  This is used for calculating interface PIO timings.
 * The default is 40 for known PCI systems, 50 otherwise.
 * The "idebus=xx" parameter can be used to override this value.
 * The actual value to be used is computed/displayed the first time through.
 */
int ide_system_bus_speed (void)
{
	if (!system_bus_speed) {
		if (idebus_parameter)
			system_bus_speed = idebus_parameter;	/* user supplied value */
#ifdef CONFIG_PCI
		else if (pci_present())
			system_bus_speed = 33;	/* safe default value for PCI */
#endif /* CONFIG_PCI */
		else
			system_bus_speed = 50;	/* safe default value for VESA and PCI */
		printk("ide: Assuming %dMHz system bus speed for PIO modes%s\n", system_bus_speed,
			idebus_parameter ? "" : "; override with idebus=xx");
	}
	return system_bus_speed;
}

#if SUPPORT_VLB_SYNC
/*
 * Some localbus EIDE interfaces require a special access sequence
 * when using 32-bit I/O instructions to transfer data.  We call this
 * the "vlb_sync" sequence, which consists of three successive reads
 * of the sector count register location, with interrupts disabled
 * to ensure that the reads all happen together.
 */
static inline void do_vlb_sync (ide_ioreg_t port) {
	(void) inb (port);
	(void) inb (port);
	(void) inb (port);
}
#endif /* SUPPORT_VLB_SYNC */

/*
 * This is used for most PIO data transfers *from* the IDE interface
 */
void ide_input_data (ide_drive_t *drive, void *buffer, unsigned int wcount)
{
	byte io_32bit;

	/* first check if this controller has defined a special function
	 * for handling polled ide transfers
	 */

	if(HWIF(drive)->ideproc) {
		HWIF(drive)->ideproc(ideproc_ide_input_data,
				     drive, buffer, wcount);
		return;
	}

	io_32bit = drive->io_32bit;

	if (io_32bit) {
#if SUPPORT_VLB_SYNC
		if (io_32bit & 2) {
			unsigned long flags;
			__save_flags(flags);	/* local CPU only */
			__cli();		/* local CPU only */
			do_vlb_sync(IDE_NSECTOR_REG);
			insl(IDE_DATA_REG, buffer, wcount);
			__restore_flags(flags);	/* local CPU only */
		} else
#endif /* SUPPORT_VLB_SYNC */
			insl(IDE_DATA_REG, buffer, wcount);
	} else {
#if SUPPORT_SLOW_DATA_PORTS
		if (drive->slow) {
			unsigned short *ptr = (unsigned short *) buffer;
			while (wcount--) {
				*ptr++ = inw_p(IDE_DATA_REG);
				*ptr++ = inw_p(IDE_DATA_REG);
			}
		} else
#endif /* SUPPORT_SLOW_DATA_PORTS */
			insw(IDE_DATA_REG, buffer, wcount<<1);
	}
}

/*
 * This is used for most PIO data transfers *to* the IDE interface
 */
void ide_output_data (ide_drive_t *drive, void *buffer, unsigned int wcount)
{
	byte io_32bit;

	if(HWIF(drive)->ideproc) {
		HWIF(drive)->ideproc(ideproc_ide_output_data,
				     drive, buffer, wcount);
		return;
	}

	io_32bit = drive->io_32bit;

	if (io_32bit) {
#if SUPPORT_VLB_SYNC
		if (io_32bit & 2) {
			unsigned long flags;
			__save_flags(flags);	/* local CPU only */
			__cli();		/* local CPU only */
			do_vlb_sync(IDE_NSECTOR_REG);
			outsl(IDE_DATA_REG, buffer, wcount);
			__restore_flags(flags);	/* local CPU only */
		} else
#endif /* SUPPORT_VLB_SYNC */
			outsl(IDE_DATA_REG, buffer, wcount);
	} else {
#if SUPPORT_SLOW_DATA_PORTS
		if (drive->slow) {
			unsigned short *ptr = (unsigned short *) buffer;
			while (wcount--) {
				outw_p(*ptr++, IDE_DATA_REG);
				outw_p(*ptr++, IDE_DATA_REG);
			}
		} else
#endif /* SUPPORT_SLOW_DATA_PORTS */
			outsw(IDE_DATA_REG, buffer, wcount<<1);
	}
}

/*
 * The following routines are mainly used by the ATAPI drivers.
 *
 * These routines will round up any request for an odd number of bytes,
 * so if an odd bytecount is specified, be sure that there's at least one
 * extra byte allocated for the buffer.
 */
void atapi_input_bytes (ide_drive_t *drive, void *buffer, unsigned int bytecount)
{
	if(HWIF(drive)->ideproc) {
		HWIF(drive)->ideproc(ideproc_atapi_input_bytes,
				     drive, buffer, bytecount);
		return;
	}

	++bytecount;
#if defined(CONFIG_ATARI) || defined(CONFIG_Q40)
	if (MACH_IS_ATARI || MACH_IS_Q40) {
		/* Atari has a byte-swapped IDE interface */
		insw_swapw(IDE_DATA_REG, buffer, bytecount / 2);
		return;
	}
#endif /* CONFIG_ATARI */
	ide_input_data (drive, buffer, bytecount / 4);
	if ((bytecount & 0x03) >= 2)
		insw (IDE_DATA_REG, ((byte *)buffer) + (bytecount & ~0x03), 1);
}

void atapi_output_bytes (ide_drive_t *drive, void *buffer, unsigned int bytecount)
{
	if(HWIF(drive)->ideproc) {
		HWIF(drive)->ideproc(ideproc_atapi_output_bytes,
				     drive, buffer, bytecount);
		return;
	}

	++bytecount;
#if defined(CONFIG_ATARI) || defined(CONFIG_Q40)
	if (MACH_IS_ATARI || MACH_IS_Q40) {
		/* Atari has a byte-swapped IDE interface */
		outsw_swapw(IDE_DATA_REG, buffer, bytecount / 2);
		return;
	}
#endif /* CONFIG_ATARI */
	ide_output_data (drive, buffer, bytecount / 4);
	if ((bytecount & 0x03) >= 2)
		outsw (IDE_DATA_REG, ((byte *)buffer) + (bytecount & ~0x03), 1);
}

/*
 * Needed for PCI irq sharing
 */
static inline int drive_is_ready (ide_drive_t *drive)
{
	byte stat = 0;
	if (drive->waiting_for_dma)
		return HWIF(drive)->dmaproc(ide_dma_test_irq, drive);
#if 0
	udelay(1);	/* need to guarantee 400ns since last command was issued */
#endif

#ifdef CONFIG_IDEPCI_SHARE_IRQ
	/*
	 * We do a passive status test under shared PCI interrupts on
	 * cards that truly share the ATA side interrupt, but may also share
	 * an interrupt with another pci card/device.  We make no assumptions
	 * about possible isa-pnp and pci-pnp issues yet.
	 */
	if (IDE_CONTROL_REG)
		stat = GET_ALTSTAT();
	else
#endif /* CONFIG_IDEPCI_SHARE_IRQ */
	stat = GET_STAT();	/* Note: this may clear a pending IRQ!! */

	if (stat & BUSY_STAT)
		return 0;	/* drive busy:  definitely not interrupting */
	return 1;		/* drive ready: *might* be interrupting */
}

/*
 * This is our end_request replacement function.
 */
void ide_end_request (byte uptodate, ide_hwgroup_t *hwgroup)
{
	struct request *rq;
	unsigned long flags;
	ide_drive_t *drive = hwgroup->drive;

	spin_lock_irqsave(&io_request_lock, flags);
	rq = hwgroup->rq;

	/*
	 * decide whether to reenable DMA -- 3 is a random magic for now,
	 * if we DMA timeout more than 3 times, just stay in PIO
	 */
	if (drive->state == DMA_PIO_RETRY && drive->retry_pio <= 3) {
		drive->state = 0;
		hwgroup->hwif->dmaproc(ide_dma_on, drive);
	}

	if (!end_that_request_first(rq, uptodate, hwgroup->drive->name)) {
		add_blkdev_randomness(MAJOR(rq->rq_dev));
		blkdev_dequeue_request(rq);
        	hwgroup->rq = NULL;
		end_that_request_last(rq);
	}
	spin_unlock_irqrestore(&io_request_lock, flags);
}

/*
 * This should get invoked any time we exit the driver to
 * wait for an interrupt response from a drive.  handler() points
 * at the appropriate code to handle the next interrupt, and a
 * timer is started to prevent us from waiting forever in case
 * something goes wrong (see the ide_timer_expiry() handler later on).
 */
void ide_set_handler (ide_drive_t *drive, ide_handler_t *handler,
		      unsigned int timeout, ide_expiry_t *expiry)
{
	unsigned long flags;
	ide_hwgroup_t *hwgroup = HWGROUP(drive);

	spin_lock_irqsave(&io_request_lock, flags);
	if (hwgroup->handler != NULL) {
		printk("%s: ide_set_handler: handler not null; old=%p, new=%p\n",
			drive->name, hwgroup->handler, handler);
	}
	hwgroup->handler	= handler;
	hwgroup->expiry		= expiry;
	hwgroup->timer.expires	= jiffies + timeout;
	add_timer(&hwgroup->timer);
	spin_unlock_irqrestore(&io_request_lock, flags);
}

/*
 * current_capacity() returns the capacity (in sectors) of a drive
 * according to its current geometry/LBA settings.
 */
unsigned long current_capacity (ide_drive_t *drive)
{
	if (!drive->present)
		return 0;
	if (drive->driver != NULL)
		return DRIVER(drive)->capacity(drive);
	return 0;
}

extern struct block_device_operations ide_fops[];
/*
 * ide_geninit() is called exactly *once* for each interface.
 */
void ide_geninit (ide_hwif_t *hwif)
{
	unsigned int unit;
	struct gendisk *gd = hwif->gd;

	for (unit = 0; unit < MAX_DRIVES; ++unit) {
		ide_drive_t *drive = &hwif->drives[unit];

		if (!drive->present)
			continue;
		if (drive->media!=ide_disk && drive->media!=ide_floppy)
			continue;
		register_disk(gd,MKDEV(hwif->major,unit<<PARTN_BITS),
#ifdef CONFIG_BLK_DEV_ISAPNP
			(drive->forced_geom && drive->noprobe) ? 1 :
#endif /* CONFIG_BLK_DEV_ISAPNP */
			1<<PARTN_BITS, ide_fops,
			current_capacity(drive));
	}
}

static ide_startstop_t do_reset1 (ide_drive_t *, int);		/* needed below */

/*
 * atapi_reset_pollfunc() gets invoked to poll the interface for completion every 50ms
 * during an atapi drive reset operation. If the drive has not yet responded,
 * and we have not yet hit our maximum waiting time, then the timer is restarted
 * for another 50ms.
 */
static ide_startstop_t atapi_reset_pollfunc (ide_drive_t *drive)
{
	ide_hwgroup_t *hwgroup = HWGROUP(drive);
	byte stat;

	SELECT_DRIVE(HWIF(drive),drive);
	udelay (10);

	if (OK_STAT(stat=GET_STAT(), 0, BUSY_STAT)) {
		printk("%s: ATAPI reset complete\n", drive->name);
	} else {
		if (0 < (signed long)(hwgroup->poll_timeout - jiffies)) {
			ide_set_handler (drive, &atapi_reset_pollfunc, HZ/20, NULL);
			return ide_started;	/* continue polling */
		}
		hwgroup->poll_timeout = 0;	/* end of polling */
		printk("%s: ATAPI reset timed-out, status=0x%02x\n", drive->name, stat);
		return do_reset1 (drive, 1);	/* do it the old fashioned way */
	}
	hwgroup->poll_timeout = 0;	/* done polling */
	return ide_stopped;
}

/*
 * reset_pollfunc() gets invoked to poll the interface for completion every 50ms
 * during an ide reset operation. If the drives have not yet responded,
 * and we have not yet hit our maximum waiting time, then the timer is restarted
 * for another 50ms.
 */
static ide_startstop_t reset_pollfunc (ide_drive_t *drive)
{
	ide_hwgroup_t *hwgroup = HWGROUP(drive);
	ide_hwif_t *hwif = HWIF(drive);
	byte tmp;

	if (!OK_STAT(tmp=GET_STAT(), 0, BUSY_STAT)) {
		if (0 < (signed long)(hwgroup->poll_timeout - jiffies)) {
			ide_set_handler (drive, &reset_pollfunc, HZ/20, NULL);
			return ide_started;	/* continue polling */
		}
		printk("%s: reset timed-out, status=0x%02x\n", hwif->name, tmp);
		drive->failures++;
	} else  {
		printk("%s: reset: ", hwif->name);
		if ((tmp = GET_ERR()) == 1) {
			printk("success\n");
			drive->failures = 0;
		} else {
			drive->failures++;
#if FANCY_STATUS_DUMPS
			printk("master: ");
			switch (tmp & 0x7f) {
				case 1: printk("passed");
					break;
				case 2: printk("formatter device error");
					break;
				case 3: printk("sector buffer error");
					break;
				case 4: printk("ECC circuitry error");
					break;
				case 5: printk("controlling MPU error");
					break;
				default:printk("error (0x%02x?)", tmp);
			}
			if (tmp & 0x80)
				printk("; slave: failed");
			printk("\n");
#else
			printk("failed\n");
#endif /* FANCY_STATUS_DUMPS */
		}
	}
	hwgroup->poll_timeout = 0;	/* done polling */
	return ide_stopped;
}

static void check_dma_crc (ide_drive_t *drive)
{
	if (drive->crc_count) {
		(void) HWIF(drive)->dmaproc(ide_dma_off_quietly, drive);
		if ((HWIF(drive)->speedproc) != NULL)
			HWIF(drive)->speedproc(drive, ide_auto_reduce_xfer(drive));
		if (drive->current_speed >= XFER_SW_DMA_0)
			(void) HWIF(drive)->dmaproc(ide_dma_on, drive);
	} else {
		(void) HWIF(drive)->dmaproc(ide_dma_off, drive);
	}
}

static void pre_reset (ide_drive_t *drive)
{
	if (drive->driver != NULL)
		DRIVER(drive)->pre_reset(drive);

	if (!drive->keep_settings) {
		if (drive->using_dma) {
			check_dma_crc(drive);
		} else {
			drive->unmask = 0;
			drive->io_32bit = 0;
		}
		return;
	}
	if (drive->using_dma)
		check_dma_crc(drive);
}

/*
 * do_reset1() attempts to recover a confused drive by resetting it.
 * Unfortunately, resetting a disk drive actually resets all devices on
 * the same interface, so it can really be thought of as resetting the
 * interface rather than resetting the drive.
 *
 * ATAPI devices have their own reset mechanism which allows them to be
 * individually reset without clobbering other devices on the same interface.
 *
 * Unfortunately, the IDE interface does not generate an interrupt to let
 * us know when the reset operation has finished, so we must poll for this.
 * Equally poor, though, is the fact that this may a very long time to complete,
 * (up to 30 seconds worstcase).  So, instead of busy-waiting here for it,
 * we set a timer to poll at 50ms intervals.
 */
static ide_startstop_t do_reset1 (ide_drive_t *drive, int do_not_try_atapi)
{
	unsigned int unit;
	unsigned long flags;
	ide_hwif_t *hwif = HWIF(drive);
	ide_hwgroup_t *hwgroup = HWGROUP(drive);

	__save_flags(flags);	/* local CPU only */
	__cli();		/* local CPU only */

	/* For an ATAPI device, first try an ATAPI SRST. */
	if (drive->media != ide_disk && !do_not_try_atapi) {
		pre_reset(drive);
		SELECT_DRIVE(hwif,drive);
		udelay (20);
		OUT_BYTE (WIN_SRST, IDE_COMMAND_REG);
		hwgroup->poll_timeout = jiffies + WAIT_WORSTCASE;
		ide_set_handler (drive, &atapi_reset_pollfunc, HZ/20, NULL);
		__restore_flags (flags);	/* local CPU only */
		return ide_started;
	}

	/*
	 * First, reset any device state data we were maintaining
	 * for any of the drives on this interface.
	 */
	for (unit = 0; unit < MAX_DRIVES; ++unit)
		pre_reset(&hwif->drives[unit]);

#if OK_TO_RESET_CONTROLLER
	if (!IDE_CONTROL_REG) {
		__restore_flags(flags);
		return ide_stopped;
	}
	/*
	 * Note that we also set nIEN while resetting the device,
	 * to mask unwanted interrupts from the interface during the reset.
	 * However, due to the design of PC hardware, this will cause an
	 * immediate interrupt due to the edge transition it produces.
	 * This single interrupt gives us a "fast poll" for drives that
	 * recover from reset very quickly, saving us the first 50ms wait time.
	 */
	OUT_BYTE(drive->ctl|6,IDE_CONTROL_REG);	/* set SRST and nIEN */
	udelay(10);			/* more than enough time */
	OUT_BYTE(drive->ctl|2,IDE_CONTROL_REG);	/* clear SRST, leave nIEN */
	udelay(10);			/* more than enough time */
	hwgroup->poll_timeout = jiffies + WAIT_WORSTCASE;
	ide_set_handler (drive, &reset_pollfunc, HZ/20, NULL);

	/*
	 * Some weird controller like resetting themselves to a strange
	 * state when the disks are reset this way. At least, the Winbond
	 * 553 documentation says that
	 */
	if (hwif->resetproc != NULL)
		hwif->resetproc(drive);

#endif	/* OK_TO_RESET_CONTROLLER */

	__restore_flags (flags);	/* local CPU only */
	return ide_started;
}

/*
 * ide_do_reset() is the entry point to the drive/interface reset code.
 */
ide_startstop_t ide_do_reset (ide_drive_t *drive)
{
	return do_reset1 (drive, 0);
}

/*
 * Clean up after success/failure of an explicit drive cmd
 */
void ide_end_drive_cmd (ide_drive_t *drive, byte stat, byte err)
{
	unsigned long flags;
	struct request *rq;

	spin_lock_irqsave(&io_request_lock, flags);
	rq = HWGROUP(drive)->rq;
	spin_unlock_irqrestore(&io_request_lock, flags);

	if (rq->cmd == IDE_DRIVE_CMD) {
		byte *args = (byte *) rq->buffer;
		rq->errors = !OK_STAT(stat,READY_STAT,BAD_STAT);
		if (args) {
			args[0] = stat;
			args[1] = err;
			args[2] = IN_BYTE(IDE_NSECTOR_REG);
		}
	} else if (rq->cmd == IDE_DRIVE_TASK) {
		byte *args = (byte *) rq->buffer;
		rq->errors = !OK_STAT(stat,READY_STAT,BAD_STAT);
		if (args) {
			args[0] = stat;
			args[1] = err;
			args[2] = IN_BYTE(IDE_NSECTOR_REG);
			args[3] = IN_BYTE(IDE_SECTOR_REG);
			args[4] = IN_BYTE(IDE_LCYL_REG);
			args[5] = IN_BYTE(IDE_HCYL_REG);
			args[6] = IN_BYTE(IDE_SELECT_REG);
		}
	}
	spin_lock_irqsave(&io_request_lock, flags);
	blkdev_dequeue_request(rq);
	HWGROUP(drive)->rq = NULL;
	end_that_request_last(rq);
	spin_unlock_irqrestore(&io_request_lock, flags);
}

/*
 * Error reporting, in human readable form (luxurious, but a memory hog).
 */
byte ide_dump_status (ide_drive_t *drive, const char *msg, byte stat)
{
	unsigned long flags;
	byte err = 0;

	__save_flags (flags);	/* local CPU only */
	ide__sti();		/* local CPU only */
	printk("%s: %s: status=0x%02x", drive->name, msg, stat);
#if FANCY_STATUS_DUMPS
	printk(" { ");
	if (stat & BUSY_STAT)
		printk("Busy ");
	else {
		if (stat & READY_STAT)	printk("DriveReady ");
		if (stat & WRERR_STAT)	printk("DeviceFault ");
		if (stat & SEEK_STAT)	printk("SeekComplete ");
		if (stat & DRQ_STAT)	printk("DataRequest ");
		if (stat & ECC_STAT)	printk("CorrectedError ");
		if (stat & INDEX_STAT)	printk("Index ");
		if (stat & ERR_STAT)	printk("Error ");
	}
	printk("}");
#endif	/* FANCY_STATUS_DUMPS */
	printk("\n");
	if ((stat & (BUSY_STAT|ERR_STAT)) == ERR_STAT) {
		err = GET_ERR();
		printk("%s: %s: error=0x%02x", drive->name, msg, err);
#if FANCY_STATUS_DUMPS
		if (drive->media == ide_disk) {
			printk(" { ");
			if (err & ABRT_ERR)	printk("DriveStatusError ");
			if (err & ICRC_ERR)	printk("%s", (err & ABRT_ERR) ? "BadCRC " : "BadSector ");
			if (err & ECC_ERR)	printk("UncorrectableError ");
			if (err & ID_ERR)	printk("SectorIdNotFound ");
			if (err & TRK0_ERR)	printk("TrackZeroNotFound ");
			if (err & MARK_ERR)	printk("AddrMarkNotFound ");
			printk("}");
			if ((err & (BBD_ERR | ABRT_ERR)) == BBD_ERR || (err & (ECC_ERR|ID_ERR|MARK_ERR))) {
				byte cur = IN_BYTE(IDE_SELECT_REG);
				if (cur & 0x40) {	/* using LBA? */
					printk(", LBAsect=%ld", (unsigned long)
					 ((cur&0xf)<<24)
					 |(IN_BYTE(IDE_HCYL_REG)<<16)
					 |(IN_BYTE(IDE_LCYL_REG)<<8)
					 | IN_BYTE(IDE_SECTOR_REG));
				} else {
					printk(", CHS=%d/%d/%d",
					 (IN_BYTE(IDE_HCYL_REG)<<8) +
					  IN_BYTE(IDE_LCYL_REG),
					  cur & 0xf,
					  IN_BYTE(IDE_SECTOR_REG));
				}
				if (HWGROUP(drive) && HWGROUP(drive)->rq)
					printk(", sector=%ld", HWGROUP(drive)->rq->sector);
			}
		}
#endif	/* FANCY_STATUS_DUMPS */
		printk("\n");
	}
	__restore_flags (flags);	/* local CPU only */
	return err;
}

/*
 * try_to_flush_leftover_data() is invoked in response to a drive
 * unexpectedly having its DRQ_STAT bit set.  As an alternative to
 * resetting the drive, this routine tries to clear the condition
 * by read a sector's worth of data from the drive.  Of course,
 * this may not help if the drive is *waiting* for data from *us*.
 */
static void try_to_flush_leftover_data (ide_drive_t *drive)
{
	int i = (drive->mult_count ? drive->mult_count : 1) * SECTOR_WORDS;

	if (drive->media != ide_disk)
		return;
	while (i > 0) {
		u32 buffer[16];
		unsigned int wcount = (i > 16) ? 16 : i;
		i -= wcount;
		ide_input_data (drive, buffer, wcount);
	}
}

/*
 * ide_error() takes action based on the error returned by the drive.
 */
ide_startstop_t ide_error (ide_drive_t *drive, const char *msg, byte stat)
{
	struct request *rq;
	byte err;

	err = ide_dump_status(drive, msg, stat);
	if (drive == NULL || (rq = HWGROUP(drive)->rq) == NULL)
		return ide_stopped;
	/* retry only "normal" I/O: */
	if (rq->cmd == IDE_DRIVE_CMD || rq->cmd == IDE_DRIVE_TASK) {
		rq->errors = 1;
		ide_end_drive_cmd(drive, stat, err);
		return ide_stopped;
	}
	if (stat & BUSY_STAT || ((stat & WRERR_STAT) && !drive->nowerr)) { /* other bits are useless when BUSY */
		rq->errors |= ERROR_RESET;
	} else {
		if (drive->media == ide_disk && (stat & ERR_STAT)) {
			/* err has different meaning on cdrom and tape */
			if (err == ABRT_ERR) {
				if (drive->select.b.lba && IN_BYTE(IDE_COMMAND_REG) == WIN_SPECIFY)
					return ide_stopped; /* some newer drives don't support WIN_SPECIFY */
			} else if ((err & (ABRT_ERR | ICRC_ERR)) == (ABRT_ERR | ICRC_ERR)) {
				drive->crc_count++; /* UDMA crc error -- just retry the operation */
			} else if (err & (BBD_ERR | ECC_ERR))	/* retries won't help these */
				rq->errors = ERROR_MAX;
			else if (err & TRK0_ERR)	/* help it find track zero */
				rq->errors |= ERROR_RECAL;
		}
		if ((stat & DRQ_STAT) && rq->cmd != WRITE)
			try_to_flush_leftover_data(drive);
	}
	if (GET_STAT() & (BUSY_STAT|DRQ_STAT))
		OUT_BYTE(WIN_IDLEIMMEDIATE,IDE_COMMAND_REG);	/* force an abort */

	if (rq->errors >= ERROR_MAX) {
		if (drive->driver != NULL)
			DRIVER(drive)->end_request(0, HWGROUP(drive));
		else
	 		ide_end_request(0, HWGROUP(drive));
	} else {
		if ((rq->errors & ERROR_RESET) == ERROR_RESET) {
			++rq->errors;
			return ide_do_reset(drive);
		}
		if ((rq->errors & ERROR_RECAL) == ERROR_RECAL)
			drive->special.b.recalibrate = 1;
		++rq->errors;
	}
	return ide_stopped;
}

/*
 * Issue a simple drive command
 * The drive must be selected beforehand.
 */
void ide_cmd (ide_drive_t *drive, byte cmd, byte nsect, ide_handler_t *handler)
{
	ide_set_handler (drive, handler, WAIT_CMD, NULL);
	if (IDE_CONTROL_REG)
		OUT_BYTE(drive->ctl,IDE_CONTROL_REG);	/* clear nIEN */
	SELECT_MASK(HWIF(drive),drive,0);
	OUT_BYTE(nsect,IDE_NSECTOR_REG);
	OUT_BYTE(cmd,IDE_COMMAND_REG);
}

/*
 * drive_cmd_intr() is invoked on completion of a special DRIVE_CMD.
 */
static ide_startstop_t drive_cmd_intr (ide_drive_t *drive)
{
	struct request *rq = HWGROUP(drive)->rq;
	byte *args = (byte *) rq->buffer;
	byte stat = GET_STAT();
	int retries = 10;

	ide__sti();	/* local CPU only */
	if ((stat & DRQ_STAT) && args && args[3]) {
		byte io_32bit = drive->io_32bit;
		drive->io_32bit = 0;
		ide_input_data(drive, &args[4], args[3] * SECTOR_WORDS);
		drive->io_32bit = io_32bit;
		while (((stat = GET_STAT()) & BUSY_STAT) && retries--)
			udelay(100);
	}

	if (!OK_STAT(stat, READY_STAT, BAD_STAT))
		return ide_error(drive, "drive_cmd", stat); /* calls ide_end_drive_cmd */
	ide_end_drive_cmd (drive, stat, GET_ERR());
	return ide_stopped;
}

/*
 * do_special() is used to issue WIN_SPECIFY, WIN_RESTORE, and WIN_SETMULT
 * commands to a drive.  It used to do much more, but has been scaled back.
 */
static ide_startstop_t do_special (ide_drive_t *drive)
{
	special_t *s = &drive->special;

#ifdef DEBUG
	printk("%s: do_special: 0x%02x\n", drive->name, s->all);
#endif
	if (s->b.set_tune) {
		ide_tuneproc_t *tuneproc = HWIF(drive)->tuneproc;
		s->b.set_tune = 0;
		if (tuneproc != NULL)
			tuneproc(drive, drive->tune_req);
	} else if (drive->driver != NULL) {
		return DRIVER(drive)->special(drive);
	} else if (s->all) {
		printk("%s: bad special flag: 0x%02x\n", drive->name, s->all);
		s->all = 0;
	}
	return ide_stopped;
}

/*
 * This routine busy-waits for the drive status to be not "busy".
 * It then checks the status for all of the "good" bits and none
 * of the "bad" bits, and if all is okay it returns 0.  All other
 * cases return 1 after invoking ide_error() -- caller should just return.
 *
 * This routine should get fixed to not hog the cpu during extra long waits..
 * That could be done by busy-waiting for the first jiffy or two, and then
 * setting a timer to wake up at half second intervals thereafter,
 * until timeout is achieved, before timing out.
 */
int ide_wait_stat (ide_startstop_t *startstop, ide_drive_t *drive, byte good, byte bad, unsigned long timeout) {
	byte stat;
	int i;
	unsigned long flags;
 
	/* bail early if we've exceeded max_failures */
	if (drive->max_failures && (drive->failures > drive->max_failures)) {
		*startstop = ide_stopped;
		return 1;
	}

	udelay(1);	/* spec allows drive 400ns to assert "BUSY" */
	if ((stat = GET_STAT()) & BUSY_STAT) {
		__save_flags(flags);	/* local CPU only */
		ide__sti();		/* local CPU only */
		timeout += jiffies;
		while ((stat = GET_STAT()) & BUSY_STAT) {
			if (0 < (signed long)(jiffies - timeout)) {
				__restore_flags(flags);	/* local CPU only */
				*startstop = ide_error(drive, "status timeout", stat);
				return 1;
			}
		}
		__restore_flags(flags);	/* local CPU only */
	}
	/*
	 * Allow status to settle, then read it again.
	 * A few rare drives vastly violate the 400ns spec here,
	 * so we'll wait up to 10usec for a "good" status
	 * rather than expensively fail things immediately.
	 * This fix courtesy of Matthew Faupel & Niccolo Rigacci.
	 */
	for (i = 0; i < 10; i++) {
		udelay(1);
		if (OK_STAT((stat = GET_STAT()), good, bad))
			return 0;
	}
	*startstop = ide_error(drive, "status error", stat);
	return 1;
}

/*
 * execute_drive_cmd() issues a special drive command,
 * usually initiated by ioctl() from the external hdparm program.
 */
static ide_startstop_t execute_drive_cmd (ide_drive_t *drive, struct request *rq)
{
	byte *args = rq->buffer;
	if (args && rq->cmd == IDE_DRIVE_TASK) {
		byte sel;
#ifdef DEBUG
		printk("%s: DRIVE_TASK_CMD data=x%02x cmd=0x%02x fr=0x%02x ns=0x%02x sc=0x%02x lcyl=0x%02x hcyl=0x%02x sel=0x%02x\n",
			drive->name,
			args[0], args[1], args[2], args[3],
			args[4], args[5], args[6], args[7]);
#endif
		OUT_BYTE(args[1], IDE_FEATURE_REG);
		OUT_BYTE(args[3], IDE_SECTOR_REG);
		OUT_BYTE(args[4], IDE_LCYL_REG);
		OUT_BYTE(args[5], IDE_HCYL_REG);
		sel = (args[6] & ~0x10);
		if (drive->select.b.unit)
			sel |= 0x10;
		OUT_BYTE(sel, IDE_SELECT_REG);
		ide_cmd(drive, args[0], args[2], &drive_cmd_intr);
		return ide_started;
	} else if (args) {
#ifdef DEBUG
		printk("%s: DRIVE_CMD cmd=0x%02x sc=0x%02x fr=0x%02x xx=0x%02x\n",
		 drive->name, args[0], args[1], args[2], args[3]);
#endif
		if (args[0] == WIN_SMART) {
			OUT_BYTE(0x4f, IDE_LCYL_REG);
			OUT_BYTE(0xc2, IDE_HCYL_REG);
			OUT_BYTE(args[2],IDE_FEATURE_REG);
			OUT_BYTE(args[1],IDE_SECTOR_REG);
			ide_cmd(drive, args[0], args[3], &drive_cmd_intr);
			return ide_started;
		}
		OUT_BYTE(args[2],IDE_FEATURE_REG);
		ide_cmd(drive, args[0], args[1], &drive_cmd_intr);
		return ide_started;
	} else {
		/*
		 * NULL is actually a valid way of waiting for
		 * all current requests to be flushed from the queue.
		 */
#ifdef DEBUG
		printk("%s: DRIVE_CMD (null)\n", drive->name);
#endif
		ide_end_drive_cmd(drive, GET_STAT(), GET_ERR());
		return ide_stopped;
	}
}

/*
 * start_request() initiates handling of a new I/O request
 */
static ide_startstop_t start_request (ide_drive_t *drive)
{
	ide_startstop_t startstop;
	unsigned long block, blockend;
	struct request *rq = blkdev_entry_next_request(&drive->queue.queue_head);
	unsigned int minor = MINOR(rq->rq_dev), unit = minor >> PARTN_BITS;
	ide_hwif_t *hwif = HWIF(drive);

#ifdef DEBUG
	printk("%s: start_request: current=0x%08lx\n", hwif->name, (unsigned long) rq);
#endif
	/* bail early if we've exceeded max_failures */
	if (drive->max_failures && (drive->failures > drive->max_failures)) {
		goto kill_rq;
	}

	if (unit >= MAX_DRIVES) {
		printk("%s: bad device number: %s\n", hwif->name, kdevname(rq->rq_dev));
		goto kill_rq;
	}
#ifdef DEBUG
	if (rq->bh && !buffer_locked(rq->bh)) {
		printk("%s: block not locked\n", drive->name);
		goto kill_rq;
	}
#endif
	block    = rq->sector;
	blockend = block + rq->nr_sectors;

	if ((rq->cmd == READ || rq->cmd == WRITE) &&
	    (drive->media == ide_disk || drive->media == ide_floppy)) {
		if ((blockend < block) || (blockend > drive->part[minor&PARTN_MASK].nr_sects)) {
			printk("%s%c: bad access: block=%ld, count=%ld\n", drive->name,
			 (minor&PARTN_MASK)?'0'+(minor&PARTN_MASK):' ', block, rq->nr_sectors);
			goto kill_rq;
		}
		block += drive->part[minor&PARTN_MASK].start_sect + drive->sect0;
	}
	/* Yecch - this will shift the entire interval,
	   possibly killing some innocent following sector */
	if (block == 0 && drive->remap_0_to_1 == 1)
		block = 1;  /* redirect MBR access to EZ-Drive partn table */

#if (DISK_RECOVERY_TIME > 0)
	while ((read_timer() - hwif->last_time) < DISK_RECOVERY_TIME);
#endif

	SELECT_DRIVE(hwif, drive);
	if (ide_wait_stat(&startstop, drive, drive->ready_stat, BUSY_STAT|DRQ_STAT, WAIT_READY)) {
		printk("%s: drive not ready for command\n", drive->name);
		return startstop;
	}
	if (!drive->special.all) {
		if (rq->cmd == IDE_DRIVE_CMD || rq->cmd == IDE_DRIVE_TASK) {
			return execute_drive_cmd(drive, rq);
		}
		if (drive->driver != NULL) {
			return (DRIVER(drive)->do_request(drive, rq, block));
		}
		printk("%s: media type %d not supported\n", drive->name, drive->media);
		goto kill_rq;
	}
	return do_special(drive);
kill_rq:
	if (drive->driver != NULL)
		DRIVER(drive)->end_request(0, HWGROUP(drive));
	else
		ide_end_request(0, HWGROUP(drive));
	return ide_stopped;
}

ide_startstop_t restart_request (ide_drive_t *drive)
{
	ide_hwgroup_t *hwgroup = HWGROUP(drive);
	unsigned long flags;

	spin_lock_irqsave(&io_request_lock, flags);
	hwgroup->handler = NULL;
	del_timer(&hwgroup->timer);
	spin_unlock_irqrestore(&io_request_lock, flags);

	return start_request(drive);
}

/*
 * ide_stall_queue() can be used by a drive to give excess bandwidth back
 * to the hwgroup by sleeping for timeout jiffies.
 */
void ide_stall_queue (ide_drive_t *drive, unsigned long timeout)
{
	if (timeout > WAIT_WORSTCASE)
		timeout = WAIT_WORSTCASE;
	drive->sleep = timeout + jiffies;
}

#define WAKEUP(drive)	((drive)->service_start + 2 * (drive)->service_time)

/*
 * choose_drive() selects the next drive which will be serviced.
 */
static inline ide_drive_t *choose_drive (ide_hwgroup_t *hwgroup)
{
	ide_drive_t *drive, *best;

repeat:	
	best = NULL;
	drive = hwgroup->drive;
	do {
		if (!list_empty(&drive->queue.queue_head) && (!drive->sleep || 0 <= (signed long)(jiffies - drive->sleep))) {
			if (!best
			 || (drive->sleep && (!best->sleep || 0 < (signed long)(best->sleep - drive->sleep)))
			 || (!best->sleep && 0 < (signed long)(WAKEUP(best) - WAKEUP(drive))))
			{
				if( !drive->queue.plugged )
					best = drive;
			}
		}
	} while ((drive = drive->next) != hwgroup->drive);
	if (best && best->nice1 && !best->sleep && best != hwgroup->drive && best->service_time > WAIT_MIN_SLEEP) {
		long t = (signed long)(WAKEUP(best) - jiffies);
		if (t >= WAIT_MIN_SLEEP) {
			/*
			 * We *may* have some time to spare, but first let's see if
			 * someone can potentially benefit from our nice mood today..
			 */
			drive = best->next;
			do {
				if (!drive->sleep
				 && 0 < (signed long)(WAKEUP(drive) - (jiffies - best->service_time))
				 && 0 < (signed long)((jiffies + t) - WAKEUP(drive)))
				{
					ide_stall_queue(best, IDE_MIN(t, 10 * WAIT_MIN_SLEEP));
					goto repeat;
				}
			} while ((drive = drive->next) != best);
		}
	}
	return best;
}

/*
 * Issue a new request to a drive from hwgroup
 * Caller must have already done spin_lock_irqsave(&io_request_lock, ..);
 *
 * A hwgroup is a serialized group of IDE interfaces.  Usually there is
 * exactly one hwif (interface) per hwgroup, but buggy controllers (eg. CMD640)
 * may have both interfaces in a single hwgroup to "serialize" access.
 * Or possibly multiple ISA interfaces can share a common IRQ by being grouped
 * together into one hwgroup for serialized access.
 *
 * Note also that several hwgroups can end up sharing a single IRQ,
 * possibly along with many other devices.  This is especially common in
 * PCI-based systems with off-board IDE controller cards.
 *
 * The IDE driver uses the single global io_request_lock spinlock to protect
 * access to the request queues, and to protect the hwgroup->busy flag.
 *
 * The first thread into the driver for a particular hwgroup sets the
 * hwgroup->busy flag to indicate that this hwgroup is now active,
 * and then initiates processing of the top request from the request queue.
 *
 * Other threads attempting entry notice the busy setting, and will simply
 * queue their new requests and exit immediately.  Note that hwgroup->busy
 * remains set even when the driver is merely awaiting the next interrupt.
 * Thus, the meaning is "this hwgroup is busy processing a request".
 *
 * When processing of a request completes, the completing thread or IRQ-handler
 * will start the next request from the queue.  If no more work remains,
 * the driver will clear the hwgroup->busy flag and exit.
 *
 * The io_request_lock (spinlock) is used to protect all access to the
 * hwgroup->busy flag, but is otherwise not needed for most processing in
 * the driver.  This makes the driver much more friendlier to shared IRQs
 * than previous designs, while remaining 100% (?) SMP safe and capable.
 */
/* --BenH: made non-static as ide-pmac.c uses it to kick the hwgroup back
 *         into life on wakeup from machine sleep.
 */ 
void ide_do_request(ide_hwgroup_t *hwgroup, int masked_irq)
{
	ide_drive_t	*drive;
	ide_hwif_t	*hwif;
	ide_startstop_t	startstop;

	printk("in ide_do_request\n");
	return;

	ide_get_lock(&ide_lock, ide_intr, hwgroup);	/* for atari only: POSSIBLY BROKEN HERE(?) */

	__cli();	/* necessary paranoia: ensure IRQs are masked on local CPU */

	while (!hwgroup->busy) {
		hwgroup->busy = 1;
		drive = choose_drive(hwgroup);
		if (drive == NULL) {
			unsigned long sleep = 0;
			hwgroup->rq = NULL;
			drive = hwgroup->drive;
			do {
				if (drive->sleep && (!sleep || 0 < (signed long)(sleep - drive->sleep)))
					sleep = drive->sleep;
			} while ((drive = drive->next) != hwgroup->drive);
			if (sleep) {
				/*
				 * Take a short snooze, and then wake up this hwgroup again.
				 * This gives other hwgroups on the same a chance to
				 * play fairly with us, just in case there are big differences
				 * in relative throughputs.. don't want to hog the cpu too much.
				 */
				if (0 < (signed long)(jiffies + WAIT_MIN_SLEEP - sleep)) 
					sleep = jiffies + WAIT_MIN_SLEEP;
#if 1
				if (timer_pending(&hwgroup->timer))
					printk("ide_set_handler: timer already active\n");
#endif
				hwgroup->sleeping = 1;	/* so that ide_timer_expiry knows what to do */
				mod_timer(&hwgroup->timer, sleep);
				/* we purposely leave hwgroup->busy==1 while sleeping */
			} else {
				/* Ugly, but how can we sleep for the lock otherwise? perhaps from tq_disk? */
				ide_release_lock(&ide_lock);	/* for atari only */
				hwgroup->busy = 0;
			}
			return;		/* no more work for this hwgroup (for now) */
		}
		hwif = HWIF(drive);
		if (hwgroup->hwif->sharing_irq && hwif != hwgroup->hwif && hwif->io_ports[IDE_CONTROL_OFFSET]) {
			/* set nIEN for previous hwif */
			SELECT_INTERRUPT(hwif, drive);
		}
		hwgroup->hwif = hwif;
		hwgroup->drive = drive;
		drive->sleep = 0;
		drive->service_start = jiffies;

		if ( drive->queue.plugged )	/* paranoia */
			printk("%s: Huh? nuking plugged queue\n", drive->name);
		hwgroup->rq = blkdev_entry_next_request(&drive->queue.queue_head);
		/*
		 * Some systems have trouble with IDE IRQs arriving while
		 * the driver is still setting things up.  So, here we disable
		 * the IRQ used by this interface while the request is being started.
		 * This may look bad at first, but pretty much the same thing
		 * happens anyway when any interrupt comes in, IDE or otherwise
		 *  -- the kernel masks the IRQ while it is being handled.
		 */
		if (masked_irq && hwif->irq != masked_irq)
			disable_irq_nosync(hwif->irq);
		spin_unlock(&io_request_lock);
		ide__sti();	/* allow other IRQs while we start this request */
		startstop = start_request(drive);
		spin_lock_irq(&io_request_lock);
		if (masked_irq && hwif->irq != masked_irq)
			enable_irq(hwif->irq);
		if (startstop == ide_stopped)
			hwgroup->busy = 0;
	}
}

/*
 * ide_get_queue() returns the queue which corresponds to a given device.
 */
request_queue_t *ide_get_queue (kdev_t dev)
{
	ide_hwif_t *hwif = (ide_hwif_t *)blk_dev[MAJOR(dev)].data;

	return &hwif->drives[DEVICE_NR(dev) & 1].queue;
}

/*
 * Passes the stuff to ide_do_request
 */
void do_ide_request(request_queue_t *q)
{
	ide_do_request(q->queuedata, 0);
}

/*
 * un-busy the hwgroup etc, and clear any pending DMA status. we want to
 * retry the current request in pio mode instead of risking tossing it
 * all away
 */
void ide_dma_timeout_retry(ide_drive_t *drive)
{
	ide_hwif_t *hwif = HWIF(drive);
	struct request *rq;

	/*
	 * end current dma transaction
	 */
	(void) hwif->dmaproc(ide_dma_end, drive);

	/*
	 * complain a little, later we might remove some of this verbosity
	 */
	printk("%s: timeout waiting for DMA\n", drive->name);
	(void) hwif->dmaproc(ide_dma_timeout, drive);

	/*
	 * disable dma for now, but remember that we did so because of
	 * a timeout -- we'll reenable after we finish this next request
	 * (or rather the first chunk of it) in pio.
	 */
	drive->retry_pio++;
	drive->state = DMA_PIO_RETRY;
	(void) hwif->dmaproc(ide_dma_off_quietly, drive);

	/*
	 * un-busy drive etc (hwgroup->busy is cleared on return) and
	 * make sure request is sane
	 */
	rq = HWGROUP(drive)->rq;
	HWGROUP(drive)->rq = NULL;

	rq->errors = 0;
	rq->sector = rq->bh->b_rsector;
	rq->current_nr_sectors = rq->bh->b_size >> 9;
	rq->buffer = rq->bh->b_data;
}

/*
 * ide_timer_expiry() is our timeout function for all drive operations.
 * But note that it can also be invoked as a result of a "sleep" operation
 * triggered by the mod_timer() call in ide_do_request.
 */
void ide_timer_expiry (unsigned long data)
{
	ide_hwgroup_t	*hwgroup = (ide_hwgroup_t *) data;
	ide_handler_t	*handler;
	ide_expiry_t	*expiry;
 	unsigned long	flags;
	unsigned long	wait;

	spin_lock_irqsave(&io_request_lock, flags);
	del_timer(&hwgroup->timer);

	if ((handler = hwgroup->handler) == NULL) {
		/*
		 * Either a marginal timeout occurred
		 * (got the interrupt just as timer expired),
		 * or we were "sleeping" to give other devices a chance.
		 * Either way, we don't really want to complain about anything.
		 */
		if (hwgroup->sleeping) {
			hwgroup->sleeping = 0;
			hwgroup->busy = 0;
		}
	} else {
		ide_drive_t *drive = hwgroup->drive;
		if (!drive) {
			printk("ide_timer_expiry: hwgroup->drive was NULL\n");
			hwgroup->handler = NULL;
		} else {
			ide_hwif_t *hwif;
			ide_startstop_t startstop;
			if (!hwgroup->busy) {
				hwgroup->busy = 1;	/* paranoia */
				printk("%s: ide_timer_expiry: hwgroup->busy was 0 ??\n", drive->name);
			}
			if ((expiry = hwgroup->expiry) != NULL) {
				/* continue */
				if ((wait = expiry(drive)) != 0) {
					/* reset timer */
					hwgroup->timer.expires  = jiffies + wait;
					add_timer(&hwgroup->timer);
					spin_unlock_irqrestore(&io_request_lock, flags);
					return;
				}
			}
			hwgroup->handler = NULL;
			/*
			 * We need to simulate a real interrupt when invoking
			 * the handler() function, which means we need to globally
			 * mask the specific IRQ:
			 */
			spin_unlock(&io_request_lock);
			hwif  = HWIF(drive);
#if DISABLE_IRQ_NOSYNC
			disable_irq_nosync(hwif->irq);
#else
			disable_irq(hwif->irq);	/* disable_irq_nosync ?? */
#endif /* DISABLE_IRQ_NOSYNC */
			__cli();	/* local CPU only, as if we were handling an interrupt */
			if (hwgroup->poll_timeout != 0) {
				startstop = handler(drive);
			} else if (drive_is_ready(drive)) {
				if (drive->waiting_for_dma)
					(void) hwgroup->hwif->dmaproc(ide_dma_lostirq, drive);
				(void)ide_ack_intr(hwif);
				printk("%s: lost interrupt\n", drive->name);
				startstop = handler(drive);
			} else {
				if (drive->waiting_for_dma) {
					startstop = ide_stopped;
					ide_dma_timeout_retry(drive);
				} else
					startstop = ide_error(drive, "irq timeout", GET_STAT());
			}
			set_recovery_timer(hwif);
			drive->service_time = jiffies - drive->service_start;
			enable_irq(hwif->irq);
			spin_lock_irq(&io_request_lock);
			if (startstop == ide_stopped)
				hwgroup->busy = 0;
		}
	}
	ide_do_request(hwgroup, 0);
	spin_unlock_irqrestore(&io_request_lock, flags);
}

/*
 * There's nothing really useful we can do with an unexpected interrupt,
 * other than reading the status register (to clear it), and logging it.
 * There should be no way that an irq can happen before we're ready for it,
 * so we needn't worry much about losing an "important" interrupt here.
 *
 * On laptops (and "green" PCs), an unexpected interrupt occurs whenever the
 * drive enters "idle", "standby", or "sleep" mode, so if the status looks
 * "good", we just ignore the interrupt completely.
 *
 * This routine assumes __cli() is in effect when called.
 *
 * If an unexpected interrupt happens on irq15 while we are handling irq14
 * and if the two interfaces are "serialized" (CMD640), then it looks like
 * we could screw up by interfering with a new request being set up for irq15.
 *
 * In reality, this is a non-issue.  The new command is not sent unless the
 * drive is ready to accept one, in which case we know the drive is not
 * trying to interrupt us.  And ide_set_handler() is always invoked before
 * completing the issuance of any new drive command, so we will not be
 * accidently invoked as a result of any valid command completion interrupt.
 *
 */
static void unexpected_intr (int irq, ide_hwgroup_t *hwgroup)
{
	byte stat;
	ide_hwif_t *hwif = hwgroup->hwif;

	/*
	 * handle the unexpected interrupt
	 */
	do {
		if (hwif->irq == irq) {
			stat = IN_BYTE(hwif->io_ports[IDE_STATUS_OFFSET]);
			if (!OK_STAT(stat, READY_STAT, BAD_STAT)) {
				/* Try to not flood the console with msgs */
				static unsigned long last_msgtime, count;
				++count;
				if (0 < (signed long)(jiffies - (last_msgtime + HZ))) {
					last_msgtime = jiffies;
					printk("%s%s: unexpected interrupt, status=0x%02x, count=%ld\n",
					 hwif->name, (hwif->next == hwgroup->hwif) ? "" : "(?)", stat, count);
				}
			}
		}
	} while ((hwif = hwif->next) != hwgroup->hwif);
}

/*
 * entry point for all interrupts, caller does __cli() for us
 */
void ide_intr (int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long flags;
	ide_hwgroup_t *hwgroup = (ide_hwgroup_t *)dev_id;
	ide_hwif_t *hwif;
	ide_drive_t *drive;
	ide_handler_t *handler;
	ide_startstop_t startstop;

	spin_lock_irqsave(&io_request_lock, flags);
	hwif = hwgroup->hwif;

	if (!ide_ack_intr(hwif)) {
		spin_unlock_irqrestore(&io_request_lock, flags);
		return;
	}

	if ((handler = hwgroup->handler) == NULL || hwgroup->poll_timeout != 0) {
		/*
		 * Not expecting an interrupt from this drive.
		 * That means this could be:
		 *	(1) an interrupt from another PCI device
		 *	sharing the same PCI INT# as us.
		 * or	(2) a drive just entered sleep or standby mode,
		 *	and is interrupting to let us know.
		 * or	(3) a spurious interrupt of unknown origin.
		 *
		 * For PCI, we cannot tell the difference,
		 * so in that case we just ignore it and hope it goes away.
		 */
#ifdef CONFIG_BLK_DEV_IDEPCI
		if (IDE_PCI_DEVID_EQ(hwif->pci_devid, IDE_PCI_DEVID_NULL))
#endif	/* CONFIG_BLK_DEV_IDEPCI */
		{
			/*
			 * Probably not a shared PCI interrupt,
			 * so we can safely try to do something about it:
			 */
			unexpected_intr(irq, hwgroup);
#ifdef CONFIG_BLK_DEV_IDEPCI
		} else {
			/*
			 * Whack the status register, just in case we have a leftover pending IRQ.
			 */
			(void) IN_BYTE(hwif->io_ports[IDE_STATUS_OFFSET]);
#endif /* CONFIG_BLK_DEV_IDEPCI */
		}
		spin_unlock_irqrestore(&io_request_lock, flags);
		return;
	}
	drive = hwgroup->drive;
	if (!drive) {
		/*
		 * This should NEVER happen, and there isn't much we could do about it here.
		 */
		spin_unlock_irqrestore(&io_request_lock, flags);
		return;
	}
	if (!drive_is_ready(drive)) {
		/*
		 * This happens regularly when we share a PCI IRQ with another device.
		 * Unfortunately, it can also happen with some buggy drives that trigger
		 * the IRQ before their status register is up to date.  Hopefully we have
		 * enough advance overhead that the latter isn't a problem.
		 */
		spin_unlock_irqrestore(&io_request_lock, flags);
		return;
	}
	if (!hwgroup->busy) {
		hwgroup->busy = 1;	/* paranoia */
		printk("%s: ide_intr: hwgroup->busy was 0 ??\n", drive->name);
	}
	hwgroup->handler = NULL;
	del_timer(&hwgroup->timer);
	spin_unlock(&io_request_lock);

	if (drive->unmask)
		ide__sti();	/* local CPU only */
	startstop = handler(drive);		/* service this interrupt, may set handler for next interrupt */
	spin_lock_irq(&io_request_lock);

	/*
	 * Note that handler() may have set things up for another
	 * interrupt to occur soon, but it cannot happen until
	 * we exit from this routine, because it will be the
	 * same irq as is currently being serviced here, and Linux
	 * won't allow another of the same (on any CPU) until we return.
	 */
	set_recovery_timer(HWIF(drive));
	drive->service_time = jiffies - drive->service_start;
	if (startstop == ide_stopped) {
		if (hwgroup->handler == NULL) {	/* paranoia */
			hwgroup->busy = 0;
			ide_do_request(hwgroup, hwif->irq);
		} else {
			printk("%s: ide_intr: huh? expected NULL handler on exit\n", drive->name);
		}
	}
	spin_unlock_irqrestore(&io_request_lock, flags);
}

/*
 * get_info_ptr() returns the (ide_drive_t *) for a given device number.
 * It returns NULL if the given device number does not match any present drives.
 */
ide_drive_t *get_info_ptr (kdev_t i_rdev)
{
	int		major = MAJOR(i_rdev);
#if 0
	int		minor = MINOR(i_rdev) & PARTN_MASK;
#endif
	unsigned int	h;

	for (h = 0; h < MAX_HWIFS; ++h) {
		ide_hwif_t  *hwif = &ide_hwifs[h];
		if (hwif->present && major == hwif->major) {
			unsigned unit = DEVICE_NR(i_rdev);
			if (unit < MAX_DRIVES) {
				ide_drive_t *drive = &hwif->drives[unit];
#if 0
				if ((drive->present) && (drive->part[minor].nr_sects))
#else
				if (drive->present)
#endif
					return drive;
			}
			break;
		}
	}
	return NULL;
}

/*
 * This function is intended to be used prior to invoking ide_do_drive_cmd().
 */
void ide_init_drive_cmd (struct request *rq)
{
	memset(rq, 0, sizeof(*rq));
	rq->cmd = IDE_DRIVE_CMD;
}

/*
 * This function issues a special IDE device request
 * onto the request queue.
 *
 * If action is ide_wait, then the rq is queued at the end of the
 * request queue, and the function sleeps until it has been processed.
 * This is for use when invoked from an ioctl handler.
 *
 * If action is ide_preempt, then the rq is queued at the head of
 * the request queue, displacing the currently-being-processed
 * request and this function returns immediately without waiting
 * for the new rq to be completed.  This is VERY DANGEROUS, and is
 * intended for careful use by the ATAPI tape/cdrom driver code.
 *
 * If action is ide_next, then the rq is queued immediately after
 * the currently-being-processed-request (if any), and the function
 * returns without waiting for the new rq to be completed.  As above,
 * This is VERY DANGEROUS, and is intended for careful use by the
 * ATAPI tape/cdrom driver code.
 *
 * If action is ide_end, then the rq is queued at the end of the
 * request queue, and the function returns immediately without waiting
 * for the new rq to be completed. This is again intended for careful
 * use by the ATAPI tape/cdrom driver code.
 */
int ide_do_drive_cmd (ide_drive_t *drive, struct request *rq, ide_action_t action)
{
	unsigned long flags;
	ide_hwgroup_t *hwgroup = HWGROUP(drive);
	unsigned int major = HWIF(drive)->major;
	struct list_head *queue_head = &drive->queue.queue_head;
	DECLARE_COMPLETION(wait);

#ifdef CONFIG_BLK_DEV_PDC4030
	if (HWIF(drive)->chipset == ide_pdc4030 && rq->buffer != NULL)
		return -ENOSYS;  /* special drive cmds not supported */
#endif
	rq->errors = 0;
	rq->rq_status = RQ_ACTIVE;
	rq->rq_dev = MKDEV(major,(drive->select.b.unit)<<PARTN_BITS);
	if (action == ide_wait)
		rq->waiting = &wait;
	spin_lock_irqsave(&io_request_lock, flags);
	if (list_empty(queue_head) || action == ide_preempt) {
		if (action == ide_preempt)
			hwgroup->rq = NULL;
	} else {
		if (action == ide_wait || action == ide_end) {
			queue_head = queue_head->prev;
		} else
			queue_head = queue_head->next;
	}
	list_add(&rq->queue, queue_head);
	ide_do_request(hwgroup, 0);
	spin_unlock_irqrestore(&io_request_lock, flags);
	if (action == ide_wait) {
		wait_for_completion(&wait);	/* wait for it to be serviced */
		return rq->errors ? -EIO : 0;	/* return -EIO if errors */
	}
	return 0;

}

/*
 * This routine is called to flush all partitions and partition tables
 * for a changed disk, and then re-read the new partition table.
 * If we are revalidating a disk because of a media change, then we
 * enter with usage == 0.  If we are using an ioctl, we automatically have
 * usage == 1 (we need an open channel to use an ioctl :-), so this
 * is our limit.
 */
int ide_revalidate_disk (kdev_t i_rdev)
{
	ide_drive_t *drive;
	ide_hwgroup_t *hwgroup;
	unsigned int p, major, minor;
	long flags;

	if ((drive = get_info_ptr(i_rdev)) == NULL)
		return -ENODEV;
	major = MAJOR(i_rdev);
	minor = drive->select.b.unit << PARTN_BITS;
	hwgroup = HWGROUP(drive);
	spin_lock_irqsave(&io_request_lock, flags);
	if (drive->busy || (drive->usage > 1)) {
		spin_unlock_irqrestore(&io_request_lock, flags);
		return -EBUSY;
	};
	drive->busy = 1;
	MOD_INC_USE_COUNT;
	spin_unlock_irqrestore(&io_request_lock, flags);

	for (p = 0; p < (1<<PARTN_BITS); ++p) {
		if (drive->part[p].nr_sects > 0) {
			kdev_t devp = MKDEV(major, minor+p);
			invalidate_device(devp, 1);
		}
		drive->part[p].start_sect = 0;
		drive->part[p].nr_sects   = 0;
	};

	if (DRIVER(drive)->revalidate)
		DRIVER(drive)->revalidate(drive);

	drive->busy = 0;
	wake_up(&drive->wqueue);
	MOD_DEC_USE_COUNT;
	return 0;
}

static void revalidate_drives (void)
{
	ide_hwif_t *hwif;
	ide_drive_t *drive;
	int index, unit;

	for (index = 0; index < MAX_HWIFS; ++index) {
		hwif = &ide_hwifs[index];
		for (unit = 0; unit < MAX_DRIVES; ++unit) {
			drive = &ide_hwifs[index].drives[unit];
			if (drive->revalidate) {
				drive->revalidate = 0;
				if (!initializing)
					(void) ide_revalidate_disk(MKDEV(hwif->major, unit<<PARTN_BITS));
			}
		}
	}
}

static void ide_probe_module (void)
{
	if (!ide_probe) {
#if defined(CONFIG_KMOD) && defined(CONFIG_BLK_DEV_IDE_MODULE)
		(void) request_module("ide-probe-mod");
#endif /* (CONFIG_KMOD) && (CONFIG_BLK_DEV_IDE_MODULE) */
	} else {
		(void) ide_probe->init();
	}
	revalidate_drives();
}

static void ide_driver_module (void)
{
	int index;
	ide_module_t *module = ide_modules;

	for (index = 0; index < MAX_HWIFS; ++index)
		if (ide_hwifs[index].present)
			goto search;
	ide_probe_module();
search:
	while (module) {
		(void) module->init();
		module = module->next;
	}
	revalidate_drives();
}

static int ide_open (struct inode * inode, struct file * filp)
{
	ide_drive_t *drive;

	if ((drive = get_info_ptr(inode->i_rdev)) == NULL)
		return -ENXIO;
	if (drive->driver == NULL)
		ide_driver_module();
#ifdef CONFIG_KMOD
	if (drive->driver == NULL) {
		if (drive->media == ide_disk)
			(void) request_module("ide-disk");
		if (drive->media == ide_cdrom)
			(void) request_module("ide-cd");
		if (drive->media == ide_tape)
			(void) request_module("ide-tape");
		if (drive->media == ide_floppy)
			(void) request_module("ide-floppy");
	}
#endif /* CONFIG_KMOD */
	while (drive->busy)
		sleep_on(&drive->wqueue);
	drive->usage++;
	if (drive->driver != NULL)
		return DRIVER(drive)->open(inode, filp, drive);
	printk ("%s: driver not present\n", drive->name);
	drive->usage--;
	return -ENXIO;
}

/*
 * Releasing a block device means we sync() it, so that it can safely
 * be forgotten about...
 */
static int ide_release (struct inode * inode, struct file * file)
{
	ide_drive_t *drive;

	if ((drive = get_info_ptr(inode->i_rdev)) != NULL) {
		drive->usage--;
		if (drive->driver != NULL)
			DRIVER(drive)->release(inode, file, drive);
	}
	return 0;
}

int ide_replace_subdriver (ide_drive_t *drive, const char *driver)
{
	if (!drive->present || drive->busy || drive->usage)
		goto abort;
	if (drive->driver != NULL && DRIVER(drive)->cleanup(drive))
		goto abort;
	strncpy(drive->driver_req, driver, 9);
	ide_driver_module();
	drive->driver_req[0] = 0;
	ide_driver_module();
	if (DRIVER(drive) && !strcmp(DRIVER(drive)->name, driver))
		return 0;
abort:
	return 1;
}

#ifdef CONFIG_PROC_FS
ide_proc_entry_t generic_subdriver_entries[] = {
	{ "capacity",	S_IFREG|S_IRUGO,	proc_ide_read_capacity,	NULL },
	{ NULL, 0, NULL, NULL }
};
#endif

/*
 * Note that we only release the standard ports,
 * and do not even try to handle any extra ports
 * allocated for weird IDE interface chipsets.
 */
void hwif_unregister (ide_hwif_t *hwif)
{
	if (hwif->straight8) {
		ide_release_region(hwif->io_ports[IDE_DATA_OFFSET], 8);
		goto jump_eight;
	}
	if (hwif->io_ports[IDE_DATA_OFFSET])
		ide_release_region(hwif->io_ports[IDE_DATA_OFFSET], 1);
	if (hwif->io_ports[IDE_ERROR_OFFSET])
		ide_release_region(hwif->io_ports[IDE_ERROR_OFFSET], 1);
	if (hwif->io_ports[IDE_NSECTOR_OFFSET])
		ide_release_region(hwif->io_ports[IDE_NSECTOR_OFFSET], 1);
	if (hwif->io_ports[IDE_SECTOR_OFFSET])
		ide_release_region(hwif->io_ports[IDE_SECTOR_OFFSET], 1);
	if (hwif->io_ports[IDE_LCYL_OFFSET])
		ide_release_region(hwif->io_ports[IDE_LCYL_OFFSET], 1);
	if (hwif->io_ports[IDE_HCYL_OFFSET])
		ide_release_region(hwif->io_ports[IDE_HCYL_OFFSET], 1);
	if (hwif->io_ports[IDE_SELECT_OFFSET])
		ide_release_region(hwif->io_ports[IDE_SELECT_OFFSET], 1);
	if (hwif->io_ports[IDE_STATUS_OFFSET])
		ide_release_region(hwif->io_ports[IDE_STATUS_OFFSET], 1);
jump_eight:
	if (hwif->io_ports[IDE_CONTROL_OFFSET])
		ide_release_region(hwif->io_ports[IDE_CONTROL_OFFSET], 1);
#if defined(CONFIG_AMIGA) || defined(CONFIG_MAC)
	if (hwif->io_ports[IDE_IRQ_OFFSET])
		ide_release_region(hwif->io_ports[IDE_IRQ_OFFSET], 1);
#endif /* (CONFIG_AMIGA) || (CONFIG_MAC) */
}

void ide_unregister (unsigned int index)
{
	struct gendisk *gd;
	ide_drive_t *drive, *d;
	ide_hwif_t *hwif, *g;
	ide_hwgroup_t *hwgroup;
	int irq_count = 0, unit, i;
	unsigned long flags;
	unsigned int p, minor;
	ide_hwif_t old_hwif;

	if (index >= MAX_HWIFS)
		return;
	save_flags(flags);	/* all CPUs */
	cli();			/* all CPUs */
	hwif = &ide_hwifs[index];
	if (!hwif->present)
		goto abort;
	for (unit = 0; unit < MAX_DRIVES; ++unit) {
		drive = &hwif->drives[unit];
		if (!drive->present)
			continue;
		if (drive->busy || drive->usage)
			goto abort;
		if (drive->driver != NULL && DRIVER(drive)->cleanup(drive))
			goto abort;
	}
	hwif->present = 0;
	
	/*
	 * All clear?  Then blow away the buffer cache
	 */
	sti();
	for (unit = 0; unit < MAX_DRIVES; ++unit) {
		drive = &hwif->drives[unit];
		if (!drive->present)
			continue;
		minor = drive->select.b.unit << PARTN_BITS;
		for (p = 0; p < (1<<PARTN_BITS); ++p) {
			if (drive->part[p].nr_sects > 0) {
				kdev_t devp = MKDEV(hwif->major, minor+p);
				invalidate_device(devp, 0);
			}
		}
#ifdef CONFIG_PROC_FS
		destroy_proc_ide_drives(hwif);
#endif
	}
	cli();
	hwgroup = hwif->hwgroup;

	/*
	 * free the irq if we were the only hwif using it
	 */
	g = hwgroup->hwif;
	do {
		if (g->irq == hwif->irq)
			++irq_count;
		g = g->next;
	} while (g != hwgroup->hwif);
	if (irq_count == 1)
		free_irq(hwif->irq, hwgroup);

	/*
	 * Note that we only release the standard ports,
	 * and do not even try to handle any extra ports
	 * allocated for weird IDE interface chipsets.
	 */
	hwif_unregister(hwif);

	/*
	 * Remove us from the hwgroup, and free
	 * the hwgroup if we were the only member
	 */
	d = hwgroup->drive;
	for (i = 0; i < MAX_DRIVES; ++i) {
		drive = &hwif->drives[i];
		if (drive->de) {
			devfs_unregister (drive->de);
			drive->de = NULL;
		}
		if (!drive->present)
			continue;
		while (hwgroup->drive->next != drive)
			hwgroup->drive = hwgroup->drive->next;
		hwgroup->drive->next = drive->next;
		if (hwgroup->drive == drive)
			hwgroup->drive = NULL;
		if (drive->id != NULL) {
			kfree(drive->id);
			drive->id = NULL;
		}
		drive->present = 0;
		blk_cleanup_queue(&drive->queue);
	}
	if (d->present)
		hwgroup->drive = d;
	while (hwgroup->hwif->next != hwif)
		hwgroup->hwif = hwgroup->hwif->next;
	hwgroup->hwif->next = hwif->next;
	if (hwgroup->hwif == hwif)
		kfree(hwgroup);
	else
		hwgroup->hwif = HWIF(hwgroup->drive);

#if defined(CONFIG_BLK_DEV_IDEDMA) && !defined(CONFIG_DMA_NONPCI)
	if (hwif->dma_base) {
		(void) ide_release_dma(hwif);
		hwif->dma_base = 0;
	}
#endif /* (CONFIG_BLK_DEV_IDEDMA) && !(CONFIG_DMA_NONPCI) */

	/*
	 * Remove us from the kernel's knowledge
	 */
	unregister_blkdev(hwif->major, hwif->name);
	kfree(blksize_size[hwif->major]);
	kfree(max_sectors[hwif->major]);
	kfree(max_readahead[hwif->major]);
	blk_dev[hwif->major].data = NULL;
	blk_dev[hwif->major].queue = NULL;
	blksize_size[hwif->major] = NULL;
	gd = hwif->gd;
	if (gd) {
		del_gendisk(gd);
		kfree(gd->sizes);
		kfree(gd->part);
		if (gd->de_arr)
			kfree (gd->de_arr);
		if (gd->flags)
			kfree (gd->flags);
		kfree(gd);
		hwif->gd = NULL;
	}
	old_hwif		= *hwif;
	init_hwif_data (index);	/* restore hwif data to pristine status */
	hwif->hwgroup		= old_hwif.hwgroup;
	hwif->tuneproc		= old_hwif.tuneproc;
	hwif->speedproc		= old_hwif.speedproc;
	hwif->selectproc	= old_hwif.selectproc;
	hwif->resetproc		= old_hwif.resetproc;
	hwif->intrproc		= old_hwif.intrproc;
	hwif->maskproc		= old_hwif.maskproc;
	hwif->quirkproc		= old_hwif.quirkproc;
	hwif->rwproc		= old_hwif.rwproc;
	hwif->ideproc		= old_hwif.ideproc;
	hwif->dmaproc		= old_hwif.dmaproc;
	hwif->busproc		= old_hwif.busproc;
	hwif->bus_state		= old_hwif.bus_state;
	hwif->dma_base		= old_hwif.dma_base;
	hwif->dma_extra		= old_hwif.dma_extra;
	hwif->config_data	= old_hwif.config_data;
	hwif->select_data	= old_hwif.select_data;
	hwif->proc		= old_hwif.proc;
#ifndef CONFIG_BLK_DEV_IDECS
	hwif->irq		= old_hwif.irq;
#endif /* CONFIG_BLK_DEV_IDECS */
	hwif->major		= old_hwif.major;
	hwif->chipset		= old_hwif.chipset;
	hwif->autodma		= old_hwif.autodma;
	hwif->udma_four		= old_hwif.udma_four;
#ifdef CONFIG_BLK_DEV_IDEPCI
	hwif->pci_dev		= old_hwif.pci_dev;
	hwif->pci_devid		= old_hwif.pci_devid;
#endif /* CONFIG_BLK_DEV_IDEPCI */
	hwif->straight8		= old_hwif.straight8;
	hwif->hwif_data		= old_hwif.hwif_data;
abort:
	restore_flags(flags);	/* all CPUs */
}

/*
 * Setup hw_regs_t structure described by parameters.  You
 * may set up the hw structure yourself OR use this routine to
 * do it for you.
 */
void ide_setup_ports (	hw_regs_t *hw,
			ide_ioreg_t base, int *offsets,
			ide_ioreg_t ctrl, ide_ioreg_t intr,
			ide_ack_intr_t *ack_intr, int irq)
{
	int i;

	for (i = 0; i < IDE_NR_PORTS; i++) {
		if (offsets[i] == -1) {
			switch(i) {
				case IDE_CONTROL_OFFSET:
					hw->io_ports[i] = ctrl;
					break;
#if defined(CONFIG_AMIGA) || defined(CONFIG_MAC)
				case IDE_IRQ_OFFSET:
					hw->io_ports[i] = intr;
					break;
#endif /* (CONFIG_AMIGA) || (CONFIG_MAC) */
				default:
					hw->io_ports[i] = 0;
					break;
			}
		} else {
			hw->io_ports[i] = base + offsets[i];
		}
	}
	hw->irq = irq;
	hw->dma = NO_DMA;
	hw->ack_intr = ack_intr;
}

/*
 * Register an IDE interface, specifing exactly the registers etc
 * Set init=1 iff calling before probes have taken place.
 */
int ide_register_hw (hw_regs_t *hw, ide_hwif_t **hwifp)
{
	int index, retry = 1;
	ide_hwif_t *hwif;

	do {
		for (index = 0; index < MAX_HWIFS; ++index) {
			hwif = &ide_hwifs[index];
			if (hwif->hw.io_ports[IDE_DATA_OFFSET] == hw->io_ports[IDE_DATA_OFFSET])
				goto found;
		}
		for (index = 0; index < MAX_HWIFS; ++index) {
			hwif = &ide_hwifs[index];
			if ((!hwif->present && !hwif->mate && !initializing) ||
			    (!hwif->hw.io_ports[IDE_DATA_OFFSET] && initializing))
				goto found;
		}
		for (index = 0; index < MAX_HWIFS; index++)
			ide_unregister(index);
	} while (retry--);
	return -1;
found:
	if (hwif->present)
		ide_unregister(index);
	if (hwif->present)
		return -1;
	memcpy(&hwif->hw, hw, sizeof(*hw));
	memcpy(hwif->io_ports, hwif->hw.io_ports, sizeof(hwif->hw.io_ports));
	hwif->irq = hw->irq;
	hwif->noprobe = 0;

	if (!initializing) {
		ide_probe_module();
#ifdef CONFIG_PROC_FS
		create_proc_ide_interfaces();
#endif
		ide_driver_module();
	}

	if (hwifp)
		*hwifp = hwif;

	return (initializing || hwif->present) ? index : -1;
}

/*
 * Compatability function with existing drivers.  If you want
 * something different, use the function above.
 */
int ide_register (int arg1, int arg2, int irq)
{
	hw_regs_t hw;
	ide_init_hwif_ports(&hw, (ide_ioreg_t) arg1, (ide_ioreg_t) arg2, NULL);
	hw.irq = irq;
	return ide_register_hw(&hw, NULL);
}

void ide_add_setting (ide_drive_t *drive, const char *name, int rw, int read_ioctl, int write_ioctl, int data_type, int min, int max, int mul_factor, int div_factor, void *data, ide_procset_t *set)
{
	ide_settings_t **p = (ide_settings_t **) &drive->settings, *setting = NULL;

	while ((*p) && strcmp((*p)->name, name) < 0)
		p = &((*p)->next);
	if ((setting = kmalloc(sizeof(*setting), GFP_KERNEL)) == NULL)
		goto abort;
	memset(setting, 0, sizeof(*setting));
	if ((setting->name = kmalloc(strlen(name) + 1, GFP_KERNEL)) == NULL)
		goto abort;
	strcpy(setting->name, name);		setting->rw = rw;
	setting->read_ioctl = read_ioctl;	setting->write_ioctl = write_ioctl;
	setting->data_type = data_type;		setting->min = min;
	setting->max = max;			setting->mul_factor = mul_factor;
	setting->div_factor = div_factor;	setting->data = data;
	setting->set = set;			setting->next = *p;
	if (drive->driver)
		setting->auto_remove = 1;
	*p = setting;
	return;
abort:
	if (setting)
		kfree(setting);
}

void ide_remove_setting (ide_drive_t *drive, char *name)
{
	ide_settings_t **p = (ide_settings_t **) &drive->settings, *setting;

	while ((*p) && strcmp((*p)->name, name))
		p = &((*p)->next);
	if ((setting = (*p)) == NULL)
		return;
	(*p) = setting->next;
	kfree(setting->name);
	kfree(setting);
}

static ide_settings_t *ide_find_setting_by_ioctl (ide_drive_t *drive, int cmd)
{
	ide_settings_t *setting = drive->settings;

	while (setting) {
		if (setting->read_ioctl == cmd || setting->write_ioctl == cmd)
			break;
		setting = setting->next;
	}
	return setting;
}

ide_settings_t *ide_find_setting_by_name (ide_drive_t *drive, char *name)
{
	ide_settings_t *setting = drive->settings;

	while (setting) {
		if (strcmp(setting->name, name) == 0)
			break;
		setting = setting->next;
	}
	return setting;
}

static void auto_remove_settings (ide_drive_t *drive)
{
	ide_settings_t *setting;
repeat:
	setting = drive->settings;
	while (setting) {
		if (setting->auto_remove) {
			ide_remove_setting(drive, setting->name);
			goto repeat;
		}
		setting = setting->next;
	}
}

int ide_read_setting (ide_drive_t *drive, ide_settings_t *setting)
{
	int		val = -EINVAL;
	unsigned long	flags;

	if ((setting->rw & SETTING_READ)) {
		spin_lock_irqsave(&io_request_lock, flags);
		switch(setting->data_type) {
			case TYPE_BYTE:
				val = *((u8 *) setting->data);
				break;
			case TYPE_SHORT:
				val = *((u16 *) setting->data);
				break;
			case TYPE_INT:
			case TYPE_INTA:
				val = *((u32 *) setting->data);
				break;
		}
		spin_unlock_irqrestore(&io_request_lock, flags);
	}
	return val;
}

int ide_spin_wait_hwgroup (ide_drive_t *drive)
{
	ide_hwgroup_t *hwgroup = HWGROUP(drive);
	unsigned long timeout = jiffies + (3 * HZ);

	spin_lock_irq(&io_request_lock);

	while (hwgroup->busy) {
		unsigned long lflags;
		spin_unlock_irq(&io_request_lock);
		__save_flags(lflags);	/* local CPU only */
		__sti();		/* local CPU only; needed for jiffies */
		if (0 < (signed long)(jiffies - timeout)) {
			__restore_flags(lflags);	/* local CPU only */
			printk("%s: channel busy\n", drive->name);
			return -EBUSY;
		}
		__restore_flags(lflags);	/* local CPU only */
		spin_lock_irq(&io_request_lock);
	}
	return 0;
}

/*
 * FIXME:  This should be changed to enqueue a special request
 * to the driver to change settings, and then wait on a sema for completion.
 * The current scheme of polling is kludgey, though safe enough.
 */
int ide_write_setting (ide_drive_t *drive, ide_settings_t *setting, int val)
{
	int i;
	u32 *p;

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;
	if (!(setting->rw & SETTING_WRITE))
		return -EPERM;
	if (val < setting->min || val > setting->max)
		return -EINVAL;
	if (setting->set)
		return setting->set(drive, val);
	if (ide_spin_wait_hwgroup(drive))
		return -EBUSY;
	switch (setting->data_type) {
		case TYPE_BYTE:
			*((u8 *) setting->data) = val;
			break;
		case TYPE_SHORT:
			*((u16 *) setting->data) = val;
			break;
		case TYPE_INT:
			*((u32 *) setting->data) = val;
			break;
		case TYPE_INTA:
			p = (u32 *) setting->data;
			for (i = 0; i < 1 << PARTN_BITS; i++, p++)
				*p = val;
			break;
	}
	spin_unlock_irq(&io_request_lock);
	return 0;
}

static int set_io_32bit(ide_drive_t *drive, int arg)
{
	drive->io_32bit = arg;
#ifdef CONFIG_BLK_DEV_DTC2278
	if (HWIF(drive)->chipset == ide_dtc2278)
		HWIF(drive)->drives[!drive->select.b.unit].io_32bit = arg;
#endif /* CONFIG_BLK_DEV_DTC2278 */
	return 0;
}

static int set_using_dma (ide_drive_t *drive, int arg)
{
	if (!drive->driver || !DRIVER(drive)->supports_dma)
		return -EPERM;
	if (!drive->id || !(drive->id->capability & 1) || !HWIF(drive)->dmaproc)
		return -EPERM;
	if (HWIF(drive)->dmaproc(arg ? ide_dma_on : ide_dma_off, drive))
		return -EIO;
	return 0;
}

static int set_pio_mode (ide_drive_t *drive, int arg)
{
	struct request rq;

	if (!HWIF(drive)->tuneproc)
		return -ENOSYS;
	if (drive->special.b.set_tune)
		return -EBUSY;
	ide_init_drive_cmd(&rq);
	drive->tune_req = (byte) arg;
	drive->special.b.set_tune = 1;
	(void) ide_do_drive_cmd (drive, &rq, ide_wait);
	return 0;
}

void ide_add_generic_settings (ide_drive_t *drive)
{
/*
 *			drive	setting name		read/write access				read ioctl		write ioctl		data type	min	max				mul_factor	div_factor	data pointer			set function
 */
	ide_add_setting(drive,	"io_32bit",		drive->no_io_32bit ? SETTING_READ : SETTING_RW,	HDIO_GET_32BIT,		HDIO_SET_32BIT,		TYPE_BYTE,	0,	1 + (SUPPORT_VLB_SYNC << 1),	1,		1,		&drive->io_32bit,		set_io_32bit);
	ide_add_setting(drive,	"keepsettings",		SETTING_RW,					HDIO_GET_KEEPSETTINGS,	HDIO_SET_KEEPSETTINGS,	TYPE_BYTE,	0,	1,				1,		1,		&drive->keep_settings,		NULL);
	ide_add_setting(drive,	"nice1",		SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	1,				1,		1,		&drive->nice1,			NULL);
	ide_add_setting(drive,	"pio_mode",		SETTING_WRITE,					-1,			HDIO_SET_PIO_MODE,	TYPE_BYTE,	0,	255,				1,		1,		NULL,				set_pio_mode);
	ide_add_setting(drive,	"slow",			SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	1,				1,		1,		&drive->slow,			NULL);
	ide_add_setting(drive,	"unmaskirq",		drive->no_unmask ? SETTING_READ : SETTING_RW,	HDIO_GET_UNMASKINTR,	HDIO_SET_UNMASKINTR,	TYPE_BYTE,	0,	1,				1,		1,		&drive->unmask,			NULL);
	ide_add_setting(drive,	"using_dma",		SETTING_RW,					HDIO_GET_DMA,		HDIO_SET_DMA,		TYPE_BYTE,	0,	1,				1,		1,		&drive->using_dma,		set_using_dma);
	ide_add_setting(drive,	"ide_scsi",		SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	1,				1,		1,		&drive->scsi,			NULL);
	ide_add_setting(drive,	"init_speed",		SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	69,				1,		1,		&drive->init_speed,		NULL);
	ide_add_setting(drive,	"current_speed",	SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	69,				1,		1,		&drive->current_speed,		NULL);
	ide_add_setting(drive,	"number",		SETTING_RW,					-1,			-1,			TYPE_BYTE,	0,	3,				1,		1,		&drive->dn,			NULL);
}

int ide_wait_cmd (ide_drive_t *drive, int cmd, int nsect, int feature, int sectors, byte *buf)
{
	struct request rq;
	byte buffer[4];

	if (!buf)
		buf = buffer;
	memset(buf, 0, 4 + SECTOR_WORDS * 4 * sectors);
	ide_init_drive_cmd(&rq);
	rq.buffer = buf;
	*buf++ = cmd;
	*buf++ = nsect;
	*buf++ = feature;
	*buf++ = sectors;
	return ide_do_drive_cmd(drive, &rq, ide_wait);
}

int ide_wait_cmd_task (ide_drive_t *drive, byte *buf)
{
	struct request rq;

	ide_init_drive_cmd(&rq);
	rq.cmd = IDE_DRIVE_TASK;
	rq.buffer = buf;
	return ide_do_drive_cmd(drive, &rq, ide_wait);
}

/*
 * Delay for *at least* 50ms.  As we don't know how much time is left
 * until the next tick occurs, we wait an extra tick to be safe.
 * This is used only during the probing/polling for drives at boot time.
 *
 * However, its usefullness may be needed in other places, thus we export it now.
 * The future may change this to a millisecond setable delay.
 */
void ide_delay_50ms (void)
{
#ifndef CONFIG_BLK_DEV_IDECS
	mdelay(50);
#else
	__set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(HZ/20);
#endif /* CONFIG_BLK_DEV_IDECS */
}

int system_bus_clock (void)
{
	return((int) ((!system_bus_speed) ? ide_system_bus_speed() : system_bus_speed ));
}

static int ide_ioctl (struct inode *inode, struct file *file,
			unsigned int cmd, unsigned long arg)
{
	int err = 0, major, minor;
	ide_drive_t *drive;
	struct request rq;
	kdev_t dev;
	ide_settings_t *setting;

	if (!inode || !(dev = inode->i_rdev))
		return -EINVAL;
	major = MAJOR(dev); minor = MINOR(dev);
	if ((drive = get_info_ptr(inode->i_rdev)) == NULL)
		return -ENODEV;

	if ((setting = ide_find_setting_by_ioctl(drive, cmd)) != NULL) {
		if (cmd == setting->read_ioctl) {
			err = ide_read_setting(drive, setting);
			return err >= 0 ? put_user(err, (long *) arg) : err;
		} else {
			if ((MINOR(inode->i_rdev) & PARTN_MASK))
				return -EINVAL;
			return ide_write_setting(drive, setting, arg);
		}
	}

	ide_init_drive_cmd (&rq);
	switch (cmd) {
		case HDIO_GETGEO:
		{
			struct hd_geometry *loc = (struct hd_geometry *) arg;
			unsigned short bios_cyl = drive->bios_cyl; /* truncate */
			if (!loc || (drive->media != ide_disk && drive->media != ide_floppy)) return -EINVAL;
			if (put_user(drive->bios_head, (byte *) &loc->heads)) return -EFAULT;
			if (put_user(drive->bios_sect, (byte *) &loc->sectors)) return -EFAULT;
			if (put_user(bios_cyl, (unsigned short *) &loc->cylinders)) return -EFAULT;
			if (put_user((unsigned)drive->part[MINOR(inode->i_rdev)&PARTN_MASK].start_sect,
				(unsigned long *) &loc->start)) return -EFAULT;
			return 0;
		}

		case HDIO_GETGEO_BIG:
		{
			struct hd_big_geometry *loc = (struct hd_big_geometry *) arg;
			if (!loc || (drive->media != ide_disk && drive->media != ide_floppy)) return -EINVAL;
			if (put_user(drive->bios_head, (byte *) &loc->heads)) return -EFAULT;
			if (put_user(drive->bios_sect, (byte *) &loc->sectors)) return -EFAULT;
			if (put_user(drive->bios_cyl, (unsigned int *) &loc->cylinders)) return -EFAULT;
			if (put_user((unsigned)drive->part[MINOR(inode->i_rdev)&PARTN_MASK].start_sect,
				(unsigned long *) &loc->start)) return -EFAULT;
			return 0;
		}

		case HDIO_GETGEO_BIG_RAW:
		{
			struct hd_big_geometry *loc = (struct hd_big_geometry *) arg;
			if (!loc || (drive->media != ide_disk && drive->media != ide_floppy)) return -EINVAL;
			if (put_user(drive->head, (byte *) &loc->heads)) return -EFAULT;
			if (put_user(drive->sect, (byte *) &loc->sectors)) return -EFAULT;
			if (put_user(drive->cyl, (unsigned int *) &loc->cylinders)) return -EFAULT;
			if (put_user((unsigned)drive->part[MINOR(inode->i_rdev)&PARTN_MASK].start_sect,
				(unsigned long *) &loc->start)) return -EFAULT;
			return 0;
		}

	 	case BLKGETSIZE:   /* Return device size */
			return put_user(drive->part[MINOR(inode->i_rdev)&PARTN_MASK].nr_sects, (unsigned long *) arg);
	 	case BLKGETSIZE64:
			return put_user((u64)drive->part[MINOR(inode->i_rdev)&PARTN_MASK].nr_sects << 9, (u64 *) arg);

		case BLKRRPART: /* Re-read partition tables */
			if (!capable(CAP_SYS_ADMIN)) return -EACCES;
			return ide_revalidate_disk(inode->i_rdev);

		case HDIO_OBSOLETE_IDENTITY:
		case HDIO_GET_IDENTITY:
			if (MINOR(inode->i_rdev) & PARTN_MASK)
				return -EINVAL;
			if (drive->id == NULL)
				return -ENOMSG;
			if (copy_to_user((char *)arg, (char *)drive->id, (cmd == HDIO_GET_IDENTITY) ? sizeof(*drive->id) : 142))
				return -EFAULT;
			return 0;

		case HDIO_GET_NICE:
			return put_user(drive->dsc_overlap	<<	IDE_NICE_DSC_OVERLAP	|
					drive->atapi_overlap	<<	IDE_NICE_ATAPI_OVERLAP	|
					drive->nice0		<< 	IDE_NICE_0		|
					drive->nice1		<<	IDE_NICE_1		|
					drive->nice2		<<	IDE_NICE_2,
					(long *) arg);
		case HDIO_DRIVE_CMD:
		{
			byte args[4], *argbuf = args;
			byte xfer_rate = 0;
			int argsize = 4;
			if (!capable(CAP_SYS_ADMIN) || !capable(CAP_SYS_RAWIO)) return -EACCES;
			if (NULL == (void *) arg)
				return ide_do_drive_cmd(drive, &rq, ide_wait);
			if (copy_from_user(args, (void *)arg, 4))
				return -EFAULT;
			if (args[3]) {
				argsize = 4 + (SECTOR_WORDS * 4 * args[3]);
				argbuf = kmalloc(argsize, GFP_KERNEL);
				if (argbuf == NULL)
					return -ENOMEM;
				memcpy(argbuf, args, 4);
			}

			if (set_transfer(drive, args[0], args[1], args[2])) {
				xfer_rate = args[1];
				if (ide_ata66_check(drive, args[0], args[1], args[2]))
					goto abort;
			}

			err = ide_wait_cmd(drive, args[0], args[1], args[2], args[3], argbuf);

			if (!err && xfer_rate) {
				/* active-retuning-calls future */
				if ((HWIF(drive)->speedproc) != NULL)
					HWIF(drive)->speedproc(drive, xfer_rate);
				ide_driveid_update(drive);
			}
		abort:
			if (copy_to_user((void *)arg, argbuf, argsize))
				err = -EFAULT;
			if (argsize > 4)
				kfree(argbuf);
			return err;
		}
		case HDIO_DRIVE_TASK:
		{
			byte args[7], *argbuf = args;
			int argsize = 7;
			if (!capable(CAP_SYS_ADMIN) || !capable(CAP_SYS_RAWIO)) return -EACCES;
			if (copy_from_user(args, (void *)arg, 7))
				return -EFAULT;
			err = ide_wait_cmd_task(drive, argbuf);
			if (copy_to_user((void *)arg, argbuf, argsize))
				err = -EFAULT;
			return err;
		}
		case HDIO_SCAN_HWIF:
		{
			int args[3];
			if (!capable(CAP_SYS_ADMIN)) return -EACCES;
			if (copy_from_user(args, (void *)arg, 3 * sizeof(int)))
				return -EFAULT;
			if (ide_register(args[0], args[1], args[2]) == -1)
				return -EIO;
			return 0;
		}
	        case HDIO_UNREGISTER_HWIF:
			if (!capable(CAP_SYS_ADMIN)) return -EACCES;
			/* (arg > MAX_HWIFS) checked in function */
			ide_unregister(arg);
			return 0;
		case HDIO_SET_NICE:
			if (!capable(CAP_SYS_ADMIN)) return -EACCES;
			if (drive->driver == NULL)
				return -EPERM;
			if (arg != (arg & ((1 << IDE_NICE_DSC_OVERLAP) | (1 << IDE_NICE_1))))
				return -EPERM;
			drive->dsc_overlap = (arg >> IDE_NICE_DSC_OVERLAP) & 1;
			if (drive->dsc_overlap && !DRIVER(drive)->supports_dsc_overlap) {
				drive->dsc_overlap = 0;
				return -EPERM;
			}
			drive->nice1 = (arg >> IDE_NICE_1) & 1;
			return 0;
		case HDIO_DRIVE_RESET:
			if (!capable(CAP_SYS_ADMIN)) return -EACCES;
			(void) ide_do_reset(drive);
			if (drive->suspend_reset) {
/*
 *				APM WAKE UP todo !!
 *				int nogoodpower = 1;
 *				while(nogoodpower) {
 *					check_power1() or check_power2()
 *					nogoodpower = 0;
 *				} 
 *				HWIF(drive)->multiproc(drive);
 */
				return ide_revalidate_disk(inode->i_rdev);
			}
			return 0;

		case BLKROSET:
		case BLKROGET:
		case BLKFLSBUF:
		case BLKSSZGET:
		case BLKPG:
		case BLKELVGET:
		case BLKELVSET:
		case BLKBSZGET:
		case BLKBSZSET:
			return blk_ioctl(inode->i_rdev, cmd, arg);

		case HDIO_GET_BUSSTATE:
			if (!capable(CAP_SYS_ADMIN))
				return -EACCES;
			if (put_user(HWIF(drive)->bus_state, (long *)arg))
				return -EFAULT;
			return 0;

		case HDIO_SET_BUSSTATE:
			if (!capable(CAP_SYS_ADMIN))
				return -EACCES;
			if (HWIF(drive)->busproc)
				HWIF(drive)->busproc(HWIF(drive), arg);
			return 0;

		default:
			if (drive->driver != NULL)
				return DRIVER(drive)->ioctl(drive, inode, file, cmd, arg);
			return -EPERM;
	}
}

static int ide_check_media_change (kdev_t i_rdev)
{
	ide_drive_t *drive;

	if ((drive = get_info_ptr(i_rdev)) == NULL)
		return -ENODEV;
	if (drive->driver != NULL)
		return DRIVER(drive)->media_change(drive);
	return 0;
}

void ide_fixstring (byte *s, const int bytecount, const int byteswap)
{
	byte *p = s, *end = &s[bytecount & ~1]; /* bytecount must be even */

	if (byteswap) {
		/* convert from big-endian to host byte order */
		for (p = end ; p != s;) {
			unsigned short *pp = (unsigned short *) (p -= 2);
			*pp = ntohs(*pp);
		}
	}

	/* strip leading blanks */
	while (s != end && *s == ' ')
		++s;

	/* compress internal blanks and strip trailing blanks */
	while (s != end && *s) {
		if (*s++ != ' ' || (s != end && *s && *s != ' '))
			*p++ = *(s-1);
	}

	/* wipe out trailing garbage */
	while (p != end)
		*p++ = '\0';
}

/*
 * stridx() returns the offset of c within s,
 * or -1 if c is '\0' or not found within s.
 */
static int __init stridx (const char *s, char c)
{
	char *i = strchr(s, c);
	return (i && c) ? i - s : -1;
}

/*
 * match_parm() does parsing for ide_setup():
 *
 * 1. the first char of s must be '='.
 * 2. if the remainder matches one of the supplied keywords,
 *     the index (1 based) of the keyword is negated and returned.
 * 3. if the remainder is a series of no more than max_vals numbers
 *     separated by commas, the numbers are saved in vals[] and a
 *     count of how many were saved is returned.  Base10 is assumed,
 *     and base16 is allowed when prefixed with "0x".
 * 4. otherwise, zero is returned.
 */
static int __init match_parm (char *s, const char *keywords[], int vals[], int max_vals)
{
	static const char *decimal = "0123456789";
	static const char *hex = "0123456789abcdef";
	int i, n;

	if (*s++ == '=') {
		/*
		 * Try matching against the supplied keywords,
		 * and return -(index+1) if we match one
		 */
		if (keywords != NULL) {
			for (i = 0; *keywords != NULL; ++i) {
				if (!strcmp(s, *keywords++))
					return -(i+1);
			}
		}
		/*
		 * Look for a series of no more than "max_vals"
		 * numeric values separated by commas, in base10,
		 * or base16 when prefixed with "0x".
		 * Return a count of how many were found.
		 */
		for (n = 0; (i = stridx(decimal, *s)) >= 0;) {
			vals[n] = i;
			while ((i = stridx(decimal, *++s)) >= 0)
				vals[n] = (vals[n] * 10) + i;
			if (*s == 'x' && !vals[n]) {
				while ((i = stridx(hex, *++s)) >= 0)
					vals[n] = (vals[n] * 0x10) + i;
			}
			if (++n == max_vals)
				break;
			if (*s == ',' || *s == ';')
				++s;
		}
		if (!*s)
			return n;
	}
	return 0;	/* zero = nothing matched */
}

/*
 * ide_setup() gets called VERY EARLY during initialization,
 * to handle kernel "command line" strings beginning with "hdx="
 * or "ide".  Here is the complete set currently supported:
 *
 * "hdx="  is recognized for all "x" from "a" to "h", such as "hdc".
 * "idex=" is recognized for all "x" from "0" to "3", such as "ide1".
 *
 * "hdx=noprobe"	: drive may be present, but do not probe for it
 * "hdx=none"		: drive is NOT present, ignore cmos and do not probe
 * "hdx=nowerr"		: ignore the WRERR_STAT bit on this drive
 * "hdx=cdrom"		: drive is present, and is a cdrom drive
 * "hdx=cyl,head,sect"	: disk drive is present, with specified geometry
 * "hdx=noremap"	: do not remap 0->1 even though EZD was detected
 * "hdx=autotune"	: driver will attempt to tune interface speed
 *				to the fastest PIO mode supported,
 *				if possible for this drive only.
 *				Not fully supported by all chipset types,
 *				and quite likely to cause trouble with
 *				older/odd IDE drives.
 *
 * "hdx=slow"		: insert a huge pause after each access to the data
 *				port. Should be used only as a last resort.
 *
 * "hdx=swapdata"	: when the drive is a disk, byte swap all data
 * "hdx=bswap"		: same as above..........
 * "hdxlun=xx"          : set the drive last logical unit.
 * "hdx=flash"		: allows for more than one ata_flash disk to be
 *				registered. In most cases, only one device
 *				will be present.
 * "hdx=scsi"		: the return of the ide-scsi flag, this is useful for
 *				allowwing ide-floppy, ide-tape, and ide-cdrom|writers
 *				to use ide-scsi emulation on a device specific option.
 * "idebus=xx"		: inform IDE driver of VESA/PCI bus speed in MHz,
 *				where "xx" is between 20 and 66 inclusive,
 *				used when tuning chipset PIO modes.
 *				For PCI bus, 25 is correct for a P75 system,
 *				30 is correct for P90,P120,P180 systems,
 *				and 33 is used for P100,P133,P166 systems.
 *				If in doubt, use idebus=33 for PCI.
 *				As for VLB, it is safest to not specify it.
 *
 * "idex=noprobe"	: do not attempt to access/use this interface
 * "idex=base"		: probe for an interface at the addr specified,
 *				where "base" is usually 0x1f0 or 0x170
 *				and "ctl" is assumed to be "base"+0x206
 * "idex=base,ctl"	: specify both base and ctl
 * "idex=base,ctl,irq"	: specify base, ctl, and irq number
 * "idex=autotune"	: driver will attempt to tune interface speed
 *				to the fastest PIO mode supported,
 *				for all drives on this interface.
 *				Not fully supported by all chipset types,
 *				and quite likely to cause trouble with
 *				older/odd IDE drives.
 * "idex=noautotune"	: driver will NOT attempt to tune interface speed
 *				This is the default for most chipsets,
 *				except the cmd640.
 * "idex=serialize"	: do not overlap operations on idex and ide(x^1)
 * "idex=four"		: four drives on idex and ide(x^1) share same ports
 * "idex=reset"		: reset interface before first use
 * "idex=dma"		: enable DMA by default on both drives if possible
 * "idex=ata66"		: informs the interface that it has an 80c cable
 *				for chipsets that are ATA-66 capable, but
 *				the ablity to bit test for detection is
 *				currently unknown.
 * "ide=reverse"	: Formerly called to pci sub-system, but now local.
 *
 * The following are valid ONLY on ide0, (except dc4030)
 * and the defaults for the base,ctl ports must not be altered.
 *
 * "ide0=dtc2278"	: probe/support DTC2278 interface
 * "ide0=ht6560b"	: probe/support HT6560B interface
 * "ide0=cmd640_vlb"	: *REQUIRED* for VLB cards with the CMD640 chip
 *			  (not for PCI -- automatically detected)
 * "ide0=qd65xx"	: probe/support qd65xx interface
 * "ide0=ali14xx"	: probe/support ali14xx chipsets (ALI M1439, M1443, M1445)
 * "ide0=umc8672"	: probe/support umc8672 chipsets
 * "idex=dc4030"	: probe/support Promise DC4030VL interface
 * "ide=doubler"	: probe/support IDE doublers on Amiga
 */
int __init ide_setup (char *s)
{
	int i, vals[3];
	ide_hwif_t *hwif;
	ide_drive_t *drive;
	unsigned int hw, unit;
	const char max_drive = 'a' + ((MAX_HWIFS * MAX_DRIVES) - 1);
	const char max_hwif  = '0' + (MAX_HWIFS - 1);

	
	if (strncmp(s,"hd",2) == 0 && s[2] == '=')	/* hd= is for hd.c   */
		return 0;				/* driver and not us */

	if (strncmp(s,"ide",3) &&
	    strncmp(s,"idebus",6) &&
	    strncmp(s,"hd",2))		/* hdx= & hdxlun= */
		return 0;

	printk("ide_setup: %s", s);
	init_ide_data ();

#ifdef CONFIG_BLK_DEV_IDEDOUBLER
	if (!strcmp(s, "ide=doubler")) {
		extern int ide_doubler;

		printk(" : Enabled support for IDE doublers\n");
		ide_doubler = 1;
		return 1;
	}
#endif /* CONFIG_BLK_DEV_IDEDOUBLER */

	if (!strcmp(s, "ide=nodma")) {
		printk("IDE: Prevented DMA\n");
		noautodma = 1;
		return 1;
	}

#ifdef CONFIG_BLK_DEV_IDEPCI
	if (!strcmp(s, "ide=reverse")) {
		ide_scan_direction = 1;
		printk(" : Enabled support for IDE inverse scan order.\n");
		return 1;
	}
#endif /* CONFIG_BLK_DEV_IDEPCI */

	/*
	 * Look for drive options:  "hdx="
	 */
	if (s[0] == 'h' && s[1] == 'd' && s[2] >= 'a' && s[2] <= max_drive) {
		const char *hd_words[] = {"none", "noprobe", "nowerr", "cdrom",
				"serialize", "autotune", "noautotune",
				"slow", "swapdata", "bswap", "flash",
				"remap", "noremap", "scsi", NULL};
		unit = s[2] - 'a';
		hw   = unit / MAX_DRIVES;
		unit = unit % MAX_DRIVES;
		hwif = &ide_hwifs[hw];
		drive = &hwif->drives[unit];
		if (strncmp(s + 4, "ide-", 4) == 0) {
			strncpy(drive->driver_req, s + 4, 9);
			goto done;
		}
		/*
		 * Look for last lun option:  "hdxlun="
		 */
		if (s[3] == 'l' && s[4] == 'u' && s[5] == 'n') {
			if (match_parm(&s[6], NULL, vals, 1) != 1)
				goto bad_option;
			if (vals[0] >= 0 && vals[0] <= 7) {
				drive->last_lun = vals[0];
				drive->forced_lun = 1;
			} else
				printk(" -- BAD LAST LUN! Expected value from 0 to 7");
			goto done;
		}
		switch (match_parm(&s[3], hd_words, vals, 3)) {
			case -1: /* "none" */
				drive->nobios = 1;  /* drop into "noprobe" */
			case -2: /* "noprobe" */
				drive->noprobe = 1;
				goto done;
			case -3: /* "nowerr" */
				drive->bad_wstat = BAD_R_STAT;
				hwif->noprobe = 0;
				goto done;
			case -4: /* "cdrom" */
				drive->present = 1;
				drive->media = ide_cdrom;
				hwif->noprobe = 0;
				goto done;
			case -5: /* "serialize" */
				printk(" -- USE \"ide%d=serialize\" INSTEAD", hw);
				goto do_serialize;
			case -6: /* "autotune" */
				drive->autotune = 1;
				goto done;
			case -7: /* "noautotune" */
				drive->autotune = 2;
				goto done;
			case -8: /* "slow" */
				drive->slow = 1;
				goto done;
			case -9: /* "swapdata" or "bswap" */
			case -10:
				drive->bswap = 1;
				goto done;
			case -11: /* "flash" */
				drive->ata_flash = 1;
				goto done;
			case -12: /* "remap" */
				drive->remap_0_to_1 = 1;
				goto done;
			case -13: /* "noremap" */
				drive->remap_0_to_1 = 2;
				goto done;
			case -14: /* "scsi" */
#if defined(CONFIG_BLK_DEV_IDESCSI) && defined(CONFIG_SCSI)
				drive->scsi = 1;
				goto done;
#else
				drive->scsi = 0;
				goto bad_option;
#endif /* defined(CONFIG_BLK_DEV_IDESCSI) && defined(CONFIG_SCSI) */
			case 3: /* cyl,head,sect */
				drive->media	= ide_disk;
				drive->cyl	= drive->bios_cyl  = vals[0];
				drive->head	= drive->bios_head = vals[1];
				drive->sect	= drive->bios_sect = vals[2];
				drive->present	= 1;
				drive->forced_geom = 1;
				hwif->noprobe = 0;
				goto done;
			default:
				goto bad_option;
		}
	}

	if (s[0] != 'i' || s[1] != 'd' || s[2] != 'e')
		goto bad_option;
	/*
	 * Look for bus speed option:  "idebus="
	 */
	if (s[3] == 'b' && s[4] == 'u' && s[5] == 's') {
		if (match_parm(&s[6], NULL, vals, 1) != 1)
			goto bad_option;
		if (vals[0] >= 20 && vals[0] <= 66) {
			idebus_parameter = vals[0];
		} else
			printk(" -- BAD BUS SPEED! Expected value from 20 to 66");
		goto done;
	}
	/*
	 * Look for interface options:  "idex="
	 */
	if (s[3] >= '0' && s[3] <= max_hwif) {
		/*
		 * Be VERY CAREFUL changing this: note hardcoded indexes below
		 * -8,-9,-10 : are reserved for future idex calls to ease the hardcoding.
		 */
		const char *ide_words[] = {
			"noprobe", "serialize", "autotune", "noautotune", "reset", "dma", "ata66",
			"minus8", "minus9", "minus10",
			"four", "qd65xx", "ht6560b", "cmd640_vlb", "dtc2278", "umc8672", "ali14xx", "dc4030", NULL };
		hw = s[3] - '0';
		hwif = &ide_hwifs[hw];
		i = match_parm(&s[4], ide_words, vals, 3);

		/*
		 * Cryptic check to ensure chipset not already set for hwif:
		 */
		if (i > 0 || i <= -11) {			/* is parameter a chipset name? */
			if (hwif->chipset != ide_unknown)
				goto bad_option;	/* chipset already specified */
			if (i <= -11 && i != -18 && hw != 0)
				goto bad_hwif;		/* chipset drivers are for "ide0=" only */
			if (i <= -11 && i != -18 && ide_hwifs[hw+1].chipset != ide_unknown)
				goto bad_option;	/* chipset for 2nd port already specified */
			printk("\n");
		}

		switch (i) {
#ifdef CONFIG_BLK_DEV_PDC4030
			case -18: /* "dc4030" */
			{
				extern void init_pdc4030(void);
				init_pdc4030();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_PDC4030 */
#ifdef CONFIG_BLK_DEV_ALI14XX
			case -17: /* "ali14xx" */
			{
				extern void init_ali14xx (void);
				init_ali14xx();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_ALI14XX */
#ifdef CONFIG_BLK_DEV_UMC8672
			case -16: /* "umc8672" */
			{
				extern void init_umc8672 (void);
				init_umc8672();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_UMC8672 */
#ifdef CONFIG_BLK_DEV_DTC2278
			case -15: /* "dtc2278" */
			{
				extern void init_dtc2278 (void);
				init_dtc2278();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_DTC2278 */
#ifdef CONFIG_BLK_DEV_CMD640
			case -14: /* "cmd640_vlb" */
			{
				extern int cmd640_vlb; /* flag for cmd640.c */
				cmd640_vlb = 1;
				goto done;
			}
#endif /* CONFIG_BLK_DEV_CMD640 */
#ifdef CONFIG_BLK_DEV_HT6560B
			case -13: /* "ht6560b" */
			{
				extern void init_ht6560b (void);
				init_ht6560b();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_HT6560B */
#if CONFIG_BLK_DEV_QD65XX
			case -12: /* "qd65xx" */
			{
				extern void init_qd65xx (void);
				init_qd65xx();
				goto done;
			}
#endif /* CONFIG_BLK_DEV_QD65XX */
#ifdef CONFIG_BLK_DEV_4DRIVES
			case -11: /* "four" drives on one set of ports */
			{
				ide_hwif_t *mate = &ide_hwifs[hw^1];
				mate->drives[0].select.all ^= 0x20;
				mate->drives[1].select.all ^= 0x20;
				hwif->chipset = mate->chipset = ide_4drives;
				mate->irq = hwif->irq;
				memcpy(mate->io_ports, hwif->io_ports, sizeof(hwif->io_ports));
				goto do_serialize;
			}
#endif /* CONFIG_BLK_DEV_4DRIVES */
			case -10: /* minus10 */
			case -9: /* minus9 */
			case -8: /* minus8 */
				goto bad_option;
			case -7: /* ata66 */
#ifdef CONFIG_BLK_DEV_IDEPCI
				hwif->udma_four = 1;
				goto done;
#else /* !CONFIG_BLK_DEV_IDEPCI */
				hwif->udma_four = 0;
				goto bad_hwif;
#endif /* CONFIG_BLK_DEV_IDEPCI */
			case -6: /* dma */
				hwif->autodma = 1;
				goto done;
			case -5: /* "reset" */
				hwif->reset = 1;
				goto done;
			case -4: /* "noautotune" */
				hwif->drives[0].autotune = 2;
				hwif->drives[1].autotune = 2;
				goto done;
			case -3: /* "autotune" */
				hwif->drives[0].autotune = 1;
				hwif->drives[1].autotune = 1;
				goto done;
			case -2: /* "serialize" */
			do_serialize:
				hwif->mate = &ide_hwifs[hw^1];
				hwif->mate->mate = hwif;
				hwif->serialized = hwif->mate->serialized = 1;
				goto done;

			case -1: /* "noprobe" */
				hwif->noprobe = 1;
				goto done;

			case 1:	/* base */
				vals[1] = vals[0] + 0x206; /* default ctl */
			case 2: /* base,ctl */
				vals[2] = 0;	/* default irq = probe for it */
			case 3: /* base,ctl,irq */
				hwif->hw.irq = vals[2];
				ide_init_hwif_ports(&hwif->hw, (ide_ioreg_t) vals[0], (ide_ioreg_t) vals[1], &hwif->irq);
				memcpy(hwif->io_ports, hwif->hw.io_ports, sizeof(hwif->io_ports));
				hwif->irq      = vals[2];
				hwif->noprobe  = 0;
				hwif->chipset  = ide_generic;
				goto done;

			case 0: goto bad_option;
			default:
				printk(" -- SUPPORT NOT CONFIGURED IN THIS KERNEL\n");
				return 1;
		}
	}
bad_option:
	printk(" -- BAD OPTION\n");
	return 1;
bad_hwif:
	printk("-- NOT SUPPORTED ON ide%d", hw);
done:
	printk("\n");
	return 1;
}

/*
 * probe_for_hwifs() finds/initializes "known" IDE interfaces
 */
static void __init probe_for_hwifs (void)
{
#ifdef CONFIG_PCI
	if (pci_present())
	{
#ifdef CONFIG_BLK_DEV_IDEPCI
		ide_scan_pcibus(ide_scan_direction);
#else
#ifdef CONFIG_BLK_DEV_RZ1000
		{
			extern void ide_probe_for_rz100x(void);
			ide_probe_for_rz100x();
		}
#endif /* CONFIG_BLK_DEV_RZ1000 */
#endif /* CONFIG_BLK_DEV_IDEPCI */
	}
#endif /* CONFIG_PCI */

#ifdef CONFIG_ETRAX_IDE
	{
		extern void init_e100_ide(void);
		init_e100_ide();
	}
#endif /* CONFIG_ETRAX_IDE */
#ifdef CONFIG_BLK_DEV_CMD640
	{
		extern void ide_probe_for_cmd640x(void);
		ide_probe_for_cmd640x();
	}
#endif /* CONFIG_BLK_DEV_CMD640 */
#ifdef CONFIG_BLK_DEV_PDC4030
	{
		extern int ide_probe_for_pdc4030(void);
		(void) ide_probe_for_pdc4030();
	}
#endif /* CONFIG_BLK_DEV_PDC4030 */
#ifdef CONFIG_BLK_DEV_IDE_PMAC
	{
		extern void pmac_ide_probe(void);
		pmac_ide_probe();
	}
#endif /* CONFIG_BLK_DEV_IDE_PMAC */
#ifdef CONFIG_BLK_DEV_IDE_ICSIDE
	{
		extern void icside_init(void);
		icside_init();
	}
#endif /* CONFIG_BLK_DEV_IDE_ICSIDE */
#ifdef CONFIG_BLK_DEV_IDE_RAPIDE
	{
		extern void rapide_init(void);
		rapide_init();
	}
#endif /* CONFIG_BLK_DEV_IDE_RAPIDE */
#ifdef CONFIG_BLK_DEV_GAYLE
	{
		extern void gayle_init(void);
		gayle_init();
	}
#endif /* CONFIG_BLK_DEV_GAYLE */
#ifdef CONFIG_BLK_DEV_FALCON_IDE
	{
		extern void falconide_init(void);
		falconide_init();
	}
#endif /* CONFIG_BLK_DEV_FALCON_IDE */
#ifdef CONFIG_BLK_DEV_MAC_IDE
	{
		extern void macide_init(void);
		macide_init();
	}
#endif /* CONFIG_BLK_DEV_MAC_IDE */
#ifdef CONFIG_BLK_DEV_Q40IDE
	{
		extern void q40ide_init(void);
		q40ide_init();
	}
#endif /* CONFIG_BLK_DEV_Q40IDE */
#ifdef CONFIG_BLK_DEV_BUDDHA
	{
		extern void buddha_init(void);
		buddha_init();
	}
#endif /* CONFIG_BLK_DEV_BUDDHA */
#if defined(CONFIG_BLK_DEV_ISAPNP) && defined(CONFIG_ISAPNP)
	{
		extern void pnpide_init(int enable);
		pnpide_init(1);
	}
#endif /* CONFIG_BLK_DEV_ISAPNP */
}

void __init ide_init_builtin_drivers (void)
{
	/*
	 * Probe for special PCI and other "known" interface chipsets
	 */
	probe_for_hwifs ();

#ifdef CONFIG_BLK_DEV_IDE
#if defined(__mc68000__) || defined(CONFIG_APUS)
	if (ide_hwifs[0].io_ports[IDE_DATA_OFFSET]) {
		ide_get_lock(&ide_lock, NULL, NULL);	/* for atari only */
		disable_irq(ide_hwifs[0].irq);	/* disable_irq_nosync ?? */
//		disable_irq_nosync(ide_hwifs[0].irq);
	}
#endif /* __mc68000__ || CONFIG_APUS */

	(void) ideprobe_init();

#if defined(__mc68000__) || defined(CONFIG_APUS)
	if (ide_hwifs[0].io_ports[IDE_DATA_OFFSET]) {
		enable_irq(ide_hwifs[0].irq);
		ide_release_lock(&ide_lock);	/* for atari only */
	}
#endif /* __mc68000__ || CONFIG_APUS */
#endif /* CONFIG_BLK_DEV_IDE */

#ifdef CONFIG_PROC_FS
	proc_ide_create();
#endif

	/*
	 * Attempt to match drivers for the available drives
	 */
#ifdef CONFIG_BLK_DEV_IDEDISK
	(void) idedisk_init();
#endif /* CONFIG_BLK_DEV_IDEDISK */
#ifdef CONFIG_BLK_DEV_IDECD
	(void) ide_cdrom_init();
#endif /* CONFIG_BLK_DEV_IDECD */
#ifdef CONFIG_BLK_DEV_IDETAPE
	(void) idetape_init();
#endif /* CONFIG_BLK_DEV_IDETAPE */
#ifdef CONFIG_BLK_DEV_IDEFLOPPY
	(void) idefloppy_init();
#endif /* CONFIG_BLK_DEV_IDEFLOPPY */
#ifdef CONFIG_BLK_DEV_IDESCSI
 #ifdef CONFIG_SCSI
	(void) idescsi_init();
 #else
    #warning ide scsi-emulation selected but no SCSI-subsystem in kernel
 #endif
#endif /* CONFIG_BLK_DEV_IDESCSI */
}

static int default_cleanup (ide_drive_t *drive)
{
	return ide_unregister_subdriver(drive);
}

static ide_startstop_t default_do_request(ide_drive_t *drive, struct request *rq, unsigned long block)
{
	ide_end_request(0, HWGROUP(drive));
	return ide_stopped;
}
 
static void default_end_request (byte uptodate, ide_hwgroup_t *hwgroup)
{
	ide_end_request(uptodate, hwgroup);
}
  
static int default_ioctl (ide_drive_t *drive, struct inode *inode, struct file *file,
			  unsigned int cmd, unsigned long arg)
{
	return -EIO;
}

static int default_open (struct inode *inode, struct file *filp, ide_drive_t *drive)
{
	drive->usage--;
	return -EIO;
}

static void default_release (struct inode *inode, struct file *filp, ide_drive_t *drive)
{
}

static int default_check_media_change (ide_drive_t *drive)
{
	return 1;
}

static void default_pre_reset (ide_drive_t *drive)
{
}

static unsigned long default_capacity (ide_drive_t *drive)
{
	return 0x7fffffff;
}

static ide_startstop_t default_special (ide_drive_t *drive)
{
	special_t *s = &drive->special;

	s->all = 0;
	drive->mult_req = 0;
	return ide_stopped;
}

static int default_driver_reinit (ide_drive_t *drive)
{
	printk(KERN_ERR "%s: does not support hotswap of device class !\n", drive->name);

	return 0;
}

static void setup_driver_defaults (ide_drive_t *drive)
{
	ide_driver_t *d = drive->driver;

	if (d->cleanup == NULL)		d->cleanup = default_cleanup;
	if (d->do_request == NULL)	d->do_request = default_do_request;
	if (d->end_request == NULL)	d->end_request = default_end_request;
	if (d->ioctl == NULL)		d->ioctl = default_ioctl;
	if (d->open == NULL)		d->open = default_open;
	if (d->release == NULL)		d->release = default_release;
	if (d->media_change == NULL)	d->media_change = default_check_media_change;
	if (d->pre_reset == NULL)	d->pre_reset = default_pre_reset;
	if (d->capacity == NULL)	d->capacity = default_capacity;
	if (d->special == NULL)		d->special = default_special;
	if (d->driver_reinit == NULL)	d->driver_reinit = default_driver_reinit;
}

ide_drive_t *ide_scan_devices (byte media, const char *name, ide_driver_t *driver, int n)
{
	unsigned int unit, index, i;

	for (index = 0, i = 0; index < MAX_HWIFS; ++index) {
		ide_hwif_t *hwif = &ide_hwifs[index];
		if (!hwif->present)
			continue;
		for (unit = 0; unit < MAX_DRIVES; ++unit) {
			ide_drive_t *drive = &hwif->drives[unit];
			char *req = drive->driver_req;
			if (*req && !strstr(name, req))
				continue;
			if (drive->present && drive->media == media && drive->driver == driver && ++i > n)
				return drive;
		}
	}
	return NULL;
}

int ide_register_subdriver (ide_drive_t *drive, ide_driver_t *driver, int version)
{
	unsigned long flags;
	
	save_flags(flags);		/* all CPUs */
	cli();				/* all CPUs */
	if (version != IDE_SUBDRIVER_VERSION || !drive->present || drive->driver != NULL || drive->busy || drive->usage) {
		restore_flags(flags);	/* all CPUs */
		return 1;
	}
	drive->driver = driver;
	setup_driver_defaults(drive);
	restore_flags(flags);		/* all CPUs */
	if (drive->autotune != 2) {
		if (driver->supports_dma && HWIF(drive)->dmaproc != NULL) {
			/*
			 * Force DMAing for the beginning of the check.
			 * Some chipsets appear to do interesting things,
			 * if not checked and cleared.
			 *   PARANOIA!!!
			 */
			(void) (HWIF(drive)->dmaproc(ide_dma_off_quietly, drive));
			(void) (HWIF(drive)->dmaproc(ide_dma_check, drive));
		}
		drive->dsc_overlap = (drive->next != drive && driver->supports_dsc_overlap);
		drive->nice1 = 1;
	}
	drive->revalidate = 1;
	drive->suspend_reset = 0;
#ifdef CONFIG_PROC_FS
	ide_add_proc_entries(drive->proc, generic_subdriver_entries, drive);
	ide_add_proc_entries(drive->proc, driver->proc, drive);
#endif
	return 0;
}

int ide_unregister_subdriver (ide_drive_t *drive)
{
	unsigned long flags;
	
	save_flags(flags);		/* all CPUs */
	cli();				/* all CPUs */
	if (drive->usage || drive->busy || drive->driver == NULL || DRIVER(drive)->busy) {
		restore_flags(flags);	/* all CPUs */
		return 1;
	}
#if defined(CONFIG_BLK_DEV_ISAPNP) && defined(CONFIG_ISAPNP) && defined(MODULE)
	pnpide_init(0);
#endif /* CONFIG_BLK_DEV_ISAPNP */
#ifdef CONFIG_PROC_FS
	ide_remove_proc_entries(drive->proc, DRIVER(drive)->proc);
	ide_remove_proc_entries(drive->proc, generic_subdriver_entries);
#endif
	auto_remove_settings(drive);
	drive->driver = NULL;
	restore_flags(flags);		/* all CPUs */
	return 0;
}

int ide_register_module (ide_module_t *module)
{
	ide_module_t *p = ide_modules;

	while (p) {
		if (p == module)
			return 1;
		p = p->next;
	}
	module->next = ide_modules;
	ide_modules = module;
	revalidate_drives();
	return 0;
}

void ide_unregister_module (ide_module_t *module)
{
	ide_module_t **p;

	for (p = &ide_modules; (*p) && (*p) != module; p = &((*p)->next));
	if (*p)
		*p = (*p)->next;
}

struct block_device_operations ide_fops[] = {{
	owner:			THIS_MODULE,
	open:			ide_open,
	release:		ide_release,
	ioctl:			ide_ioctl,
	check_media_change:	ide_check_media_change,
	revalidate:		ide_revalidate_disk
}};

EXPORT_SYMBOL(ide_hwifs);
EXPORT_SYMBOL(ide_register_module);
EXPORT_SYMBOL(ide_unregister_module);
EXPORT_SYMBOL(ide_spin_wait_hwgroup);

/*
 * Probe module
 */
devfs_handle_t ide_devfs_handle;

EXPORT_SYMBOL(ide_probe);
EXPORT_SYMBOL(drive_is_flashcard);
EXPORT_SYMBOL(ide_timer_expiry);
EXPORT_SYMBOL(ide_intr);
EXPORT_SYMBOL(ide_fops);
EXPORT_SYMBOL(ide_get_queue);
EXPORT_SYMBOL(ide_add_generic_settings);
EXPORT_SYMBOL(ide_devfs_handle);
EXPORT_SYMBOL(do_ide_request);
/*
 * Driver module
 */
EXPORT_SYMBOL(ide_scan_devices);
EXPORT_SYMBOL(ide_register_subdriver);
EXPORT_SYMBOL(ide_unregister_subdriver);
EXPORT_SYMBOL(ide_replace_subdriver);
EXPORT_SYMBOL(ide_input_data);
EXPORT_SYMBOL(ide_output_data);
EXPORT_SYMBOL(atapi_input_bytes);
EXPORT_SYMBOL(atapi_output_bytes);
EXPORT_SYMBOL(ide_set_handler);
EXPORT_SYMBOL(ide_dump_status);
EXPORT_SYMBOL(ide_error);
EXPORT_SYMBOL(ide_fixstring);
EXPORT_SYMBOL(ide_wait_stat);
EXPORT_SYMBOL(ide_do_reset);
EXPORT_SYMBOL(restart_request);
EXPORT_SYMBOL(ide_init_drive_cmd);
EXPORT_SYMBOL(ide_do_drive_cmd);
EXPORT_SYMBOL(ide_end_drive_cmd);
EXPORT_SYMBOL(ide_end_request);
EXPORT_SYMBOL(ide_revalidate_disk);
EXPORT_SYMBOL(ide_cmd);
EXPORT_SYMBOL(ide_wait_cmd);
EXPORT_SYMBOL(ide_wait_cmd_task);
EXPORT_SYMBOL(ide_delay_50ms);
EXPORT_SYMBOL(ide_stall_queue);
#ifdef CONFIG_PROC_FS
EXPORT_SYMBOL(ide_add_proc_entries);
EXPORT_SYMBOL(ide_remove_proc_entries);
EXPORT_SYMBOL(proc_ide_read_geometry);
EXPORT_SYMBOL(create_proc_ide_interfaces);
#endif
EXPORT_SYMBOL(ide_add_setting);
EXPORT_SYMBOL(ide_remove_setting);

EXPORT_SYMBOL(ide_register_hw);
EXPORT_SYMBOL(ide_register);
EXPORT_SYMBOL(ide_unregister);
EXPORT_SYMBOL(ide_setup_ports);
EXPORT_SYMBOL(hwif_unregister);
EXPORT_SYMBOL(get_info_ptr);
EXPORT_SYMBOL(current_capacity);

EXPORT_SYMBOL(system_bus_clock);

/*
 * This is gets invoked once during initialization, to set *everything* up
 */
int __init ide_init (void)
{
	static char banner_printed;
	int i;

	if (!banner_printed) {
		printk(KERN_INFO "Uniform Multi-Platform E-IDE driver " REVISION "\n");
		ide_devfs_handle = devfs_mk_dir (NULL, "ide", NULL);
		system_bus_speed = ide_system_bus_speed();
		banner_printed = 1;
	}

	init_ide_data ();

	initializing = 1;
	ide_init_builtin_drivers();
	initializing = 0;

	for (i = 0; i < MAX_HWIFS; ++i) {
		ide_hwif_t  *hwif = &ide_hwifs[i];
		if (hwif->present)
			ide_geninit(hwif);
	}

	ide_vmsinit();

	return 0;
}

#ifdef MODULE
char *options = NULL;
MODULE_PARM(options,"s");
MODULE_LICENSE("GPL");

static void __init parse_options (char *line)
{
	char *next = line;

	if (line == NULL || !*line)
		return;
	while ((line = next) != NULL) {
 		if ((next = strchr(line,' ')) != NULL)
			*next++ = 0;
		if (!ide_setup(line))
			printk ("Unknown option '%s'\n", line);
	}
}

int init_module (void)
{
	parse_options(options);
	return ide_init();
}

void cleanup_module (void)
{
	int index;

	for (index = 0; index < MAX_HWIFS; ++index) {
		ide_unregister(index);
#if defined(CONFIG_BLK_DEV_IDEDMA) && !defined(CONFIG_DMA_NONPCI)
		if (ide_hwifs[index].dma_base)
			(void) ide_release_dma(&ide_hwifs[index]);
#endif /* (CONFIG_BLK_DEV_IDEDMA) && !(CONFIG_DMA_NONPCI) */
	}

#ifdef CONFIG_PROC_FS
	proc_ide_destroy();
#endif
	devfs_unregister (ide_devfs_handle);
}

#else /* !MODULE */

__setup("", ide_setup);

#endif /* MODULE */
