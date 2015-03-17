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
 * snmp_auth.c -
 *   Authentication for SNMP (RFC 1067).  This implements a null
 * authentication layer.
 *
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#ifdef KINETICS
#include "gw.h"
#include "fp4/cmdmacro.h"
#endif

#if (defined(unix) && !defined(KINETICS))
#include <sys/types.h>
#include <netinet/in.h>
#ifndef NULL
#define NULL 0
#endif
#endif

#ifdef vms
#include in
#endif

#include "asn1.h"
#include "snmp.h"
#include "snmp_impl.h"

u_char *
snmp_auth_parse(data, length, sid, slen, version)
u_char	    *data;
int		    *length;
u_char	    *sid;
int		    *slen;
long	    *version;
{
    u_char    type;

    data = asn_parse_header(data, length, &type);
    if (data == NULL)
    {
        ERROR("bad header");
        return NULL;
    }
    if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        ERROR("wrong auth header type");
        return NULL;
    }
    data = asn_parse_int(data, length, &type, version, sizeof(*version));
    if (data == NULL)
    {
        ERROR("bad parse of version");
        return NULL;
    }
    data = asn_parse_string(data, length, &type, sid, slen);
    if (data == NULL)
    {
        ERROR("bad parse of community");
        return NULL;
    }
    sid[*slen] = '\0';
    return (u_char *)data;
}

u_char *
snmp_auth_build(data, length, sid, slen, version, messagelen)
u_char	    *data;
int		    *length;
u_char	    *sid;
int		    *slen;
long	    *version;
int		    messagelen;
{
    data = asn_build_header(data, length, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), messagelen + *slen + 5);
    if (data == NULL)
    {
        ERROR("buildheader");
        return NULL;
    }
    data = asn_build_int(data, length,
                         (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                         (long *)version, sizeof(*version));
    if (data == NULL)
    {
        ERROR("buildint");
        return NULL;
    }
    data = asn_build_string(data, length,
                            (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR),
                            sid, *slen);
    if (data == NULL)
    {
        ERROR("buildstring");
        return NULL;
    }
    return (u_char *)data;
}
