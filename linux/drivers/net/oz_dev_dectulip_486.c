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
/*  Digital 21041/21140A ethernet driver				*/
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

#if 0
#include "ozone.h"

#include "oz_dev_pci.h"
#include "oz_dev_timer.h"
#include "oz_knl_devio.h"
#include "oz_knl_event.h"
#include "oz_knl_hw.h"
#include "oz_knl_kmalloc.h"
#include "oz_knl_misc.h"
#include "oz_knl_phymem.h"
#include "oz_knl_procmode.h"
#include "oz_knl_sdata.h"
#include "oz_knl_section.h"
#include "oz_knl_spte.h"
#include "oz_knl_status.h"
#include "oz_io_ether.h"
#include "oz_sys_xprintf.h"
#endif

#define ARPHWTYPE 1
#define ADDRSIZE 6
#define PROTOSIZE 2
#define DATASIZE 1500
#define BUFFSIZE (2*ADDRSIZE+PROTOSIZE+DATASIZE+4)

#define N2HW(__nw) (((__nw)[0] << 8) + (__nw)[1])
#define CEQENADDR(__ea1,__ea2) (memcmp (__ea1, __ea2, ADDRSIZE) == 0)

typedef struct Chnex Chnex;
typedef struct Devex Devex;
typedef struct Iopex Iopex;
typedef struct Rxbuf Rxbuf;
typedef struct Txbuf Txbuf;

/* Controller parameters */

#define CACHE_ALIGNMENT 2	/* 1=cache align at 8lws, 2=16lws, 3=32lws */
#define BURST_LENGTH 16		/* max longwords to transfer at a time (don't use 32 - bug in old Tulips) */
#define TRANSMIT_THRESHOLD 2	/* 128 bytes */

/* Format of ethernet buffers */

typedef struct { uByte dstaddr[ADDRSIZE];
                 uByte srcaddr[ADDRSIZE];
                 uByte proto[PROTOSIZE];
                 uByte data[DATASIZE];
                 uByte crc[4];
               } Ether_buf;

/* Format of receive buffers */

struct Rxbuf { Rxbuf *next;		/* next in receive list */
               Long refcount;		/* ref count (number of iopex -> rxbuf's pointing to it) */
               uLong rdes0_physaddr;	/* physaddr of rdes0 */
					/* rdes0..rdes3 must be physically contiguous and longword aligned */
               uLong rdes0;		/* ownership and receive status */
               uLong rdes1;		/* chain flag and buffer size */
               uLong rdes2;		/* data buffer pointer (physical address of buf.dstaddr) */
               uLong rdes3;		/* next rdes0 pointer (physical address of next rdes0) */
               uLong dlen;		/* length of received data (not incl header or crc) */
					/* buf must be physically contiguous, and buf.dstaddr must be longword aligned */
               Ether_buf buf;		/* ethernet receive data */
             };

/* Format of transmit data buffers */

struct Txbuf { Txbuf *next;		/* next in transmit list */
               Iopex *iopex;		/* associated iopex to post */
               uLong tdes0_physaddr;	/* physaddr of tdes0 */
               uLong buf_physaddr;	/* physaddr of buf.dstaddr */
					/* tdes0..tdes3 must be physically contiguous and longword aligned */
               uLong tdes0;		/* ownership and transmit status */
               uLong tdes1;		/* chain flag and next buffer size */
               uLong tdes2;		/* data buffer pointer (physical address of next buf.dstaddr) */
               uLong tdes3;		/* next tdes0 pointer (physical address of next tdes0) */
               uLong dlen;		/* length of data to transmit (not incl header or crc) */
					/* buf must be physically contiguous, and buf.dstaddr must be longword aligned */
               Ether_buf buf;		/* ethernet transmit data */
             };

/* Device extension structure */

#define DIDVID_21140A 0x00091011
#define DIDVID_21041  0x00141011

struct Devex { OZ_Devunit *devunit;		/* devunit pointer */
               const char *name;		/* devunit name string pointer */
               uLong ioaddr;			/* base I/O address (if odd, use I/O space, if even, use memory space) */
               uLong didvid;			/* 00091011 for 21140A, 00141011 for 21041 */
               uByte intvec;			/* irq vector */
               uByte pad1;
               uByte enaddr[ADDRSIZE];		/* hardware address */
               OZ_Smplock *smplock;		/* pointer to irq level smp lock */
						/* if NULL, there are no interrupts (loader environment) */

               Chnex *chnexs;			/* all open channels on the device */

               Long receive_buffers_queued;	/* number of buffers queued to device (not including the null terminating buffer) */
               Long receive_requests_queued;	/* number of receive I/O requests queued to all channels on device */

               Rxbuf *rcvbusyqf;		/* first buffer in receive queue list */
               Rxbuf *rcvbusyql;		/* last buffer in receive queue list */

               Txbuf *xmtbusyqf;		/* first buffer in transmit queue list */
               Txbuf *xmtbusyql;		/* last buffer in transmit queue list */

               OZ_Hw486_irq_many irq_many;	/* interrupt block */
             };

/* Channel extension structure */

struct Chnex { Chnex *next;			/* next in devex->chnexs list */
               uWord proto;			/* protocol number (or 0 for all) */
               uWord promis;			/* promiscuous mode (0 normal, 1 promiscuous) */

               /* Receive related data */

               uLong rcvmissed;			/* number of receive packets missed */
               Iopex *rxreqh;			/* list of pending receive requests */
               Iopex **rxreqt;			/* points to rxreqh if list empty */
						/* points to last one's iopex -> next if requests in queue */
						/* NULL if channel is closed */
             };

/* I/O extension structure */

struct Iopex { Iopex *next;				/* next in list of requests */
               OZ_Ioop *ioop;				/* I/O operation block pointer */
               OZ_Procmode procmode;			/* processor mode of request */
               Devex *devex;				/* pointer to device */

               /* Receive related data */

               OZ_IO_ether_receive receive;		/* receive request I/O parameters */
               Rxbuf *rxbuf;				/* pointer to buffer received */

               /* Transmit related data */

               OZ_IO_ether_transmit transmit;		/* transmit request I/O parameters */
               uLong status;				/* completion status */
               int retries;				/* number of remaining retries */
             };

/* Function table */

static int dectulip_shutdown (OZ_Devunit *devunit, void *devexv);
static uLong dectulip_assign (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv, OZ_Procmode procmode);
static int dectulip_deassign (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv);
static uLong dectulip_start (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv, OZ_Procmode procmode, 
                            OZ_Ioop *ioop, void *iopexv, uLong funcode, uLong as, void *ap);

static const OZ_Devfunc dectulip_functable = { sizeof (Devex), sizeof (Chnex), sizeof (Iopex), 0, 
                                               dectulip_shutdown, NULL, NULL, dectulip_assign, dectulip_deassign, NULL, dectulip_start, NULL };

/* Internal static data */

static int initialized = 0;
static OZ_Devclass *devclass;
static OZ_Devdriver *devdriver;
static Rxbuf *volatile free_rxbufs = NULL;	// atomic access only
static Txbuf *volatile free_txbufs = NULL;	// atomic access only
static uByte broadcast[ADDRSIZE];

/* Internal routines */

static uWord read_srom_uword (int as, uLong addr, uLong ioaddr);
static uWord calc_srom_crc (uByte *sromdata);
static void csr9_outl (uLong ioaddr, uLong csr9bits);
static uLong ether_enable (Devex *devex);
static void ether_disable (Devex *devex);
static int close_channel (Devex *devex, Chnex *chnexx, uLong dv);
static int ether_interrupt (void *devexv, OZ_Mchargs *mchargs);
static int receive_done (Devex *devex);
static void receive_iodone (void *iopexv, int finok, uLong *status_r);
static int transmit_done (Devex *devex);
static void turnondev (Devex *devex);
static Rxbuf *allocrcvbuf (void);
static void queuercvbuf (Devex *devex, Rxbuf *rxbuf);
static void freercvbuf (Rxbuf *rxbuf);
static Txbuf *allocxmtbuf (void);
static void queuexmtbuf (Devex *devex, Iopex *iopex, Txbuf *txbuf);
static void freexmtbuf (Txbuf *txbuf);
static void transmit_iodone (void *txbufv, int finok, uLong *status_r);
static uLong npp_virt_to_phys (uLong size, void *addr);
static uLong ether_inl (uLong ioaddr, uWord offset);
static void ether_outl (uLong value, uLong ioaddr, uWord offset);

