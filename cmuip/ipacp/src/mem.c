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
#if 0
MODULE
    Memory (
	ADDRESSING_MODE (
	    EXTERNAL	= LONG_RELATIVE,
	    NONEXTERNAL	= LONG_RELATIVE),
	LANGUAGE (BLISS32),
	LIST (NOBINARY, ASSEMBLY, NOEXPAND)
	)
#endif

//++
// Mem.B32	Copyright (c) 1986	Carnegie Mellon University
//
// Description:
//
//	A few routines to aid with dynamic memory manegment.
//
// Written By:	Dale Moore	CMU-CS/RI
//
// Modifications:
//
//--

//LIBRARY "SYS$LIBRARY:XPORT";
#include <starlet.h>
#include	"tcp.h" 
#include "cmuip.h" // needed before tcpmacros.h
#include	"tcpmacros.h" 

#include <ssdef.h>

extern
LIB$GET_VM(),
  LIB$FREE_VM(),
  LIB$RESET_VM_ZONE(),
  LIB$STAT_VM();

extern signed long
    log_state ;

Get_Mem();
Free_Mem();
Reset_Mem();
 void    MEM_STAT();
void Mem_Stat (void);

void Make_Zone (void)
    {
    extern
      LIB$CREATE_USER_VM_ZONE(),
      LIB$CREATE_VM_ZONE(),
      LIB$DELETE_VM_ZONE;
    signed long
	Real_Zone,
	User_Zone,
	Status;

    Status = LIB$CREATE_VM_ZONE(&Real_Zone) ;
    XLOG$FAO(LOG$MEM,"!%T MAKE_ZONE: Status: !SL, Zone: !SL!/",
	0, Status, Real_Zone) ;
    if (! Status)
	{
	Signal (Status);
	};

    Status = LIB$CREATE_USER_VM_ZONE(&User_Zone, &Real_Zone,
	Get_Mem,
	Free_Mem,
	Reset_Mem,
	LIB$DELETE_VM_ZONE) ;
    XLOG$FAO(LOG$MEM,"!%T MAKE_ZONE: Status: !SL, Zone: !SL UZone: !SL!/",
	0, Status, Real_Zone, User_Zone) ;
    if (! Status)
	{
	Signal (Status);
	};
    Mem_Stat();
    return (User_Zone);
    }

Get_Mem (Size, Block_A, Zone)
    {
    signed long
	Status;

    XLOG$FAO(LOG$MEM,"!%T GET_MEM: Size: !SL, Addr: !SL, Zone: !SL!/",
	0, Size, Block_A, Zone) ;
    Status = LIB$GET_VM(Size, &Block_A, Zone) ;
    XLOG$FAO(LOG$MEM,"!%T GET_MEM: Status: !SL, Size: !SL, Addr: !SL, Zone: !SL!/",
	0, Status, Size, Block_A, Zone) ;
    Mem_Stat();
    return (Status);
    }

Free_Mem (Size, Block_A, Zone)
    {
    signed long
	Status;

    Status = LIB$GET_VM(Size, &Block_A, Zone) ;
    XLOG$FAO(LOG$MEM,"!%T FREE_MEM: Status: !SL, Size: !SL, Addr: !SL, Zone: !SL!/",
	0, Status, Size, Block_A, Zone) ;
    Mem_Stat() ;
    return (Status);
    }

Reset_Mem (Size, Block_A, Zone)
    {
    signed long
	Status;

    Status = LIB$RESET_VM_ZONE(Zone) ;
    XLOG$FAO(LOG$MEM,"!%T RESET_MEM: Status: !SL Size: !SL, Addr: !SL, Zone: !SL!/",
	0, Status, Size, Block_A, Zone) ;
    Mem_Stat() ;
    return (Status);
    }

void Mem_Stat (void)
    {
    signed long
	ngets,
	nfrees,
	nbytes,
	Status;

    Status = LIB$STAT_VM(/*%REF*/(1), &ngets) ;
    if (! Status) Signal (Status);
    Status = LIB$STAT_VM(/*%REF*/(2), &nfrees) ;
    if (! Status) Signal (Status);
    Status = LIB$STAT_VM(/*%REF*/(3), &nbytes) ;
    XLOG$FAO(LOG$MEM,"!%T MEM_STAT: Gets: !SL, Frees: !SL, Bytes: !SL!/",
	0,ngets, nfrees, nbytes);
    if (! Status) Signal (Status);
    }
