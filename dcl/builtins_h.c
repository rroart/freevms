/*
================================================================================
  DCL/2 version 1.00
  	Digital Command Language

    Date de création    : 28 novembre 2001

    Tous droits réservés à l'auteur, Joël BERTRAND
	All rights reserved worldwide
================================================================================
*/


/*
================================================================================
  Copyright (C) 2001 BERTRAND Joël

  This file is part of DCL/2.

  DCL/2 is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.
            
  DCL/2 is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.
              
  You should have received a copy of the GNU General Public License
  along with Octave; see the file COPYING.  If not, write to the Free
  Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
================================================================================
*/


#include "dcl.h"

int
help_function(unsigned char *argument, dcl$command *self,
		dcl$command *commands, dcl$env *env)
/*
HELP

   The   HELP   command   invokes   the  HELP  Facility  to  display
   information about a command or topic.  In response to the "Topic?"
   prompt, you can:

      o Type  the  name of the command  or topic for which you need help.

      o Type  INSTRUCTIONS  for more detailed  instructions on how to use
        HELP.

      o Type  HINTS  if you are not  sure of the  name  of the command or
        topic for which you need help.

      o Type HELP/MESSAGE for help with the HELP/MESSAGE command.

      o Type a question mark (?) to redisplay the most recently requested
        text.

      o Press the Return key one or more times to exit from HELP.

   You  can  abbreviate any  topic name, although ambiguous abbreviations
   result in all matches being displayed.


  Additional information available:

  :=         =          @          ACCOUNTING ALLOCATE   ANALYZE    APPEND
  ASSIGN     ATTACH     AUTHORIZE  AUTOGEN    BACKUP     CALL       CANCEL
  CC         CLOSE      CMS        CONFIGURE  CONNECT    CONTINUE   CONVERT
  COPY       CREATE     CXXDEMANGLE           DCL_Tips   DEALLOCATE DEASSIGN
  DEBUG      DECK       DECnet-Plus           DECset     DECthreads DEFINE
  DELETE     DEPOSIT    DIFFERENCES           DIRECTORY  DISABLE
  DISCONNECT DISMOUNT   DSR        DTM        DUMP       EDIT       ENABLE
  ENDSUBROUTINE         EOD        EOJ        Errors     EXAMINE    EXCHANGE
  EXIT       Ext_File_Specs        FDL        FINGER     FONT       FTP
  GENCAT     GOSUB      GOTO       HELP       Hints      ICONV      IF
  INITIALIZE INQUIRE    INSTALL    Instructions          JAVA       JOB
  LANCP      LATCP      Lexicals   LIBRARY    LICENSE    Line_editing
  LINK       LMCP       LOCALE     LOGIN      LOGOUT     LPQ        LPRM
  LSEDIT     MACRO      MAIL       MERGE      MESSAGE    MIBCOMP    MMS
  MONITOR    MOUNT      NCP        NCS        NOTES      NOTICE     ON
  OPEN       PASSWORD   PATCH      PCA        PHONE      PIPE
  POLYCENTER PRINT      PRODUCT    PSWRAP     PURGE      Queues     RCP
  READ       RECALL     RECOVER    RENAME     REPLY      REQUEST    RETURN
  REXEC      RLOGIN     RMS        RPC        RPCGEN     RSH
  RTL_Routines          RUN        RUNOFF     SCA        SEARCH     SEND
  SET        SHOW       SORT       SPAWN      START      STOP       SUBMIT
  SUBROUTINE Symbol_Assign         SYNCHRONIZE           SYSGEN     SYSMAN
  System_Services       Sys_Files  Sys_Parameters        TCPIP_Services
  TCPTRACE   TELNET     TFF        TN3270     TYPE       UNLOCK
  V72_Features          VIEW       WAIT       WRITE
*/
{
	return(DCL$SUCCESS);
}