/************************************************************************/
/*									*/
/*  I/O space registers							*/
/*									*/
/************************************************************************/

#define CSR0  0x00	/* bus mode register */
#define CSR1  0x08	/* transmit poll register */
#define CSR2  0x10	/* receive poll demand register */
#define CSR3  0x18	/* receive list base address register  */
#define CSR4  0x20	/* transmit list base address register */
#define CSR5  0x28	/* status register */
#define CSR6  0x30	/* operation mode register */
#define CSR7  0x38	/* interrupt mask register */
#define CSR8  0x40	/* missed frame counter register */
#define CSR9  0x48	/* boot and serial roms register */
#define CSR10 0x50	/* boot rom address register */
#define CSR11 0x58	/* general purpose timer register */
#define CSR12 0x60	/* 21041  - SIA status register */
			/* 21140A - general purpose port register */
#define CSR13 0x68	/* 21041  - SIA connectivity register */
			/* 21140A - do not access */
#define CSR14 0x70	/* 21041  - SIA transmit receive register */
			/* 21140A - do not access */
#define CSR15 0x78	/* 21041  - SIA general register */
			/* 21140A - watchdog timer */

#define CSR0_M_SWR 0x1	/* software reset */
#define CSR0_V_PBL 8	/* programmable burst length */
#define CSR0_V_CAL 14	/* cache alignment */

#define CSR5_SE  0x2000	/* system error */
#define CSR5_LNF 0x1000	/* link fail (21041 only) */
#define CSR5_RWT 0x0200	/* receive watchdog timeout */
#define CSR5_RPS 0x0100	/* receive process stopped */
#define CSR5_UNF 0x0020	/* transmit underflow */
#define CSR5_TJT 0x0008	/* transmit jabber timeout */
#define CSR5_TPS 0x0002	/* transmit process stopped */

#define CSR6_V_TR 14		/* threshold control bits */
#define CSR6_M_PR 0x0040	/* promiscuous mode */
#define CSR6_M_ST 0x2000	/* start transmission process */
#define CSR6_M_SR 0x0002	/* start reception process */

#define CSR7_NIM 0x10000	/* normal interrupt summary mask */
#define CSR7_AIM 0x08000	/* abnormal interrupt summary mask */
#define CSR7_SEM 0x02000	/* system error mask */
#define CSR7_LFM 0x01000	/* link fail mask (21041 only) */
#define CSR7_TMM 0x00800	/* timer expired mask */
#define CSR7_RWM 0x00200	/* receive watchdog timeout mask */
#define CSR7_RSM 0x00100	/* receive stopped mask */
#define CSR7_RIM 0x00040	/* receive interrupt mask */
#define CSR7_LPM 0x00010	/* link pass mask (21041 only) */
#define CSR7_TJM 0x00008	/* transmit jabber timeout mask */
#define CSR7_TSM 0x00002	/* transmission stopped mask */
#define CSR7_TIM 0x00001	/* transmit interrupt mask */

#define RDES0_M_OWN 0x80000000	/* set if owned by controller */
#define RDES0_M_FL  0x3FFF0000	/* frame length (including crc) */
				/* - the 21041 is 15 bits long */
				/* - the 21140A is 14 bits long */
#define RDES0_V_FL  16
#define RDES0_M_ES  0x8000	/* error summary */
#define RDES0_M_LE  0x4000	/* length error */
#define RDES0_M_RF  0x0800	/* runt frame */
#define RDES0_M_FS  0x0200	/* first descriptor */
#define RDES0_M_LS  0x0100	/* last descriptor */
#define RDES0_M_TL  0x0080	/* frame too long */
#define RDES0_M_CS  0x0040	/* collision seen */
#define RDES0_M_RJ  0x0010	/* receive watchdog */
#define RDES0_M_DB  0x0004	/* dribbling bit */
#define RDES0_M_CE  0x0002	/* crc error */
#define RDES0_M_OF  0x0001	/* overflow (21041 only, 21140A always returns 0) */

#define RDES1_M_RCH  0x01000000	/* second address chained */
#define RDES1_V_RBS1 0		/* buffer 1 size */

#define TDES0_OWN 0x80000000	/* set if owned by controller */
#define TDES0_ES  0x8000	/* error summary */
#define TDES0_TO  0x4000	/* transmit jabber timeout */
#define TDES0_LO  0x0800	/* loss of carrier */
#define TDES0_NC  0x0400	/* no carrier */
#define TDES0_LC  0x0200	/* late collision */
#define TDES0_EC  0x0100	/* excessive collisions */
#define TDES0_LF  0x0004	/* link fail */
#define TDES0_UF  0x0002	/* underflow error */

#define TDES1_M_IC   0x80000000	/* interrupt on completion */
#define TDES1_M_LS   0x40000000	/* last segment */
#define TDES1_M_FS   0x20000000	/* first segment */
#define TDES1_M_TCH  0x01000000	/* second address chained */
#define TDES1_V_TBS1 0		/* buffer 1 size */

/************************************************************************/
/*									*/
/*  Boot-time initialization routine					*/
/*									*/
/************************************************************************/

static const struct { uLong didvid;
                      const char *model;
                    } didvids[] = { DIDVID_21041, "DEC21041", 
                                   DIDVID_21140A, "DEC21140A", 
                                               0, NULL };

void oz_dev_dectulip_init ()

