#pragma module ENABLE_IDE "X-1"

/************************************************************************/
/*									*/
/* Copyright © Digital Equipment Corporation, 1994 All Rights Reserved.	*/
/* Unpublished rights reserved under the copyright laws of the United	*/
/* States.								*/
/*									*/
/* The software contained on this media is proprietary to and embodies	*/
/* the confidential technology of Digital Equipment Corporation.	*/
/* Possession, use, duplication or dissemination of the software and	*/
/* media is authorized only pursuant to a valid written license from	*/
/* Digital Equipment Corporation.					*/
/*									*/
/* RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the	*/
/* U.S. Government is subject to restrictions as set forth in		*/
/* Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,	*/
/* as applicable.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/* Abstract:								*/
/*	This program enables the IDE interface on systems with the	*/
/*	8731x SuperI/O chip.						*/
/*									*/
/* Author:								*/
/*	Benjamin J. Thomas III / November 1994				*/
/*									*/
/* Revision History:							*/
/*									*/
/*	X-1		Benjamin J. Thomas III		November, 1994	*/
/*		Initial version.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*	$ CC ENABLE-IDE + SYS$LIBRARY:SYS$LIB_C.TLB/LIBRARY		*/
/*	$ LINK/SYSEXE ENABLE-IDE					*/
/*									*/
/************************************************************************/

#include	adpdef			/* Define the ADP */
#include	dcdef			/* Define device and ADP codes */
#include	descrip			/* Define descriptors */
#include	iocdef			/* Add I/O constants */
#include	iogendef		/* IOGEN symbols (load driver) */
#include	ipldef			/* Define IPLs */
#include	lib$routines		/* Define LIB$ routines */
#include	pcbdef			/* Define the PCB */
#include	ssdef			/* System service statys codes */
#include	starlet			/* Define SYS$ routines */
#include	stdio			/* Standard I/O */
#include	stdlib			/* Define standard definitions */
#include	string			/* Define string handling routines */
#include	stsdef			/* Define status codes */
#include	varargs			/* Handle variable argument lists */

#include	ioc_routines		/* Define the IOC$ routines */
#include	sch_routines		/* Deinf the SCH$ routines */
#include	vms_macros		/* Include the VMS macros */

int sys$load_driver(__unknown_params);

static ADP	*isa_adp;		/* ISA ADP address */
static __int64	isa_base;		/* ISA base address */
static int	isa_tr;			/* ISA TR number */
static unsigned __int64 iohandle;	/* I/O handle */
static int	isa_offset;		/* Offset to 87312 register */

extern ADP	*IOC$GL_ADPLIST;	/* Start of ADP list */
extern PCB	*CTL$GL_PCB;		/* Point to current process PCB */

/* Define what an IOSB looks like */

typedef struct {			/* Standard I/O status block */
    short int   status;			/* Status word */
    short int   byte_cnt;		/* Transferred byte count */
    int     unused;			/* Unused */
    } IOSB_T;

/* Define what a VMS item list looks like */

typedef struct {			/* VMS item list item */
    short int   buf_len;		/* Length of data buffer */
    short int   item_code;		/* Item code number */
    void   *buff_addr;			/* Pointer to data buffer */
    int    *ret_addr;			/* Returned data length */
    } ITEM_T;


#define	$SUCCESS(code)	( (code & STS$M_SUCCESS) == 1)
#define	$FAIL(code)	( (code & STS$M_SUCCESS) == 0)

void chk_sts(status,string,va_alist)

/* This routine is used to check a VMS status code.  If the code does	*/
/* not represent success, then the error string is output and the	*/
/* program is exited with that status code.  If the code does show a	*/
/* success status, then this routine simply returns to the caller.	*/
/*									*/
/*	Input:								*/
/*		string	- error string					*/
/*		status	- status code					*/

int	status;					/* Status to check */
char	string[];				/* String to be output */

