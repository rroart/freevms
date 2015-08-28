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
 * Definitions for SNMP (RFC 1067) implementation.
 *
 *
 */

#ifndef SNMP_IMPL_H
#define SNMP_IMPL_H

#if (defined vax) || (defined (mips))
/*
 * This is a fairly bogus thing to do, but there seems to be no better way for
 * compilers that don't understand void pointers.
 */
#define void char
#endif

/*
 * Error codes:
 */
/*
 * These must not clash with SNMP error codes (all positive).
 */
#define PARSE_ERROR -1
#define BUILD_ERROR -2

#define SID_MAX_LEN 64
#define MAX_NAME_LEN    64  /* number of subid's in a objid */

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#define READ        1
#define WRITE       0

#define RONLY   0xAAAA  /* read access for everyone */
#define RWRITE  0xAABA  /* add write access for community private */
#define NOACCESS 0x0000 /* no access for anybody */

#define INTEGER     ASN_INTEGER
#define STRING      ASN_OCTET_STR
#define OBJID       ASN_OBJECT_ID
#define NULLOBJ     ASN_NULL

/* defined types (from the SMI, RFC 1065) */
#define IPADDRESS   (ASN_APPLICATION | 0)
#define COUNTER     (ASN_APPLICATION | 1)
#define GAUGE       (ASN_APPLICATION | 2)
#define TIMETICKS   (ASN_APPLICATION | 3)
#define OPAQUE      (ASN_APPLICATION | 4)

#ifdef DEBUG
#define ERROR(string)   printf("%s(%d): %s",__FILE__, __LINE__, string);
#else
#define ERROR(string)
#endif

/* from snmp.c*/
extern u_char   sid[];  /* size SID_MAX_LEN */

u_char  *snmp_parse_var_op();
u_char  *snmp_build_var_op();

u_char  *snmp_auth_parse();
u_char  *snmp_auth_build();

#endif /* SNMP_IMPL_H */