{
  char unitdesc[OZ_DEVUNIT_DESCSIZE], unitname[OZ_DEVUNIT_NAMESIZE];
  const char *extra, *model;
  Devex *devex;
  int as, didvididx, i, init, j;
  OZ_Dev_pci_conf_p pciconfp;
  OZ_Devunit *devunit;
  Ether_buf *buf;
  uByte enaddr[ADDRSIZE], intvec;
  uLong didvid, ioaddr;
  union { uByte b[128];
          uWord w[64];
        } srom;
  uWord pcicmd, srom_crc;

  if (initialized) return;

  oz_knl_printk ("oz_dev_dectulip_init\n");
  initialized = 1;

  memset (broadcast, -1, sizeof broadcast);		/* broadcast ethernet address */

  devclass  = oz_knl_devclass_create (OZ_IO_ETHER_CLASSNAME, OZ_IO_ETHER_BASE, OZ_IO_ETHER_MASK, "dectulip_486");
  devdriver = oz_knl_devdriver_create (devclass, "dectulip_486");

  /* Scan pci for dectulip chips */

  for (didvididx = 0; (didvid = didvids[didvididx].didvid) != 0; didvididx ++) for (init = 1; oz_dev_pci_conf_scan_didvid (&pciconfp, init, didvid); init = 0) {
    if (pciconfp.pcifunc != 0) continue;

    model = didvids[didvididx].model;
    oz_knl_printk ("oz_dev_dectulip: %s ethernet controller found: bus/device %u/%u\n", model, pciconfp.pcibus, pciconfp.pcidev, pciconfp.pcifunc);

    /* Create device unit struct and fill it in */

    oz_sys_sprintf (sizeof unitname, unitname, "dectulip_%u_%u", pciconfp.pcibus, pciconfp.pcidev); /* make up device name string */

    extra = oz_knl_misc_getextra (unitname, "enable");					/* see if it should be disabled */
    if (strcmp (extra, "disable") == 0) {
      oz_knl_printk ("oz_dev_dectulip: %s disabled via extra string\n", unitname);
      continue;
    }
    if (strcmp (extra, "enable") != 0) {
      oz_knl_printk ("oz_dev_dectulip: %s extra string '%s' invalid -- ignoring\n", unitname, extra);
      oz_knl_printk ("oz_dev_dectulip: set to ['enable'] or 'disable'\n");
    }

    pcicmd  = oz_dev_pci_conf_inw (&pciconfp, OZ_DEV_PCI_CONF_W_PCICMD);		/* make sure bus mastering is enabled */
    pcicmd |= OZ_DEV_PCI_CONF_PCICMD_ENAB_IO | OZ_DEV_PCI_CONF_PCICMD_ENAB_MAS;
    oz_dev_pci_conf_outw (pcicmd, &pciconfp, OZ_DEV_PCI_CONF_W_PCICMD);

    ioaddr = oz_dev_pci_conf_inl (&pciconfp, OZ_DEV_PCI_CONF_L_BASADR0);		/* get base I/O address */
    intvec = oz_dev_pci_conf_inb (&pciconfp, OZ_DEV_PCI_CONF_B_INTLINE);		/* get irq number */

    for (as = 6; as <= 8; as += 2) {							/* try 6-bit srom addresses then 8 */
      for (i = 0; i < (sizeof srom) / 2; i ++) {					/* read srom */
        srom.w[i] = read_srom_uword (as, i, ioaddr);
      }
      /* old style ethernet address */
      for (j = 0; j < 8; j ++) {							/* check for old-style srom format */
        if ((srom.b[j] != srom.b[15-j]) || (srom.b[j] != srom.b[16+j])) break;		/* ... the data is stored thusly */
      }
      if (j == 8) {
        memcpy (enaddr, srom.b, sizeof enaddr);						/* successful, save ethernet address */
        break;
      }
      /* new style ethernet address */
      srom_crc = calc_srom_crc (srom.b);						/* check for new-style srom format */
      if (srom_crc == srom.w[63]) {
        memcpy (enaddr, srom.b + 20, sizeof enaddr);					/* successful, save ethernet address */
        break;
      }
    }
    /* make sure we got it one way or another */
    if (as > 8) {
      oz_knl_printk ("oz_dev_dectulip: %s bad srom format\n", unitname);
      oz_knl_dumpmem (sizeof srom, &srom);
      continue;
    }

    oz_sys_sprintf (sizeof unitdesc, unitdesc, "%s port %X, irq %u, addr %2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X", 
	model, ioaddr, intvec, enaddr[0], enaddr[1], enaddr[2], enaddr[3], enaddr[4], enaddr[5]);

    devunit = oz_knl_devunit_create (devdriver, unitname, unitdesc, &dectulip_functable, 0, oz_s_secattr_sysdev); /* create device driver database entries */
    devex   = oz_knl_devunit_ex (devunit);

    devex -> devunit = devunit;								/* save devunit pointer */
    devex -> name    = oz_knl_devunit_devname (devunit);				/* save devname string pointer */
    devex -> didvid  = didvid;								/* save model ident */
    devex -> ioaddr  = ioaddr;								/* save I/O address */
    devex -> intvec  = intvec;								/* save IRQ number */
    devex -> irq_many.entry = ether_interrupt;						/* set up interrupt routine */
    devex -> irq_many.param = devex;
    devex -> irq_many.descr = oz_knl_devunit_devname (devunit);
    devex -> smplock = oz_hw486_irq_many_add (devex -> intvec, &(devex -> irq_many));
    memcpy (devex -> enaddr, enaddr, sizeof devex -> enaddr);				/* save the ethernet address */

    /* Start hardware going */

    ether_enable (devex);
  }
}

/************************************************************************/
/*									*/
/*  Read an unsigned word from the SROM at the given address		*/
/*									*/
/*    Input:								*/
/*									*/
/*	addr = word address in SROM (0..63)				*/
/*	ioaddr = base I/O address of ethernet controller		*/
/*									*/
/*    Output:								*/
/*									*/
/*	read_srom_uword = data read from SROM				*/
/*									*/
/*    Note:								*/
/*									*/
/*	We just use 250nS delays for everything to simplify coding	*/
/*	So what if we wasted a few extra uSecs during init		*/
/*									*/
/************************************************************************/

static uWord read_srom_uword (int as, uLong addr, uLong ioaddr)

{
  int i;
  uWord d;
  uWord readcmd;

  readcmd = addr | (6 << 6);
  csr9_outl (ioaddr, 0);				/* shut off enable to reset srom state */
  csr9_outl (ioaddr, 1);				/* turn on the chip enable */
  for (i = as + 3; -- i >= 0;) {			/* three command bits followed by six or eight address bits */
    d = (readcmd >> i) & 1;				/* get a readcmd bit */
    d <<= 2;						/* shift it over to bit <2> */
    csr9_outl (ioaddr, d | 1);				/* output the bit */
    csr9_outl (ioaddr, d | 3);				/* turn on the clocking strobe */
    csr9_outl (ioaddr, d | 1);				/* turn off the clocking strobe */
  }
  oz_hw_stl_nanowait (250);
  for (i = 16; -- i >= 0;) {				/* get 16 bits of data */
    csr9_outl (ioaddr, 3);				/* turn on the clocking strobe */
    d += d + ((ether_inl (ioaddr, CSR9) >> 3) & 1);	/* retrieve the data bit */
    oz_hw_stl_nanowait (250);
    csr9_outl (ioaddr, 1);				/* turn off the clocking strobe */
  }
  csr9_outl (ioaddr, 0);				/* turn off chip enable */
  return (d);						/* return the dataword */
}

/* Output SROM control bits to CSR9 then wait 250nS */

static void csr9_outl (uLong ioaddr, uLong csr9bits)

{
  ether_outl (csr9bits | 0x4800, ioaddr, CSR9);
  oz_hw_stl_nanowait (250);
}

/* Calculate srom crc */

#define POLY 0x04C11DB6

static uWord calc_srom_crc (uByte *sromdata)

{
  uByte currentbyte;
  uLong bit;
  uLong crc = 0xFFFFFFFF;
  uLong flippedcrc = 0;
  uLong index;
  uLong msb;

  for (index = 0; index < 126; index ++) {
    currentbyte = sromdata[index];
    for (bit = 0; bit < 8; bit ++) {
      msb = (crc >> 31) & 1;
      crc <<= 1;
      if (msb ^ (currentbyte & 1)) {
        crc ^= POLY;
        crc |= 0x00000001;
      }
      currentbyte >>= 1;
    }
  }

  for (index = 0; index < 32; index ++) {
    flippedcrc <<= 1;
    bit = crc & 1;
    crc >>= 1;
    flippedcrc += bit;
  }

  crc = flippedcrc ^ 0xFFFFFFFF;
  return (crc & 0xFFFF);
}

/************************************************************************/
/*									*/
/*  Shutdown device							*/
/*									*/
/************************************************************************/

static int dectulip_shutdown (OZ_Devunit *devunit, void *devexv)

{
  Devex *devex;

  devex = devexv;

  ether_outl (CSR0_M_SWR, devex -> ioaddr, CSR0);		/* turn on software reset bit */
  oz_hw_stl_microwait (2, NULL, NULL);				/* wait 50 PCI cycles (assume worst case 25MHz bus = 2uS) */
  ether_outl (0, devex -> ioaddr, CSR0);			/* turn off software reset bit */
  oz_hw_stl_microwait (2, NULL, NULL);				/* wait 50 PCI cycles (assume worst case 25MHz bus = 2uS) */

  return (1);
}

/************************************************************************/
/*									*/
/*  A new channel was assigned to the device				*/
/*  This routine initializes the chnex area				*/
/*									*/
/************************************************************************/

static uLong dectulip_assign (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv, OZ_Procmode procmode)

{
  Devex *devex;

  devex = devexv;
  if (devex -> smplock == NULL) {
    oz_knl_printk ("oz_dev_dectulip assign: irq conflict\n");
    return (OZ_DEVOFFLINE);
  }
  memset (chnexv, 0, sizeof (Chnex));
  return (OZ_SUCCESS);
}

/************************************************************************/
/*									*/
/*  A channel is about to be deassigned from a device			*/
/*  Here we do a close if it is open					*/
/*									*/
/************************************************************************/

static int dectulip_deassign (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv)

{
  Chnex *chnex;
  Devex *devex;
  uLong dv;

  chnex = chnexv;
  devex = devexv;

  if (chnex -> rxreqt != NULL) {
    dv = oz_hw_smplock_wait (devex -> smplock);
    close_channel (devex, chnex, dv);
    oz_hw_smplock_clr (devex -> smplock, dv);
    if (chnex -> rxreqt != NULL) oz_crash ("oz_dev_dectulip deassign: channel still open after close");
  }

  return (0);
}