va_dcl						/* Declare varying arg list */
{
va_list	ap;					/* Argument list pointer */

/* Check for error */

    if ($VMS_STATUS_SUCCESS(status))
        return;					/* Success - just return */

/* Error - print message and exit */

    va_start(ap);				/* Start the arg list */
    fprintf(stderr,"? ");			/* Output start of message */
    vfprintf(stderr,string,ap);			/* Output the string */
    fprintf(stderr,"\n");			/* Terminate string */
    va_end(ap);					/* End list */
    exit(status);				/* Exit with error */
}

int	check_system_type() {

/* This routine is used to check that this system supports an internal	*/
/* IDE controller that can be enabled.					*/
/*									*/
/* Input:								*/
/*	none								*/
/*									*/
/* Output:								*/
/*	status		SS$_NORMAL	system is supported		*/
/*			SS$_UNSUPPORTED	no support on this system	*/

#include	syidef			/* Define GETSYI items */

IOSB_T	iosb;				/* IOSB for $GETSYI */
static int cpu_type;			/* CPU type code */
static int sys_type;			/* System type code */
int	efn;				/* Event flag number */
int	status;				/* Routine status code */

struct {				/* $GETSYI for system and CPU type */
    ITEM_T	items[2];		/* Define the items */
    int     end;			/* Define an end item */
}    syi_item = {4, SYI$_CPUTYPE, &cpu_type, 0,
                 4, SYI$_SYSTYPE, &sys_type, 0,
                 0 };
    
    status = lib$get_ef(&efn);		/* Acquire an EFN */
    chk_sts(status,"Unable to acquire EFN, status = %X",status);

    status = sys$getsyiw(efn,0,0,&syi_item,&iosb,0,0);	/* Get information */
    chk_sts(status,"GETSYI failed to obtain system type, status = %X",status);
    chk_sts(iosb.status,"GETSYI failed to obtain system type, IOSB status = %X",
            iosb.status);

/* Check types */

    if ( (cpu_type == 2) && (sys_type == 13) ) {	/* Avanti ? */
        isa_offset = 0x26e;		/* The ISA CSR address */
        return SS$_NORMAL;		/* Supported */
    }

    return SS$_UNSUPPORTED;		/* None of the above - exit with ?? */
}

int	set_ide() {

/* This routine is used to enable the IDE interface in the 8731x.	*/
/*									*/
/* Input:								*/
/*	none								*/
/*									*/
/* Output:								*/
/*	status		SS$_NORMAL	system is supported		*/
/*			SS$_UNSUPPORTED	no support on this system	*/

int	status;				/* Status from MAP_IO */
int	data;				/* Data read/written */
int	sts;				/* Routine status */
int	idx_shift;			/* Index register shift */
int	data_shift;			/* Data register shift */
int	i;				/* Loop counter */
int	saved_ipl;			/* Saved IPL value */
unsigned __int64 ofs;			/* ISA Offset value */

/* Precompute some values */

    idx_shift  = ( isa_offset    & 3) << 3;	/* Compute shift count */
    data_shift = ((isa_offset+1) & 3) << 3;	/* Compute shift count */

/* Map the page */

    dsbint(IPL$_SYNCH, saved_ipl);	/* Bump the IPL */
    ofs = isa_offset;			/* Get the ISA offset */
    sts = ioc$map_io(isa_adp, 0, &ofs, 2, IOC$K_BUS_IO_BYTE_GRAN,
                        &iohandle);

/* Set the index register and read the value */
/* We'll do this a few times, just to be sure that we actually wrote it. */
/* The chip requires two back-to-back writes, and while IPL 31 will	*/
/* improve the odds, it isn't bullet-proof.				*/

    for (i = 0; i < 10; i++) {
        data = 0;			/* Index register is 0 */
        sts = ioc$write_io(isa_adp, &iohandle, 0, 1, &data);	/* Write index */
        sts = ioc$read_io(isa_adp, &iohandle, 1, 1, &data);	/* Get data */
        data = data >> data_shift;	/* Shift the data down */
        if ( (data & 0x40) != 0) {	/* Was it already set ? */
            if (i == 0)			/* If first time, then */
                status = SS$_WASSET;	/*  indicate set already  */
            else
                status = SS$_NORMAL;	/* Indicate success in setting it */
            break;			/*  and exit the loop */
        }

/* Set IDE enable bit and write it twice (87312 rules) */

        data = (data|0x40)<<data_shift;	/* Enable the IDE as a primary */
        setipl(IPL$_MEGA);		/* Set IPL to 31 */
        sts = ioc$write_io(isa_adp, &iohandle, 1, 1, &data);
        sts = ioc$write_io(isa_adp, &iohandle, 1, 1, &data);
        setipl(IPL$_SYNCH);		/* Set IPL back to 8 */
    }

/* Unmap and exit */

    ioc$unmap_io(isa_adp, &iohandle);	/* Unmap the registers */
    enbint(saved_ipl);			/* Restore IPL */

    return	status;			/* Exit with status */
}

