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
  outb (0x0E, atactl + ATACTL_BW_DEVCTL);					// software reset the drives, disable interrupts
  if (gibp -> ctrlr != NULL) {
    oz_hw_stl_nanowait (50000000);						// wait 50mS
    outb (0x08, atactl + ATACTL_BW_DEVCTL);				// release the drives, enable interrupts
    oz_hw_stl_nanowait (50000000);						// wait 50mS

    /* Set up data transfer mode and other controller stuff */

    if (chipidx >= 0) {
      (*(chips[chipidx].setup)) (pciconfp, cablesel, gibp -> ctrlr, gibp -> devexs[0], gibp -> devexs[1]);
    }

    /* Enable the drives */

    for (drive_id = 0; drive_id < 2; drive_id ++) {
      if (gibp -> devexs[drive_id] == NULL) continue;				// see if we found anything in the scan
      outb ((drive_id << 4) | 0xA0, atacmd + ATACMD_B_DRHEAD);		// select the drive to be initialized
      oz_hw_stl_nanowait (400);
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

  outb (0x0E, gibp -> atactl + ATACTL_BW_DEVCTL);				// software reset the drives
  oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb (0x0A, gibp -> atactl + ATACTL_BW_DEVCTL);				// release the drives
  oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb ((drive_id << 4) | 0xA0, gibp -> atacmd + ATACMD_B_DRHEAD);	// select the drive to be identified
  oz_hw_stl_nanowait (400);

  /* Recal to see if there is an ATA drive there */

  outb (0x10, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to recalibrate
  oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
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

  outb (0xEC, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
  oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
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
  outb (0x91, gibp -> atacmd + ATACMD_BW_COMMAND);				// initialize drive parameters
  oz_hw_stl_nanowait (400);								// give the drive 400nS to start processing the command
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
      outb (0xC6, gibp -> atacmd + ATACMD_BW_COMMAND);			/* tell it to set the multiple block count */
      oz_hw_stl_nanowait (400);							/* give the drive 400nS to start processing the command */
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

  outb (0x0E, gibp -> atactl + ATACTL_BW_DEVCTL);				// software reset the drives
  oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb (0x0A, gibp -> atactl + ATACTL_BW_DEVCTL);				// release the drives
  oz_hw_stl_nanowait (50000000);						// wait 50mS
  outb ((drive_id << 4) | 0xA0, gibp -> atacmd + ATACMD_B_DRHEAD);	// select the drive to be identified
  oz_hw_stl_nanowait (400);

  /* Try to tell ATAPI drive to identify itself */

  outb (0xA1, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
  oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
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

  outb (0xA1, gibp -> atacmd + ATACMD_BW_COMMAND);			// tell it to identify itself
  oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing the command
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
    outb (0xEF, atacmd + ATACMD_BW_COMMAND);				// start setting the features
    oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing
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
    outb (0xEF, atacmd + ATACMD_BW_COMMAND);				// start setting the features
    oz_hw_stl_nanowait (400);							// give the drive 400nS to start processing
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
  status = inb (atactl + ATACTL_BR_ALTSTS);		// read status w/out acking interrupt
  if (status & 0x80) return (0);				// if still bussy, continue waiting
  oz_hw_stl_nanowait (400);					// give it a chance to update status
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
     
     static unsigned long ata_start (struct _irp * i, struct _ucb * u)

{
  struct _ucb *devunit;
  void *devexv;
  struct _ccb   *iochan;
  void *chnexv;
  OZ_Procmode procmode;
  struct _irp *ioop;
  void *iopexv;
  unsigned long funcode;
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

      iopex -> atacmdcode = (disk_readblocks.inhretries & 1) ? devex -> atacmd_read_inh : devex -> atacmd_read;
      iopex -> writedisk  = 0;
      if (sts == SS$_NORMAL) sts = ata_queuereq (disk_readblocks.size, disk_readblocks.slbn, phypages, byteoffs, iopex);
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

int ubd_vmsinit(void) {
  unsigned short chan;
  $DESCRIPTOR(u0,"ubd0");
  struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"ubd0","ubdriver");
  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&fdt,IO$_READLBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt,IO$_READPBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt,IO$_READVBLK,acp_std$readblk,1);
  exe$assign(&u0,&chan,0,0,0);
  registerdevchan(MKDEV(UBD_MAJOR,0),chan);
  return chan;
}