/************************************************************************/
/*									*/
/*  Start performing an ethernet I/O function				*/
/*									*/
/************************************************************************/

#if 0
static uLong dectulip_start (OZ_Devunit *devunit, void *devexv, OZ_Iochan *iochan, void *chnexv, OZ_Procmode procmode, 
                            OZ_Ioop *ioop, void *iopexv, uLong funcode, uLong as, void *ap)
#endif

     static unsigned long dectulip_start (struct _irp * i, struct _ucb * u)

{
  Chnex *chnex, **lchnex;
  Devex *devex;
  Iopex *iopex, **liopex;
  int i;
  uLong dv, sts;
  Rxbuf *rxbuf;
  Txbuf *txbuf;

  chnex = chnexv;
  devex = devexv;
  iopex = iopexv;

  iopex -> ioop     = ioop;
  iopex -> next     = NULL;
  iopex -> procmode = procmode;
  iopex -> devex    = devex;

  switch (funcode) {

    /* Open - associates a protocol number with a channel and starts reception */

    case OZ_IO_ETHER_OPEN: {
      OZ_IO_ether_open ether_open;

      movc4 (as, ap, sizeof ether_open, &ether_open);

      /* Make sure not already open */

      dv = oz_hw_smplock_wait (devex -> smplock);
      if (chnex -> rxreqt != NULL) {
        oz_hw_smplock_clr (devex -> smplock, dv);
        return (OZ_FILEALREADYOPEN);
      }

      /* Put channel on list of open channels - the interrupt routine will now see it */

      chnex -> proto  = N2HW (ether_open.proto);
      chnex -> promis = ether_open.promis;
      chnex -> rxreqt = &(chnex -> rxreqh);
      chnex -> next   = devex -> chnexs;
      devex -> chnexs = chnex;
      oz_hw_smplock_clr (devex -> smplock, dv);
      return (OZ_SUCCESS);
    }

    /* Disassociates a protocol with a channel and stops reception */

    case OZ_IO_ETHER_CLOSE: {
      dv = oz_hw_smplock_wait (devex -> smplock);
      i  = close_channel (devex, chnex, dv);
      oz_hw_smplock_clr (devex -> smplock, dv);

      return (i ? OZ_SUCCESS : OZ_FILENOTOPEN);
    }

    /* Receive a message - note that in this driver (and all our ethernet */
    /* drivers), if there is no receive queued, the message is lost       */

    case OZ_IO_ETHER_RECEIVE: {

      /* Get parameter block into iopex for future reference */

      movc4 (as, ap, sizeof iopex -> receive, &(iopex -> receive));

      /* If any of the rcv... parameters are filled in, it must be called from kernel mode */

      if ((iopex -> receive.rcvfre != NULL) || (iopex -> receive.rcvdrv_r != NULL) || (iopex -> receive.rcveth_r != NULL)) {
        if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);
      }

      /* Set up the request parameters and queue request so the interrupt routine can fill the buffer with an incoming message */

      iopex -> ioop     = ioop;
      iopex -> next     = NULL;
      iopex -> procmode = procmode;
      iopex -> devex    = devex;

      rxbuf = iopex -> receive.rcvfre;						/* maybe requestor has a buffer to free off */
      if ((rxbuf != NULL) && (oz_hw_atomic_inc_long (&(rxbuf -> refcount), -1) > 0)) rxbuf = NULL; /* maybe others are still using it, though */
      if ((rxbuf == NULL) && (devex -> receive_requests_queued >= devex -> receive_buffers_queued)) { /* see if we have enough receive buffers to cover all receive requests */
        rxbuf = allocrcvbuf ();							/* if not, allocate a new one */
      }

      dv = oz_hw_smplock_wait (devex -> smplock);				/* lock database */
      liopex = chnex -> rxreqt;
      if (liopex == NULL) {							/* make sure channel is still open */
        oz_hw_smplock_clr (devex -> smplock, dv);
        if (rxbuf != NULL) freercvbuf (rxbuf);
        return (OZ_FILENOTOPEN);
      }
      *liopex = iopex;								/* put reqeuest on end of queue - interrupt routine can now see it */
      chnex -> rxreqt = &(iopex -> next);
      devex -> receive_requests_queued ++;					/* one more receive request queued to some channel of the device */
      if (rxbuf != NULL) queuercvbuf (devex, rxbuf);				/* maybe queue a new receive buffer */
      oz_hw_smplock_clr (devex -> smplock, dv);					/* unlock database */
      return (OZ_STARTED);
    }

    /* Free a receive buffer */

    case OZ_IO_ETHER_RECEIVEFREE: {
      OZ_IO_ether_receivefree ether_receivefree;

      if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);			/* can only be called from kernel mode */
      movc4 (as, ap, sizeof ether_receivefree, &ether_receivefree);		/* get the parameters */
      rxbuf = ether_receivefree.rcvfre;						/* free off the given buffer */
      if (oz_hw_atomic_inc_long (&(rxbuf -> refcount), -1) == 0) freercvbuf (rxbuf); /* ... if no one else is using it */
      return (OZ_SUCCESS);
    }

    /* Allocate a send buffer */

    case OZ_IO_ETHER_TRANSMITALLOC: {
      OZ_IO_ether_transmitalloc ether_transmitalloc;

      if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);						/* can only be called from kernel mode */
      movc4 (as, ap, sizeof ether_transmitalloc, &ether_transmitalloc);					/* get the parameters */
      txbuf = allocxmtbuf ();										/* allocate a transmit buffer */
      if (ether_transmitalloc.xmtsiz_r != NULL) *(ether_transmitalloc.xmtsiz_r) = DATASIZE;		/* this is size of data it can handle */
      if (ether_transmitalloc.xmtdrv_r != NULL) *(ether_transmitalloc.xmtdrv_r) = txbuf;		/* this is the pointer we want returned in ether_transmit.xmtdrv */
      if (ether_transmitalloc.xmteth_r != NULL) *(ether_transmitalloc.xmteth_r) = (uByte *)&(txbuf -> buf); /* this is where they put the ethernet packet to be transmitted */
      return (OZ_SUCCESS);
    }

    /* Transmit a message */

    case OZ_IO_ETHER_TRANSMIT: {

      /* Get parameter block into iopex for future reference */

      movc4 (as, ap, sizeof iopex -> transmit, &(iopex -> transmit));

      /* Any of the xmt... params requires caller be in kernel mode */

      if ((iopex -> transmit.xmtdrv != NULL) || (iopex -> transmit.xmtsiz_r != NULL) || (iopex -> transmit.xmtdrv_r != NULL) || (iopex -> transmit.xmteth_r != NULL)) {
        if (procmode != OZ_PROCMODE_KNL) return (OZ_KERNELONLY);
        if (iopex -> transmit.xmtsiz_r != NULL) *(iopex -> transmit.xmtsiz_r) = 0;
        if (iopex -> transmit.xmtdrv_r != NULL) *(iopex -> transmit.xmtdrv_r) = NULL;
        if (iopex -> transmit.xmteth_r != NULL) *(iopex -> transmit.xmteth_r) = NULL;
      }

      /* See if they gave us a system buffer */

      if (iopex -> transmit.xmtdrv != NULL) txbuf = iopex -> transmit.xmtdrv;

      /* If not, allocate one (but they have to give us a user buffer then so we can copy in data to be transmitted) */

      else if (iopex -> transmit.buff == NULL) return (OZ_MISSINGPARAM);
      else txbuf = allocxmtbuf ();

      /* No iopex associated with it yet */

      txbuf -> iopex = NULL;

      /* Anyway, copy in any data that they supplied */

      txbuf -> dlen = iopex -> transmit.dlen;
      if (iopex -> transmit.buff != NULL) {
        if (iopex -> transmit.size < txbuf -> buf.data - (uByte *)&(txbuf -> buf)) sts = OZ_BADBUFFERSIZE;						/* have to give at least the ethernet header stuff */
        else if (iopex -> transmit.size > sizeof txbuf -> buf) sts = OZ_BADBUFFERSIZE;									/* can't give us more than buffer can hold */
        else sts = oz_knl_section_uget (procmode, iopex -> transmit.size, iopex -> transmit.buff, &(txbuf -> buf));					/* copy it in */
        if ((sts == OZ_SUCCESS) && (iopex -> transmit.size < txbuf -> dlen + txbuf -> buf.data - (uByte *)&(txbuf -> buf))) sts = OZ_BADBUFFERSIZE;	/* must give enough to cover the dlen */
        if (sts != OZ_SUCCESS) {
          freexmtbuf (txbuf);
          return (sts);
        }
      }

      /* Make sure dlen (length of data not including header) not too int */

      if (txbuf -> dlen > DATASIZE) {			/* can't be longer than hardware will allow */
        freexmtbuf (txbuf);				/* free off internal buffer */
        return (OZ_BADBUFFERSIZE);			/* return error status */
      }

      /* Queue it for processing */

      iopex -> status  = OZ_SUCCESS;			/* be optimistic */
      iopex -> retries = 2;				/* transmit up to a total of 3 times */
      dv = oz_hw_smplock_wait (devex -> smplock);	/* lock out interrupts */
      queuexmtbuf (devex, iopex, txbuf);		/* queue to device */
      oz_hw_smplock_clr (devex -> smplock, dv);		/* unlock interrupts */

      return (OZ_STARTED);
    }

    /* Get info - part 1 */

    case OZ_IO_ETHER_GETINFO1: {
      OZ_IO_ether_getinfo1 ether_getinfo1;

      movc4 (as, ap, sizeof ether_getinfo1, &ether_getinfo1);
      if (ether_getinfo1.enaddrbuff != NULL) {
        if (ether_getinfo1.enaddrsize > ADDRSIZE) ether_getinfo1.enaddrsize = ADDRSIZE;
        sts = oz_knl_section_uput (procmode, ether_getinfo1.enaddrsize, devex -> enaddr, ether_getinfo1.enaddrbuff);
        if (sts != OZ_SUCCESS) return (sts);
      }
      ether_getinfo1.datasize   = DATASIZE;					// max length of data portion of message
      ether_getinfo1.buffsize   = ADDRSIZE * 2 + PROTOSIZE + DATASIZE + 4;	// max length of whole message (header, data, crc)
      ether_getinfo1.dstaddrof  = 0;						// offset of dest address in packet
      ether_getinfo1.srcaddrof  = 0 + ADDRSIZE;					// offset of source address in packet
      ether_getinfo1.protooffs  = 0 + 2 * ADDRSIZE;				// offset of protocol in packet
      ether_getinfo1.dataoffset = 0 + 2 * ADDRSIZE + PROTOSIZE;			// offset of data in packet
      ether_getinfo1.arphwtype  = ARPHWTYPE;					// ARP hardware type
      ether_getinfo1.addrsize   = ADDRSIZE;					// size of each address field
      ether_getinfo1.protosize  = PROTOSIZE;					// size of protocol field
      if (as > sizeof ether_getinfo1) as = sizeof ether_getinfo1;
      sts = oz_knl_section_uput (procmode, as, &ether_getinfo1, ap);
      return (sts);
    }
  }

  return (OZ_BADIOFUNC);
}