int	get_isa_base() {

/* This routine is used to obtain the base VA of ISA space.		*/
/*									*/
/* Input:								*/
/*	none								*/
/*									*/
/* Output:								*/
/*	status		SS$_NORMAL	system is supported		*/
/*			SS$_UNSUPPORTED	no support on this system	*/

ADP	*adp;				/* ADP pointer */
int	status;				/* Return status */

/* Set up; lock I/O mutex */

    isa_base	= 0;			/* Initialize ISA base address */
    isa_adp	= 0;			/* Initialize the ISA ADP address */
    isa_tr	= 0;			/* Initialize the ISA TR number */
    status = SS$_ITEMNOTFOUND;		/* Set up return status value */

    sch_std$iolockr(CTL$GL_PCB);	/* Lock I/O database for read */

/* Note: This code could also be enhanced to match TR numbers and other	*/
/*	 possible qualifiers.  For now, keep it simple.			*/

/* Walk the ADP list looking for an ISA ADP */

    for (adp = IOC$GL_ADPLIST; adp != 0; adp = adp->adp$l_link) {
        if (adp->adp$l_adptype == AT$_ISA) {
            isa_base = adp->adp$q_csr;	/* Get the CSR address */
            isa_adp  = adp;		/* Save ADP address */
            isa_tr   = adp->adp$l_tr;	/* Save the TR number */
            status   = SS$_NORMAL;	/* Set up status to show success */
            break;			/*  and exit loop */
        }
    }

    sch_std$iounlock(CTL$GL_PCB);	/* Unlock I/O database */
    setipl(0);				/* Lower IPL back to 0 */
    return status;			/* Return with the status code */
}
int load_driver() {

int	status;				/* Service status code */
char	device_name[5];			/* Device name string */
char	driver_name[13];		/* Driver name */
int	efn;				/* Event flag number */
IOSB_T	iosb;				/* I/O status block */
__int64	csr;				/* Device CSR */
int	vector;				/* SCB vector */
int	node;				/* Node number */
int	ucb;				/* Address of UCB */
int	crb;				/* Address of CRB */
int	idb;				/* Address of IDB */
int	dpt;				/* Address of DPT */
int	ddb;				/* Address of DDB */

struct dsc$descriptor_d			/* Descriptor for device name */
    dev_dsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

struct dsc$descriptor_d			/* Descriptor for driver name */
     name_dsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

struct {				/* $LOAD_DRIVER item list */
    ITEM_T  items[8];			/* Define the items */
    int     end;			/* Define an end item */
} list;

/* Set up parameters - hardcoded for this example */

    csr		= 0x1f0;		/*  and he CSR */
    vector	= 4*0x0e;		/*  and the vector */
    node	= 3;			/*  and the node number */

/* Build the item list */

    list.items[0].buf_len = sizeof(isa_tr);
    list.items[0].item_code = IOGEN$_ADAPTER;
    list.items[0].buff_addr = &isa_tr;
    list.items[0].ret_addr = 0;

    list.items[1].buf_len = sizeof(csr);
    list.items[1].item_code = IOGEN$_CSR;
    list.items[1].buff_addr = &csr;
    list.items[1].ret_addr = 0;

    list.items[2].buf_len = sizeof(vector);
    list.items[2].item_code = IOGEN$_VECTOR;
    list.items[2].buff_addr = &vector;
    list.items[2].ret_addr = 0;

    list.items[3].buf_len = sizeof(node);
    list.items[3].item_code = IOGEN$_NODE;
    list.items[3].buff_addr = &node;
    list.items[3].ret_addr = 0;

    list.items[4].buf_len = sizeof(ucb);
    list.items[4].item_code = IOGEN$_UCB;
    list.items[4].buff_addr = &ucb;
    list.items[4].ret_addr = 0;

    list.items[5].buf_len = sizeof(ddb);
    list.items[5].item_code = IOGEN$_DDB;
    list.items[5].buff_addr = &ddb;
    list.items[5].ret_addr = 0;

    list.items[6].buf_len = sizeof(idb);
    list.items[6].item_code = IOGEN$_IDB;
    list.items[6].buff_addr = &idb;
    list.items[6].ret_addr = 0;

    list.items[7].buf_len = sizeof(crb);
    list.items[7].item_code = IOGEN$_CRB;
    list.items[7].buff_addr = &crb;
    list.items[7].ret_addr = 0;

    list.end = 0;			/* Terminate the item list */

/* Acquire an EFN */

    status = lib$get_ef(&efn);		/* Ask nicely for an EFN */
    chk_sts(status,"Unable to acquire EFN, status = %X",status);

/* Build the device and driver names and descriptors */

    strcpy(device_name,"DQA0");
    dev_dsc.dsc$w_length = strlen(device_name);
    dev_dsc.dsc$a_pointer= device_name;

    strcpy(driver_name,"SYS$DQDRIVER");
    name_dsc.dsc$w_length = strlen(driver_name);
    name_dsc.dsc$a_pointer= driver_name;

/* Try to load the driver */

    status = sys$load_driver(IOGEN$_CONNECT,
                             &dev_dsc,
                             &name_dsc,
                             &list,
                             &iosb,
                             efn,
                             0,0);
    chk_sts(status,"Unable to load driver, service status = %X",status);
    chk_sts(status,"Unable to load driver, IOSB status = %X",iosb.status);
    dpt = iosb.unused;			/* Copy over the DPT address */

    printf("Device %s loaded with driver %s\n",device_name,driver_name);
    printf(" UCB - %X, CRB - %X, DDB - %X, IDB - %X, DPT - %X\n",
           ucb, crb, ddb, idb, dpt);

    return	SS$_NORMAL;			/* Exit with success */
}

/* Main program */

int	main(int argc, char *argv[]) {

int	status;				/* Service status code */

    status = check_system_type();	/* Check the system type */
    chk_sts(status,"This system does not support an internal IDE interface\n");

/* Note: This routine should probably lock down it's working set into	*/
/*	 memory to be sure that we don't catch any pagefaults at a high	*/
/*	 IPL.								*/

    status = sys$cmkrnl(get_isa_base,0);/* Get the ISA bus address */
    chk_sts(status,"Error in obtaining ISA base address, status = %X",status);
    printf("ISA base address is 0x%X, ISA TR number is %d\n",isa_base,isa_tr);

    status = sys$cmkrnl(set_ide,0);	/* Set the bit */
    chk_sts(status,"Error in setting IDE enable, status = %X\n",status);

    status = load_driver();		/* Load the driver */
    chk_sts(status,"Error in loading driver, status = %X\n",status);
    exit(SS$_NORMAL);			/* Exit with success */
}
