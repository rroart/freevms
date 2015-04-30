/*
    ****************************************************************

        Copyright (c) 1992, Carnegie Mellon University

        All Rights Reserved

    Permission  is  hereby  granted   to  use,  copy,  modify,  and
    distribute  this software  provided  that the  above  copyright
    notice appears in  all copies and that  any distribution be for
    noncommercial purposes.

    Carnegie Mellon University disclaims all warranties with regard
    to this software.  In no event shall Carnegie Mellon University
    be liable for  any special, indirect,  or consequential damages
    or any damages whatsoever  resulting from loss of use, data, or
    profits  arising  out of  or in  connection  with  the  use  or
    performance of this software.

    ****************************************************************
*/
/*
!++
! NETDEVICES.H  Copyright (c) 1990  Carnegie Mellon University
!
! Description:
!
!   Supporting declarations for IP Transport device modules
!
! Written By:   Bruce R. Miller     CMU Network Development
! Date:     06-Feb-1990 (Monday)
!
! Modifications:
!--
*/

#include <stdarg.h>
#include <descrip.h>

#ifndef NOKERNEL
#define sys$wake exe$wake
#endif

/* IPACP max physical buffer size*/
#define DRV$MAX_PHYSICAL_BUFSIZE (IPACP_Interface->ACPI$MPBS)

/* IAPCP callback macro. */
#define DRV$IP_RECEIVE(Buf,Buf_size,IPHdr,devlen,dev_config) \
    ((IPACP_Interface->ACPI$IP_Receive)(Buf,Buf_size,IPHdr,devlen,dev_config))

/* IPACP self-address recognition */
#define DRV$IP_ISME (IPACP_Interface->ACPI$IP_ISME)

/* Wake up the Big Guy */
#define DRV$ACPWAKE \
    { \
    if (*(IPACP_Interface->ACPI$Sleeping) == 1) \
        { \
        extern sys$wake(); \
         \
        IPACP_Interface->ACPI$Sleeping = FALSE; \
        sys$wake(0,0);              \
        } \
    }


/* pointer to the IPACP's AST_in_progress flag */
#define DRV$AST_IN_PROGRESS (*IPACP_Interface->ACPI$AST_in_progress)


/* Interrupt blocking routines */
#define DRV$NOINT ((IPACP_Interface->ACPI$NOINT)( ))
#define DRV$OKINT ((IPACP_Interface->ACPI$OKINT)( ))

/* Error reporting routine */
#define DRV$DEVICE_ERROR (IPACP_Interface->ACPI$Device_Error)

/* Memory management macros */
#define DRV$SEG_GET(Size) ((IPACP_Interface->ACPI$Seg_Get)(Size))
#define DRV$SEG_FREE (IPACP_Interface->ACPI$Seg_Free)

#define DRV$QBLK_FREE(Pntr) ((IPACP_Interface->ACPI$QBlk_Free)(Pntr))

/* Here are a bunch of macros stolen from TCPMACROS.REQ */

/*
! Note: the next six routines require the IPACP_Interface be declared
! globally.  IPACP_Interface is a (REF IPACP_Info_Structure) and points
! to the device module's IPACP Information Structure.
*/

/* Conditionally do something according to LOG_STATE flags */
#define $$LOGF SSLOGF
#define $$LOGF(LOGF) (((*IPACP_Interface->ACPI$LOG_STATE) && (LOGF)) != 0)

extern IPACP_Info_Structure * IPACP_Interface;

/* Write a message to the IPACP log file. */
static int inline DRV$LOG_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$LOG_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline DRV$XLOG_FAO(long i, ...)
{
    va_list args;
    long argv[18],argc=0;
    va_start(args,i);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    if ($$LOGF(i))
        return DRV$LOG_FAO(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

/*! Queue a message to the IPACP log file. */

static int inline DRV$QL_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$QL_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline DRV$XQL_FAO(long i, ...)
{
    va_list args;
    long argv[18],argc=0;
    va_start(args,i);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    if ($$LOGF(i))
        return DRV$QL_FAO(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

/*    ! Write a message to the console operator */

static int inline DRV$OPR_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$OPR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline DRV$ERROR_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$ERROR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline DRV$WARN_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$ERROR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline DRV$FATAL_FAO(char *c, ...)
{
    struct dsc$descriptor d;
    d.dsc$w_length=strlen(c);
    d.dsc$a_pointer=c;
    va_list args;
    long argv[18],argc=0;
    va_start(args,c);
    while(argc<18)
    {
        argv[argc]=va_arg(args,long);
        argc++;
    }
    va_end(args);
    return IPACP_Interface->ACPI$FATAL_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

/*
! Now we define some literals for use by the device support modules.
*/

/* Network logger flag bits - determine what events to log */

#define   LOG$PHY   0x01    /* Packet physical headers */
#define   LOG$ARP   0x02    /* ARP packet info */
#define   LOG$IP    0x04    /* IP packet headers */
#define   LOG$TCP   0x08    /* TCP segment info (packet trace) */
#define   LOG$TCBDUMP   0x10    /* TCB dump on servicing */
#define   LOG$USER  0x20    /* User I/O requests */
#define   LOG$TCBSTATE  0x40    /* TCB state changes */
#define   LOG$TCBCHECK  0x80    /* TCB servicing timing */
#define   LOG$TCPERR    0x100   /* TCP errors (dropped pkts, etc.) */
#define   LOG$ICMP  0x200   /* ICMP activity */
#define   LOG$UDP   0x400   /* UDP activity */
#define   LOG$TVT   0x800   /* TVT (virtual terminal) activity */
#define   LOG$IPERR 0x1000  /* IP errors (bad routing, etc.) */