/************************************************************************/
/*									*/
/*  Enable the device							*/
/*									*/
/************************************************************************/

static uLong ether_enable (Devex *devex)

{
  uLong dv;
  Rxbuf *rxbuf;
  Txbuf *txbuf;

  if (devex -> smplock == NULL) {
    oz_knl_printk ("oz_dev_dectulip ether_enable: irq conflict\n");
    return (OZ_DEVOFFLINE);
  }

  /* Fill in devex stuff */

  devex -> chnexs = NULL;					/* no I/O channels assigned to device */
  devex -> receive_requests_queued = 0;				/* no receive requeusts queued to any of the channels */

  /* Set up one receive buffer owned by the host - the receiver should suspend when it sees it */

  rxbuf = allocrcvbuf ();					/* allocate the buffer */

  rxbuf -> next  = NULL;					/* nothing follows this buffer */
  rxbuf -> rdes0 = 0;						/* this buf is owned by the host */
  rxbuf -> rdes3 = 0;						/* nothing follows this buffer */

  devex -> receive_buffers_queued = 0;				/* don't include dummy buffer at the end */
  devex -> rcvbusyqf = rxbuf;					/* it is both first and last in the queue */
  devex -> rcvbusyql = rxbuf;

  /* Same for transmitter */

  txbuf = allocxmtbuf ();					/* allocate the buffer */

  txbuf -> next  = NULL;					/* nothing follows it */
  txbuf -> tdes0 = 0;						/* it is owned by the host */
  txbuf -> tdes1 = TDES1_M_TCH;					/* chain mode, no data to transmit yet */
  txbuf -> tdes2 = 0;						/* no buffer to transmit yet */
  txbuf -> tdes3 = 0;						/* nothing follows it */

  devex -> xmtbusyqf = txbuf;					/* it is both first and last in the queue */
  devex -> xmtbusyql = txbuf;

  /* Software reset the chip */

  ether_outl (CSR0_M_SWR, devex -> ioaddr, CSR0);		/* turn on software reset bit */
  oz_hw_stl_microwait (2, NULL, NULL);				/* wait 50 PCI cycles (assume worst case 25MHz bus = 2uS) */
  ether_outl (0, devex -> ioaddr, CSR0);			/* turn off software reset bit */
  oz_hw_stl_microwait (2, NULL, NULL);				/* wait 50 PCI cycles (assume worst case 25MHz bus = 2uS) */

  /* Turn on hardware */

  dv = oz_hw_smplock_wait (devex -> smplock);
  turnondev (devex);
  oz_hw_smplock_clr (devex -> smplock, dv);

  return (OZ_SUCCESS);
}

/************************************************************************/
/*									*/
/*  Disable the ethernet device						*/
/*									*/
/************************************************************************/

static void ether_disable (Devex *devex)

{
  Chnex *chnex;
  Rxbuf *rxbuf;
  Txbuf *txbuf;
  uLong dv;

  ether_outl (CSR0_M_SWR, devex -> ioaddr, CSR0);	/* software reset the chip to halt all operations */
  oz_hw_stl_microwait (2, NULL, NULL);			/* wait for 2uS for reset to complete */

  dv = oz_hw_smplock_wait (devex -> smplock);		/* lock database */

  while ((chnex = devex -> chnexs) != NULL) {		/* close any open channels - also aborts pending receive requests */
    close_channel (devex, chnex, dv);
  }

  devex -> rcvbusyql = NULL;				/* free off all receive buffers */
  while ((rxbuf = devex -> rcvbusyqf) != NULL) {
    devex -> rcvbusyqf = rxbuf -> next;
    freercvbuf (rxbuf);
  }

  devex -> xmtbusyql = NULL;				/* free off all transmit buffers */
  while ((txbuf = devex -> xmtbusyqf) != NULL) {
    devex -> xmtbusyqf = txbuf -> next;
    txbuf -> iopex -> status = OZ_ABORTED;
    freexmtbuf (txbuf);
  }

  oz_hw_smplock_clr (devex -> smplock, dv);		/* release database */
}

/************************************************************************/
/*									*/
/*  Close an open channel						*/
/*									*/
/*    Input:								*/
/*									*/
/*	chnexx = channel to be closed					*/
/*	dv = previous smplock level (should be SOFTINT)			*/
/*	smplock = dv							*/
/*									*/
/*    Output:								*/
/*									*/
/*	close_channel = 0 : channel wasn't open				*/
/*	                1 : channel was open, now closed		*/
/*									*/
/*    Note:								*/
/*									*/
/*	smplock is released and re-acquired				*/
/*									*/
/************************************************************************/

static int close_channel (Devex *devex, Chnex *chnexx, uLong dv)

