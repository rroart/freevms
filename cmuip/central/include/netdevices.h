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
! NETDEVICES.H	Copyright (c) 1990	Carnegie Mellon University
! 
! Description:
!
!	Supporting declarations for IP Transport device modules
!
! Written By:	Bruce R. Miller		CMU Network Development
! Date:		06-Feb-1990 (Monday)
!
! Modifications:
!--
*/

/* IPACP max physical buffer size*/
#define DRV$MAX_PHYSICAL_BUFSIZE (IPACP_Interface->ACPI$MPBS)

/* IAPCP callback macro. */
#define DRV$IP_RECEIVE(Buf,Buf_size,IPHdr,devlen,dev_config) \
	((IPACP_Interface->ACPI$IP_Receive)(Buf,Buf_size,IPHdr,devlen,dev_config))

/* IPACP self-address recognition */
#define DRV$IP_ISME (IPACP_Interface->ACPI$IP_ISME)

/* Wake up the Big Guy */
#define DRV$ACPWAKE() \
	{ \
	if (*(IPACP_Interface->ACPI$SLEEPING) == 1) then \
	    { \
	    extern SYS$WAKE; \
	     \
	    IPACP_Interface->ACPI$SLEEPING = FALSE; \
	    SYS$WAKE(); \
	    } \
	}


/* pointer to the IPACP's AST_in_progress flag */
#define DRV$AST_IN_PROGRESS (IPACP_Interface->ACPI$AST_in_progress)


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
#define $$LOGF(LOGF) (((*IPACP_Interface->ACPI$LOG_STATE) & (LOGF)) != 0)

    /* Write a message to the IPACP log file. */
/*#define DRV$LOG_FAO (.IPACP_Interface[ACPI$LOG_FAO]) */
/*! Queue a message to the IPACP log file. */

/*#define DRV$QL_FAO (.IPACP_Interface[ACPI$QL_FAO]) */

/*    ! Write a message to the console operator */

/*#define DRV$OPR_FAO (.IPACP_Interface[ACPI$OPR_FAO]) */

/*
    DRV$OPR_FAO(CST) =
	BEGIN
	LOCAL
	     STR_DESC : VECTOR [2];

	STR_DESC[0] = %CHARCOUNT(CST);
	STR_DESC[1] = UPLIT(CST);

	%IF %NULL(%REMAINING) %THEN
	    (.IPACP_Interface[ACPI$OPR_FAO])(STR_DESC);
	%ELSE
	    (.IPACP_Interface[ACPI$OPR_FAO])(STR_DESC,%REMAINING);
	%FI
	END
	%,

    DRV$ERROR_FAO(CST) =
	BEGIN
	LOCAL
	     STR_DESC : VECTOR [2];

	STR_DESC[0] = %CHARCOUNT(CST);
	STR_DESC[1] = UPLIT(CST);

	%IF %NULL(%REMAINING) %THEN
	    (.IPACP_Interface[ACPI$ERROR_FAO])(STR_DESC);
	%ELSE
	    (.IPACP_Interface[ACPI$ERROR_FAO])(STR_DESC,%REMAINING);
	%FI
	END
	%,

    DRV$WARN_FAO(CST) =
	BEGIN
	LOCAL
	     STR_DESC : VECTOR [2];

	STR_DESC[0] = %CHARCOUNT(CST);
	STR_DESC[1] = UPLIT(CST);

	%IF %NULL(%REMAINING) %THEN
	    (.IPACP_Interface[ACPI$ERROR_FAO])(STR_DESC);
	%ELSE
	    (.IPACP_Interface[ACPI$ERROR_FAO])(STR_DESC,%REMAINING);
	%FI
	END
	%,

    DRV$FATAL_FAO(CST) =
	BEGIN
	LOCAL
	     STR_DESC : VECTOR [2];

	STR_DESC[0] = %CHARCOUNT(CST);
	STR_DESC[1] = UPLIT(CST);

	%IF %NULL(%REMAINING) %THEN
	    (.IPACP_Interface[ACPI$FATAL_FAO])(STR_DESC);
	%ELSE
	    (.IPACP_Interface[ACPI$FATAL_FAO])(STR_DESC,%REMAINING);
	%FI
	END
        %;
*/

/*
! Now we define some literals for use by the device support modules.
*/

/* Network logger flag bits - determine what events to log */

#define   LOG$PHY	0x01	/* Packet physical headers */
#define   LOG$ARP	0x02	/* ARP packet info */
#define   LOG$IP	0x04	/* IP packet headers */
#define   LOG$TCP	0x08	/* TCP segment info (packet trace) */
#define   LOG$TCBDUMP	0x10	/* TCB dump on servicing */
#define   LOG$USER	0x20	/* User I/O requests */
#define   LOG$TCBSTATE  0x40	/* TCB state changes */
#define   LOG$TCBCHECK  0x80	/* TCB servicing timing */
#define   LOG$TCPERR	0x100	/* TCP errors (dropped pkts, etc.) */
#define   LOG$ICMP	0x200	/* ICMP activity */
#define   LOG$UDP	0x400	/* UDP activity */
#define   LOG$TVT	0x800	/* TVT (virtual terminal) activity */
#define   LOG$IPERR	0x1000	/* IP errors (bad routing, etc.) */

