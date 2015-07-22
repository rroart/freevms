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
* TITLE:    CMUNetDef.H
*
* VERSION:  V1.0
*
* FACILITY:
*
*   CMU-OpenVMS/IP
*
* ABSTRACT:
*
*   C header file defining a number of the common constants and
*   structures for CMU-OpenVMS/IP
*
* ENVIRONMENT:
*
*   VAX/VMS
*
* AUTHOR:       Art Stine, Clarkson U/ERC
*               Copyright (c) 1990, Clarkson University
*
* CREATION DATE:    January, 1990
*
* MODIFIED BY:
*
*   12-Nov-1990 Bruce R. Miller     CMU NetDev
*   WARNING!!!!  This file is bogus and obselete!
*
*******************************************************************************
*--*/

#ifndef CMUNETDEF_H
#define CMUNETDEF_H

/*
#define TCP$OPEN        IO$_CREATE
#define TCP$CLOSE       IO$_DELETE
#define TCP$SEND        IO$_WRITEVBLK
#define TCP$RECEIVE     IO$_READVBLK
#define TCP$ABORT       IO$_DEACCESS
#define TCP$STATUS      IO$_ACPCONTROL
#define TCP$INFO        IO$_MODIFY
#define IP_HOSTNAME_SIZE
                        128
#define Passive_Open    0
#define Active_Open     1
#define Wait            0
#define NoWait          1
#define TCP_Protocol    0
#define UDP_Protocol    1
#define Push            1
#define NoPush          0
#define Urgent          1
#define NoUrgent        0
#define NormalClose     0
#define AbortClose      1

struct cmutcp_info_block
    {
      unsigned char fhost_name_size;
      unsigned char lhost_name_size;
      char foreign_host[IP_HOSTNAME_SIZE];
      unsigned short unused1;
      unsigned int foreign_port;
      char local_host[IP_HOSTNAME_SIZE];
      unsigned int local_port;
      struct ipaddr
        {
          unsigned char octet1;
          unsigned char octet2;
          unsigned char octet3;
          unsigned char octet4;
        } local, foreign;
    };

struct cmutcp_status_block
    {
        short bytes_avail;
        char state, last_state;
        int send_window;
        int recv_window;
        int owner_pid;
    };
*/

#endif /* CMUNETDEF_H */