{
  Chnex *chnex, **lchnex;
  Iopex *iopex;

  /* Unlink from list of open channels */

  for (lchnex = &(devex -> chnexs); (chnex = *lchnex) != chnexx; lchnex = &(chnex -> next)) {
    if (chnex == NULL) return (0);
  }
  *lchnex = chnex -> next;

  /* Abort all pending receive requests and don't let any more queue */

  chnex -> rxreqt = NULL;				/* mark it closed - abort any receive requests that try to queue */
  while ((iopex = chnex -> rxreqh) != NULL) {		/* abort any receive requests we may have */
    chnex -> rxreqh = iopex -> next;
    devex -> receive_requests_queued --;
    oz_hw_smplock_clr (devex -> smplock, dv);
    oz_knl_iodone (iopex -> ioop, OZ_ABORTED, NULL, NULL, NULL);
    dv = oz_hw_smplock_wait (devex -> smplock);
  }

  return (1);
}

/************************************************************************/
/*									*/
/*  Interrupt routine							*/
/*									*/
/************************************************************************/

static int ether_interrupt (void *devexv, OZ_Mchargs *mchargs)

{
  Devex *devex;
  int again;
  uLong csr5;

  devex = devexv;

  do {
    csr5  = ether_inl (devex -> ioaddr, CSR5);				/* get list of interrupt status bits */
    again = 0;								/* assume we won't have to repeat */
    ether_outl (csr5, devex -> ioaddr, CSR5);				/* acknowledge all of the current interrupt sources */
    if (csr5 & (CSR5_SE | CSR5_LNF | CSR5_RWT | CSR5_RPS | CSR5_UNF | CSR5_TJT | CSR5_TPS)) {
      oz_knl_printk ("oz_dev_dectulip: %s csr5 0x%x\n", devex -> name, csr5); /* ?? some bits indicate fatal errors and chip needs to be reset ?? */
      again = 1;
    }
    again |= receive_done (devex);					/* always check for completed receives */
    again |= transmit_done (devex);					/* always check for completed transmits */
  } while (again);

  return (0);
}

/************************************************************************/
/*									*/
/*  This routine is called by the interrupt service routine when there 	*/
/*  may be receive buffers filled in					*/
/*									*/
/************************************************************************/

static int receive_done (Devex *devex)

{
  Chnex *chnex;
  int didsomething;
  Iopex *iopex;
  uLong framelength;
  Rxbuf *rxbuf;
  uWord proto;

  /* The finished buffers will be marked by having the RDES0_M_OWN bit clear.  Note however, that there is such a buffer on the end of the list that is just */
  /* there to mark the end of the list.  It is different in that its rxbuf -> next is NULL, any genuine filled buffer will have a non-NULL rxbuf -> next.    */

  didsomething = 0;
  while (1) {
    rxbuf = devex -> rcvbusyqf;							/* point to the first in the list */
    if (rxbuf -> rdes0 & RDES0_M_OWN) break;					/* if still owned by controller, it hasn't filled it yet */
    if (rxbuf -> next == NULL) break;						/* it may be the last on the list that is just there to stop the controller */
    devex -> rcvbusyqf = rxbuf -> next;						/* ok, it has legitimate data filled in, dequeue it */
    devex -> receive_buffers_queued --;						/* one less buffer queued to device */
    rxbuf -> refcount = 0;							/* initialize refcount */
    framelength = (rxbuf -> rdes0 & RDES0_M_FL) >> RDES0_V_FL;			/* number of bytes received, including dstaddr, srcaddr, proto, data and crc */
    if ((rxbuf -> rdes0 & (RDES0_M_ES | RDES0_M_LE | RDES0_M_RF | RDES0_M_FS | RDES0_M_LS | RDES0_M_TL | RDES0_M_CS | RDES0_M_RJ | RDES0_M_DB | RDES0_M_CE | RDES0_M_OF)) != (RDES0_M_FS | RDES0_M_LS)) {
      oz_knl_printk ("oz_dev_dectulip: %s rdes0 0x%x\n", devex -> name, rxbuf -> rdes0); /* receive error, print message and throw buffer out */
    } else if ((framelength < 18) || (framelength > DATASIZE + 18)) {
      oz_knl_printk ("oz_dev_dectulip: %s bad frame length %u\n", devex -> name, framelength); /* bad frame length */
    } else {
      rxbuf -> dlen = framelength - 18;						/* fill in data length (excludes dstaddr, srcaddr, proto, data and crc) */
      proto = N2HW (rxbuf -> buf.proto);					/* get received message's protocol */
      for (chnex = devex -> chnexs; chnex != NULL; chnex = chnex -> next) {	/* find a suitable I/O channel */
        if ((chnex -> proto != 0) && (chnex -> proto != proto)) continue;	/* ... with matching protocol */
        if (!(chnex -> promis) && !CEQENADDR (rxbuf -> buf.dstaddr, devex -> enaddr) && !CEQENADDR (rxbuf -> buf.dstaddr, broadcast)) continue; /* matching enaddr */
        iopex = chnex -> rxreqh;						/* see if any receive I/O requests pending on it */
        if (iopex == NULL) {
          chnex -> rcvmissed ++;
          continue;
        }
        chnex -> rxreqh = iopex -> next;
        if (chnex -> rxreqh == NULL) chnex -> rxreqt = &(chnex -> rxreqh);
        OZ_HW_ATOMIC_INCBY1_LONG (rxbuf -> refcount);				/* increment received buffer's reference count */
        devex -> receive_requests_queued --;					/* one less receive request queued to device */
        iopex -> rxbuf = rxbuf;							/* assign the received buffer to the request */
        oz_knl_iodonehi (iopex -> ioop, OZ_SUCCESS, NULL, receive_iodone, iopex); /* post it for completion */
      }
    }
    if (rxbuf -> refcount == 0) queuercvbuf (devex, rxbuf);			/* toss if no one wants it */
    didsomething = 1;								/* anyway, we did something */
  }

  return (didsomething);
}

/************************************************************************/
/*									*/
/*  Back in requestor's thread - copy out data and/or pointers		*/
/*									*/
/*    Input:								*/
/*									*/
/*	iopexv = receive request's iopex				*/
/*	finok  = 0 : requesting thread has exited			*/
/*	         1 : made it back in requestor's thread			*/
/*									*/
/************************************************************************/

static void receive_iodone (void *iopexv, int finok, uLong *status_r)

{
  Devex *devex;
  Iopex *iopex;
  uLong size, sts;
  Rxbuf *rxbuf;

  iopex = iopexv;
  devex = iopex -> devex;
  rxbuf = iopex -> rxbuf;

  /* If requested, copy data to user's buffer */

  if ((iopex -> receive.buff != NULL) && (finok || OZ_HW_ISSYSADDR (iopex -> receive.buff))) {
    size = rxbuf -> dlen + 20;										/* size of everything we got, including length and crc */
    if (size > iopex -> receive.size) size = iopex -> receive.size;					/* chop to user's buffer size */
    sts = oz_knl_section_uput (iopex -> procmode, size, &(rxbuf -> buf), iopex -> receive.buff);	/* copy data out to user's buffer */
    if (sts != OZ_SUCCESS) {
      oz_knl_printk ("oz_dev_dectulip: copy to receive buf sts %u\n", sts);
      if (*status_r == OZ_SUCCESS) *status_r = sts;
    }
  }

  /* If requested, return length of data received */

  if ((iopex -> receive.dlen != NULL) && (finok || OZ_HW_ISSYSADDR (iopex -> receive.dlen))) {
    sts = oz_knl_section_uput (iopex -> procmode, sizeof *(iopex -> receive.dlen), &(rxbuf -> dlen), iopex -> receive.dlen);
    if (sts != OZ_SUCCESS) {
      oz_knl_printk ("oz_dev_dectulip: return rlen sts %u\n", sts);
      if (*status_r == OZ_SUCCESS) *status_r = sts;
    }
  }

  /* If requested, return pointer to system buffer                                       */
  /* Note that this can only be done from kernel mode so we don't bother validating args */

  if ((iopex -> receive.rcvdrv_r != NULL) && (finok || OZ_HW_ISSYSADDR (iopex -> receive.rcvdrv_r))) {
    *(iopex -> receive.rcvdrv_r) = rxbuf;
    *(iopex -> receive.rcveth_r) = (uByte *)&(rxbuf -> buf);
  }

  /* If we didn't return the pointer, free off receive buffer */

  else if (oz_hw_atomic_inc_long (&(rxbuf -> refcount), -1) == 0) queuercvbuf (devex, rxbuf);
}

