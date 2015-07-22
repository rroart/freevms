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
/*++
*******************************************************************************
*
* TITLE:    CMUNetError.H
*
* VERSION:  V1.0
*
* FACILITY:
*
*   CMU-OpenVMS/IP
*
* ABSTRACT:
*
*   C header file defining CMU-OpenVMS/IP error message constants
*
* ENVIRONMENT:
*
*   VAX/VMS
*
* AUTHOR:   Art Stine, Clarkson U/ERC
*           Copyright (c) 1990, Clarkson University
*
* CREATION DATE:    January, 1990
*
* MODIFIED BY:
*
* 1-001 A. Stine        Clarkson U/ERC
* Updated for V6.5 of CMU-OpenVMS/IP
*******************************************************************************
*--*/
#ifndef CMUNETERROR_H
#define CMUNETERROR_H

globalvalue NET$_IR, NET$_IFC, NET$_IPC, NET$_UCT, NET$_IFS, NET$_ILP,
            NET$_NUC, NET$_CSE, NET$_NOPRV, NET$_CIP, NET$_CDE, NET$_CR,
            NET$_FSU, NET$_UNN, NET$_VTF, NET$_CREF, NET$_CCAN, NET$_FIP,
            NET$_BTS, NET$_IHI, NET$_BDI, NET$_EPD, NET$_URC, NET$_IGF,
            NET$_UNA, NET$_UNU, NET$_CC, NET$_CTO, NET$_TWT, NET$_TE,
            NET$_FTO, NET$_NYI, NET$_NOPN, NET$_NOINA, NET$_NOANA, NET$_NOADR,
            NET$_GTHFUL, NET$_DAE, NET$_NMLTO, NET$_NSEXIT, NET$_NONS,
            NET$_NSQFULL, NET$_DSDOWN, NET$_DSNODS, NET$_DSINCOMP,
            NET$_DSNOADDR, NET$_DSNONAME, NET$_DSFMTERR, NET$_DSSRVERR,
            NET$_DSNAMERR, NET$_DSNOTIMP, NET$_DSREFUSD, NET$_DSNONSRV,
            NET$_DSUNKERR, NET$_DSREFEXC, NET$_GREENERR, NET$_GP_INVREQ,
            NET$_GP_INVINF, NET$_GP_INVNAM, NET$_GP_INVADR, NET$_GP_INVMBX,
            NET$_GP_INVCLS, NET$_GP_RSBUSY, NET$_GP_NONMSR, NET$_GP_NOHINF,
            NET$_GP_NOTFND, NET$_GP_UNKMBX, NET$_GP_NOTIMP, NET$_GP_TOOBIG,
            NET$_GP_NSDOWN, NET$_NRT;

#endif /* CMUNETERROR_H */