/************************************************************************/
/*									*/
/*  This routine is called at interrupt level when a transmission has 	*/
/*  completed								*/
/*									*/
/************************************************************************/

static int transmit_done (Devex *devex)

{
  int didsomething;
  Iopex *iopex;
  Txbuf *txbuf;

  /* The finished buffers will be marked by having the TDES0_OWN bit clear.                         */
  /* Note however, the data for that request is in the next txbuf in the list.                      */
  /* The last txbuf already has its TDES0_OWN bit clear, but it also has txbuf -> next set to NULL. */

  didsomething = 0;
  while (1) {
    txbuf = devex -> xmtbusyqf;							/* point to the first in the list */
    if (txbuf -> next == NULL) break;						/* it may be the last on the list that is just there to stop the controller */
    if (txbuf -> tdes0 & TDES0_OWN) break;					/* if still owned by controller, it hasn't finished it yet */
    devex -> xmtbusyqf = txbuf -> next;						/* ok, it has finished, dequeue it */
    didsomething = 1;
    if ((txbuf -> tdes0 & (TDES0_ES | TDES0_TO | TDES0_LO | TDES0_NC | TDES0_LC | TDES0_EC | TDES0_LF | TDES0_UF)) != 0) {
      oz_knl_printk ("oz_dev_dectulip: %s tdes0 status 0x%x\n", devex -> name, txbuf -> tdes0); /* transmit error, print message */
      iopex = txbuf -> iopex;
      if (-- (iopex -> retries) >= 0) {						/* see if any retries left */
        queuexmtbuf (devex, iopex, txbuf);					/* if so, requeue it for transmission */
        continue;
      }
      iopex -> status = OZ_IOFAILED;						/* no retries, return failure status */
    }
    freexmtbuf (txbuf);								/* free it off and post associated I/O request */
  }

  return (didsomething);
}

/************************************************************************/
/*									*/
/*  This routine turns on the device after a software reset		*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = device extension pointer				*/
/*	smplock = device lock set					*/
/*									*/
/************************************************************************/

static void turnondev (Devex *devex)

{
  ether_outl ((CACHE_ALIGNMENT << CSR0_V_CAL) | (BURST_LENGTH << CSR0_V_PBL), devex -> ioaddr, CSR0);

  /* ?? set up filtering ?? - for now, just use promiscuous mode (CSR6_M_PR, see below) */

  ether_outl (devex -> rcvbusyqf -> rdes0_physaddr, devex -> ioaddr, CSR3);

  /* Set up one transmit buffer owned by the host - the transmitter should suspend when it sees it */

  ether_outl (devex -> xmtbusyqf -> tdes0_physaddr, devex -> ioaddr, CSR4);

  /* Set media type to be 10base-T, half duplex, no autosensing, no autonegotiation */

  if (devex -> didvid == DIDVID_21041) {
    ether_outl (0xEF01, devex -> ioaddr, CSR13);
    ether_outl (0x4F3F, devex -> ioaddr, CSR14); /* 7F3F loses packets on reception */
    ether_outl (0x0008, devex -> ioaddr, CSR15);
    oz_hw_stl_microwait (10000, NULL, NULL);
  }

  /* Set up conditions that cause an interrupt */

  ether_outl (CSR7_NIM | CSR7_SEM | CSR7_TMM | CSR7_RWM | CSR7_RSM | CSR7_RIM | CSR7_LPM | CSR7_TJM | CSR7_TSM | CSR7_TIM, devex -> ioaddr, CSR7);

  /* CSR6 should be the last thing set in here             */
  /* - must be at least 10ms after writing CSR13, 14 or 15 */

  ether_outl ((TRANSMIT_THRESHOLD << CSR6_V_TR) | CSR6_M_PR, devex -> ioaddr, CSR6);
  ether_outl ((TRANSMIT_THRESHOLD << CSR6_V_TR) | CSR6_M_PR | CSR6_M_ST | CSR6_M_SR, devex -> ioaddr, CSR6);
}

/************************************************************************/
/*									*/
/*  Allocate a receive buffer						*/
/*									*/
/*    Input:								*/
/*									*/
/*	smplock <= np							*/
/*									*/
/*    Note:								*/
/*									*/
/*	The buffer must be allocated such that rxbuf -> rdes0..3 is 	*/
/*	longword aligned and physically contiguous, and rxbuf -> 	*/
/*	buf.dstaddr is longword aligned and physically contiguous.	*/
/*									*/
/************************************************************************/

static Rxbuf *allocrcvbuf (void)

{
  uLong buflen;
  Rxbuf *rxbuf;

  /* Maybe there is a free one we can take */

  do rxbuf = free_rxbufs;
  while ((rxbuf != NULL) && !oz_hw_atomic_setif_ptr ((void *volatile *)&free_rxbufs, rxbuf -> next, rxbuf));

  /* If not, allocate a physically contiguous buffer */

  if (rxbuf == NULL) rxbuf = OZ_KNL_PCMALLOC ((uByte *)&(rxbuf -> buf) - (uByte *)rxbuf + BUFFSIZE);

  /* Get physical addresses of the descriptor and data areas */

  rxbuf -> rdes0_physaddr = npp_virt_to_phys (16, &(rxbuf -> rdes0));
  rxbuf -> rdes2 = npp_virt_to_phys (BUFFSIZE, &(rxbuf -> buf));

  /* Say we use chain mode & set up size of data portion of buffer */

  rxbuf -> rdes1 = (BUFFSIZE << RDES1_V_RBS1) | RDES1_M_RCH;

  return (rxbuf);
}

/************************************************************************/
/*									*/
/*  This routine puts a buffer on the end of the busy chain for a 	*/
/*  device so it can receive a packet					*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = ethernet device to queue it to				*/
/*	rxbuf = buffer to be queued					*/
/*									*/
/*	smplock = device level						*/
/*									*/
/*    Output:								*/
/*									*/
/*	buffer placed on end of device's receive queue			*/
/*									*/
/*    Note:								*/
/*									*/
/*	We keep a buffer on the end of the list that has the OWN bit 	*/
/*	cleared at all times.  This is how we tell the controller it 	*/
/*	has reached the end of the list.  Then, when we put this 	*/
/*	buffer on the end, we activate the old buffer by setting its 	*/
/*	OWN bit and putting this one after it with the new one's OWN 	*/
/*	bit cleared.							*/
/*									*/
/************************************************************************/

static void queuercvbuf (Devex *devex, Rxbuf *rxbuf)

{
  Rxbuf *rxbufl;

  /* If we already have enough buffers queued, just free this one off */

  if (devex -> receive_buffers_queued >= devex -> receive_requests_queued) {
    freercvbuf (rxbuf);
    return;
  }

  /* Need more, queue it up */

  rxbufl = devex -> rcvbusyql;			/* point to whatever is currently last in list */
						/* - it should be 'host-owned', with null/zero links */

  rxbuf -> next  = NULL;			/* say nothing follows this one */
  rxbuf -> rdes0 = 0;				/* it is owned by the CPU - this stops the controller from filling it */
  rxbuf -> rdes3 = 0;				/* we have nothing following this one */

  devex -> rcvbusyql = rxbuf;			/* this one is now our last one */

  rxbufl -> next  = rxbuf;			/* link this one on the end of the list */
  rxbufl -> rdes3 = rxbuf -> rdes0_physaddr;	/* (also chain the descriptors) */
  rxbufl -> rdes0 = RDES0_M_OWN;		/* say the controller now owns the old last buffer (ie, it is ok to fill it in) */
  ether_outl (1, devex -> ioaddr, CSR2);	/* tell the controller we changed the last in the list in case it was stopped on it */
						/* ?? that may be unnecessary as the receiver should automatically resume when next frame comes in ?? */

  devex -> receive_buffers_queued ++;		/* one more buffer queued to device */
}

/************************************************************************/
/*									*/
/*  Free receive buffer							*/
/*									*/
/*    Input:								*/
/*									*/
/*	rxbuf = receive buffer to be freed				*/
/*									*/
/************************************************************************/

static void freercvbuf (Rxbuf *rxbuf)

{
  do rxbuf -> next = free_rxbufs;
  while (!oz_hw_atomic_setif_ptr ((void *volatile *)&free_rxbufs, rxbuf, rxbuf -> next));
}

/************************************************************************/
/*									*/
/*  Allocate a transmit buffer						*/
/*									*/
/*    Note:								*/
/*									*/
/*	The buffer must be allocated such that txbuf -> rdes0..3 is 	*/
/*	longword aligned and physically contiguous, and txbuf -> 	*/
/*	buf.dstaddr is longword aligned and physically contiguous.	*/
/*									*/
/************************************************************************/

static Txbuf *allocxmtbuf (void)

{
  Txbuf *txbuf;

  /* Maybe there is a free one we can take */

  do txbuf = free_txbufs;
  while ((txbuf != NULL) && !oz_hw_atomic_setif_ptr ((void *volatile *)&free_txbufs, txbuf -> next, txbuf));

  /* If not, allocate a physically contiguous buffer and get */
  /* physical addresses of the descriptor and data areas     */

  if (txbuf == NULL) {
    txbuf = OZ_KNL_PCMALLOC ((uByte *)&(txbuf -> buf) - (uByte *)txbuf + BUFFSIZE);
    txbuf -> tdes0_physaddr = npp_virt_to_phys (16, &(txbuf -> tdes0));
    txbuf -> buf_physaddr   = npp_virt_to_phys (BUFFSIZE, &(txbuf -> buf));
  }

  /* Say we use chain mode */

  txbuf -> tdes1 = TDES1_M_TCH;

  /* It has no associated iopex yet */

  txbuf -> iopex = NULL;

  return (txbuf);
}

/************************************************************************/
/*									*/
/*  This routine puts a buffer on the end of the busy chain for a 	*/
/*  device so it will be transmitted					*/
/*									*/
/*    Input:								*/
/*									*/
/*	devex = ethernet device to queue it to				*/
/*	txbuf = buffer to be queued					*/
/*									*/
/*	smplock = device level						*/
/*									*/
/*    Output:								*/
/*									*/
/*	buffer placed on end of device's transmit queue			*/
/*									*/
/*    Note:								*/
/*									*/
/*	We keep a buffer on the end of the list that has the OWN bit 	*/
/*	cleared at all times.  This is how we tell the controller it 	*/
/*	has reached the end of the list.  Then, when we put this 	*/
/*	buffer on the end, we activate the old buffer by setting its 	*/
/*	OWN bit and putting this one after it with the new one's OWN 	*/
/*	bit cleared.							*/
/*									*/
/************************************************************************/

static void queuexmtbuf (Devex *devex, Iopex *iopex, Txbuf *txbuf)

{
  Txbuf *txbufl;

  txbufl = devex -> xmtbusyql;			/* point to whatever is currently last in list */
						/* - it should be 'host-owned', with null/zero links */

  txbuf -> next  = NULL;			/* say nothing follows this one */
  txbuf -> tdes0 = 0;				/* it is owned by the CPU - this stops the controller from transmitting it */
  txbuf -> tdes1 = TDES1_M_TCH;			/* we have no data for this descriptor (just keep the chaining bit on) */
  txbuf -> tdes2 = 0;				/* we have no data for this descriptor */
  txbuf -> tdes3 = 0;				/* we have nothing following this one */

  devex -> xmtbusyql = txbuf;			/* this one is now our last one */

  txbufl -> next  = txbuf;			/* link this one on the end of the list */
  txbufl -> iopex = iopex;			/* post this iopex when txbufl gets freed */
  txbufl -> tdes3 = txbuf -> tdes0_physaddr;	/* (also chain the descriptors) */
  txbufl -> tdes2 = txbuf -> buf_physaddr;	/* set last descriptor's buffer address = my data's physical address */
  txbufl -> tdes1 = TDES1_M_IC | TDES1_M_LS | TDES1_M_FS | TDES1_M_TCH | ((txbuf -> dlen + 14) << TDES1_V_TBS1); /* set last descriptor's data length = my data's length (incl dstaddr, srcaddr, proto) */
  OZ_HW_MB;					/* make sure controller sees the above before we mark descriptor owner */
  txbufl -> tdes0 = TDES0_OWN;			/* say the controller now owns the old last descriptor (it points to our data so it is ok to transmit it) */
  ether_outl (1, devex -> ioaddr, CSR1);	/* tell the controller we changed the last in the list in case it was stopped on it */
}

/************************************************************************/
/*									*/
/*  Free transmit buffer						*/
/*									*/
/*    Input:								*/
/*									*/
/*	txbuf = transmit buffer to be freed				*/
/*	txbuf -> iopex = associated i/o request with status filled in	*/
/*	smplock <= hi							*/
/*									*/
/************************************************************************/

static void freexmtbuf (Txbuf *txbuf)

{
  Iopex *iopex;
  uLong hi;

  iopex = txbuf -> iopex;

  /* If there is an I/O to post and it was successful and it is requesting a replacement */
  /* transmit buffer, just return the transmit buffer that is going to be freed off.     */

  if ((iopex != NULL) && (iopex -> status == OZ_SUCCESS) && (iopex -> transmit.xmtdrv_r != NULL)) {
    oz_knl_iodonehi (iopex -> ioop, OZ_SUCCESS, NULL, transmit_iodone, txbuf);
  }

  /* Otherwise, free the buffer then post the request (if any) */

  else {
    do txbuf -> next = free_txbufs;				/* free it */
    while (!oz_hw_atomic_setif_ptr ((void *volatile *)&free_txbufs, txbuf, txbuf -> next));
    if (iopex != NULL) {					/* see if associated iopex */
      oz_knl_iodonehi (iopex -> ioop, iopex -> status, NULL, NULL, NULL); /* if so, post its completion */
    }
  }
}

/* This routine is called in requestor's process space at softint level */

static void transmit_iodone (void *txbufv, int finok, uLong *status_r)

{
  Iopex *iopex;
  Txbuf *txbuf;

  txbuf = txbufv;
  iopex = txbuf -> iopex;

  /* Completing transmit successfully, return parameters for the buffer rather  */
  /* than freeing it. Since requestor was in kernel mode, don't bother probing. */

  if (finok) {
    if (iopex -> transmit.xmtsiz_r != NULL) *(iopex -> transmit.xmtsiz_r) = DATASIZE;
    if (iopex -> transmit.xmtdrv_r != NULL) *(iopex -> transmit.xmtdrv_r) = txbuf;
    if (iopex -> transmit.xmteth_r != NULL) *(iopex -> transmit.xmteth_r) = (uByte *)&(txbuf -> buf);
  }
}

/************************************************************************/
/*									*/
/*  Convert npp virtual address to physical address - also check for 	*/
/*  longword alignment							*/
/*									*/
/*    Input:								*/
/*									*/
/*	size = size of buffer area					*/
/*	addr = address of buffer area					*/
/*									*/
/*    Output:								*/
/*									*/
/*	npp_virt_to_phys = physical address of 'addr'			*/
/*									*/
/************************************************************************/

static uLong npp_virt_to_phys (uLong size, void *addr)

{
  OZ_Mempage phypage;
  uLong length, offset;

  length = oz_knl_misc_sva2pa (addr, &phypage, &offset);
  if (length < size) {
    oz_crash ("oz_dev_dectulip npp_virt_to_phys: buf %p size %u not physically contiguous", addr, size);
  }

  if (offset & 3) oz_crash ("oz_dev_dectulip npp_virt_to_phys: buf %p not longword aligned", addr);

  return ((phypage << OZ_HW_L2PAGESIZE) + offset);
}

/************************************************************************/
/*									*/
/*  Access ethernet card's I/O registers				*/
/*									*/
/*  This chip only allows longword access so that's all we supply	*/
/*									*/
/************************************************************************/

static uLong ether_inl (uLong ioaddr, uWord offset)

{
  if (ioaddr & 1) return (oz_hw_inl (ioaddr - 1 + offset));
  return (*(uLong *)(ioaddr + offset));
}

static void ether_outl (uLong value, uLong ioaddr, uWord offset)

{
  if (ioaddr & 1) oz_hw_outl (value, ioaddr - 1 + offset);
  else *(uLong *)(ioaddr + offset) = value;
}
#endif
