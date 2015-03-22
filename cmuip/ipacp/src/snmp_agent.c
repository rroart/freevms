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
 * Simple Network Management Protocol (RFC 1067).
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

#ifdef __i386__
#define _I386_STRING_H_
#endif

#ifdef KINETICS
#include "gw.h"
#include "ab.h"
#include "inet.h"
#include "fp4/cmdmacro.h"
#include "fp4/pbuf.h"
#include "glob.h"
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
#define bcopy(s1,s2,size) memcpy(s2,s1,size)
#endif

#include "snmp.h"
#include "snmp_impl.h"
#include "asn1.h"

#include "mib.h"

#include "cmuip.h"

#include <linux/string.h>
#include <strings.h>

int snmp_input();
void    snmp_trap();
int create_identical();
int parse_var_op_list();
int snmp_access();

#define NUM_COMMUNITIES 5
char    *communities[NUM_COMMUNITIES] =
{
    "public",
    "proxy",
    "private",
    "regional",
    "core"
};

/* these can't be global in a multi-process router */
u_char  sid[SID_MAX_LEN + 1];
int     sidlen;
u_char  *packet_end;
int     community;


int
snmp_input(in_buff,in_len,out_buff,out_len)
char *in_buff,*out_buff;
int in_len;
int *out_len;
{
    int         out_length;
    u_short     udp_src;

    if (!snmp_agent_parse(in_buff, in_len, out_buff, out_len ))
    {
        return 0;
    }

    return 1;
}

int
snmp_agent_parse(data, length, out_data, out_length, sourceip)
register u_char *data;
int         length;
register u_char *out_data;
int         *out_length;
u_long      sourceip;   /* possibly for authentication */
{
    u_char      msg_type, type;
    long        zero = 0;
    long        reqid, errstat, errindex;
    register u_char *out_auth, *out_header, *out_reqid;
    u_char      *startData = data;
    int         startLength = length;
    long        version;
    int         header_shift, auth_shift;

    sidlen = SID_MAX_LEN;

    /* authenticates message and returns length if valid */
    data = snmp_auth_parse(data, &length, sid, &sidlen, &version);
    if (data == NULL)
    {
        ERROR("bad authentication");
        /* send auth fail trap */
        return 0;
    }
    if (version != SNMP_VERSION_1)
    {
        ERROR("wrong version");
        return NULL;
    }
    community = get_community(sid);
    if (community == -1)
        return NULL;
    data = asn_parse_header(data, &length, &msg_type);
    if (data == NULL)
    {
        ERROR("bad header");
        return 0;
    }
    if (msg_type != GET_REQ_MSG && msg_type != GETNEXT_REQ_MSG && msg_type != SET_REQ_MSG)
    {
        return 0;
    }
    data = asn_parse_int(data, &length, &type, &reqid, sizeof(reqid));
    if (data == NULL)
    {
        ERROR("bad parse of reqid");
        return 0;
    }
    data = asn_parse_int(data, &length, &type, &errstat, sizeof(errstat));
    if (data == NULL)
    {
        ERROR("bad parse of errstat");
        return 0;
    }
    data = asn_parse_int(data, &length, &type, &errindex, sizeof(errindex));
    if (data == NULL)
    {
        ERROR("bad parse of errindex");
        return 0;
    }
    /*
     * Now start cobbling together what is known about the output packet.
     * The final lengths are not known now, so they will have to be recomputed
     * later.
     */
    out_auth = out_data;
    out_header = snmp_auth_build(out_auth, out_length, sid, &sidlen, &zero, 0);
    if (out_header == NULL)
    {
        ERROR("snmp_auth_build failed");
        return 0;
    }
    out_reqid = asn_build_header(out_header, out_length, (u_char)GET_RSP_MSG, 0);
    if (out_reqid == NULL)
    {
        ERROR("");
        return 0;
    }

    type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    /* return identical request id */
    out_data = asn_build_int(out_reqid, out_length, type, &reqid, sizeof(reqid));
    if (out_data == NULL)
    {
        ERROR("build reqid failed");
        return 0;
    }

    /* assume that error status will be zero */
    out_data = asn_build_int(out_data, out_length, type, &zero, sizeof(zero));
    if (out_data == NULL)
    {
        ERROR("build errstat failed");
        return 0;
    }

    /* assume that error index will be zero */
    out_data = asn_build_int(out_data, out_length, type, &zero, sizeof(zero));
    if (out_data == NULL)
    {
        ERROR("build errindex failed");
        return 0;
    }

    errstat = parse_var_op_list(data, length, out_data, *out_length, msg_type, &errindex, 0);
    if (msg_type == SET_REQ_MSG && errstat == SNMP_ERR_NOERROR)
    {
        /*
         * SETS require 2 passes through the var_op_list.  The first pass verifies that
         * all types, lengths, and values are valid, and the second does the set.  Then
         * the identical GET RESPONSE packet is returned.
         */
        errstat = parse_var_op_list(data, length, out_data, *out_length, msg_type, &errindex, 1);
        if (create_identical(startData, out_auth, startLength, 0L, 0L))
            return 1;
        return 0;
    }
    switch((short)errstat)
    {
    case SNMP_ERR_NOERROR:
        /*
         * Because of the assumption above that header lengths would be encoded
         * in one byte, things need to be fixed, now that the actual lengths are known.
         */
        header_shift = 0;
        *out_length = packet_end - out_reqid;
        if (*out_length >= 0x80)
        {
            header_shift++;
            if (*out_length > 0xFF)
                header_shift++;
        }
        auth_shift = 0;
        *out_length = (packet_end - out_auth) - 2 + header_shift;
        if (*out_length >= 0x80)
        {
            auth_shift++;
            if (*out_length > 0xFF)
                auth_shift++;
        }
        if (auth_shift + header_shift)
        {
            /*
             * Shift packet (from request id to end of packet) by the sum of the
             * necessary shift counts.
             */
            shift_array(out_reqid, packet_end - out_reqid, auth_shift + header_shift);
            /* Now adjust pointers into the packet */
            packet_end += auth_shift + header_shift;
            out_reqid += auth_shift + header_shift;
            out_header += auth_shift;
        }

        /* re-encode the headers with the real lengths */
        out_data = out_header;
        *out_length = packet_end - out_reqid;
        out_data = asn_build_header(out_data, out_length, GET_RSP_MSG, *out_length);
        if (out_data != out_reqid)
        {
            ERROR("internal error: header");
            return 0;
        }

        out_data = out_auth;
        *out_length = packet_end - out_auth;
        out_data = snmp_auth_build(out_data, out_length, sid, &sidlen, &zero, packet_end - out_header);
        if (out_data != out_header)
        {
            ERROR("internal error");
            return 0;
        }
        break;
    case SNMP_ERR_NOSUCHNAME:
    case SNMP_ERR_TOOBIG:
    case SNMP_ERR_BADVALUE:
    case SNMP_ERR_READONLY:
    case SNMP_ERR_GENERR:
        if (create_identical(startData, out_auth, startLength, errstat, errindex))
            break;
        return 0;
    default:
        return 0;
    }
    *out_length = packet_end - out_auth;
    return 1;
}

/*
 * Parse_var_op_list goes through the list of variables and retrieves each one,
 * placing it's value in the output packet.  If doSet is non-zero, the variable is set
 * with the value in the packet.  If any error occurs, an error code is returned.
 */
int
parse_var_op_list(data, length, out_data, out_length, msgtype, index, doSet)
register u_char *data;
int         length;
register u_char *out_data;
int         out_length;
u_char      msgtype;
register long   *index;
int         doSet;
{
    u_char  type;
    oid     var_name[MAX_NAME_LEN];
    int     var_name_len, var_val_len;
    u_char  var_val_type, *var_val, statType;
    register u_char *statP;
    int     statLen;
    u_short acl;
    int     rw, exact;
    int     (*write_method)();
    u_char  *headerP, *var_list_start;
    int     dummyLen;
    int     header_shift;
    u_char  *getStatPtr();

    if (msgtype == SET_REQ_MSG)
        rw = WRITE;
    else
        rw = READ;
    if (msgtype == GETNEXT_REQ_MSG)
        exact = FALSE;
    else
        exact = TRUE;
    data = asn_parse_header(data, &length, &type);
    if (data == NULL)
    {
        ERROR("not enough space for varlist");
        return PARSE_ERROR;
    }
    if (type != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        ERROR("wrong type");
        return PARSE_ERROR;
    }
    headerP = out_data;
    out_data = asn_build_header(out_data, &out_length, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    if (out_data == NULL)
    {
        ERROR("not enough space in output packet");
        return BUILD_ERROR;
    }
    var_list_start = out_data;

    *index = 1;
    while((int)length > 0)
    {
        /* parse the name, value pair */
        var_name_len = MAX_NAME_LEN;
        data = snmp_parse_var_op(data, var_name, &var_name_len, &var_val_type, &var_val_len, &var_val, (int *)&length);
        if (data == NULL)
            return PARSE_ERROR;
        /* now attempt to retrieve the variable on the local entity */
        statP = getStatPtr(var_name, &var_name_len, &statType, &statLen, &acl, exact, &write_method);
        if (statP == NULL)
            return SNMP_ERR_NOSUCHNAME;
        /* Check if this user has access rights to this variable */
        if (!snmp_access(acl, community, rw))
            return SNMP_ERR_NOSUCHNAME; /* bogus */
        if (msgtype == SET_REQ_MSG)
        {
            if (write_method == 0)
            {
                /* see if the type and value is consistent with this entities variable */
                if (!goodValue(var_val_type, var_val_len, statType, statLen))
                {
                    return SNMP_ERR_BADVALUE;
                }
                /* actually do the set if necessary */
                if (doSet)
                    setVariable(var_val, var_val_type, var_val_len, statP, statLen);
            }
            else
            {
                if (!(*write_method)(doSet, var_val, var_val_type, var_val_len, statP))
                    return SNMP_ERR_BADVALUE;
            }
        }
        /* retrieve the value of the variable and place it into the outgoing packet */
        out_data = snmp_build_var_op(out_data, var_name, &var_name_len, statType, statLen, statP, &out_length);
        if (out_data == NULL)
        {
            return SNMP_ERR_TOOBIG;
        }

        (*index)++;
    }
    packet_end = out_data;  /* save a pointer to the end of the packet */

    /*
     * Because of the assumption above that header lengths would be encoded
     * in one byte, things need to be fixed, now that the actual lengths are known.
     */
    header_shift = 0;
    out_length = packet_end - var_list_start;
    if (out_length >= 0x80)
    {
        header_shift++;
        if (out_length > 0xFF)
            header_shift++;
    }
    if (header_shift)
    {
        shift_array(var_list_start, packet_end - var_list_start, header_shift);
        packet_end += header_shift;
        var_list_start += header_shift;
    }

    /* Now rebuild header with the actual lengths */
    dummyLen = packet_end - var_list_start;
    if (asn_build_header(headerP, &dummyLen, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == NULL)
    {
        return SNMP_ERR_TOOBIG; /* bogus error ???? */
    }
    *index = 0;
    return SNMP_ERR_NOERROR;
}

/*
 * create a packet identical to the input packet, except for the error status
 * and the error index which are set according to the input variables.
 * Returns 1 upon success and 0 upon failure.
 */
int
create_identical(snmp_in, snmp_out, snmp_length, errstat, errindex)
u_char      *snmp_in;
u_char      *snmp_out;
int         snmp_length;
long        errstat, errindex;
{
    register u_char *data;
    u_char      type;
    u_long      dummy;
    int         length, headerLength;
    register u_char *headerPtr, *reqidPtr, *errstatPtr, *errindexPtr, *varListPtr;

    bcopy((char *)snmp_in, (char *)snmp_out, snmp_length);
    length = snmp_length;
    headerPtr = snmp_auth_parse(snmp_out, &length, sid, &sidlen, (long *)&dummy);
    if (headerPtr == NULL)
        return 0;
    reqidPtr = asn_parse_header(headerPtr, &length, (u_char *)&dummy);
    if (reqidPtr == NULL)
        return 0;
    headerLength = length;
    errstatPtr = asn_parse_int(reqidPtr, &length, &type, (long *)&dummy, sizeof dummy); /* request id */
    if (errstatPtr == NULL)
        return 0;
    errindexPtr = asn_parse_int(errstatPtr, &length, &type, (long *)&dummy, sizeof dummy);  /* error status */
    if (errindexPtr == NULL)
        return 0;
    varListPtr = asn_parse_int(errindexPtr, &length, &type, (long *)&dummy, sizeof dummy);  /* error index */
    if (varListPtr == NULL)
        return 0;

    data = asn_build_header(headerPtr, &headerLength, GET_RSP_MSG, headerLength);
    if (data != reqidPtr)
        return 0;
    length = snmp_length;
    type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    data = asn_build_int(errstatPtr, &length, type, &errstat, sizeof errstat);
    if (data != errindexPtr)
        return 0;
    data = asn_build_int(errindexPtr, &length, type, &errindex, sizeof errindex);
    if (data != varListPtr)
        return 0;
    packet_end = snmp_out + snmp_length;
    return 1;
}

int
snmp_access(acl, community, rw)
u_short     acl;
int     community;
int     rw;
{
    /*
     * Each group has 2 bits, the more significant one is for read access,
     * the less significant one is for write access.
     */

    community <<= 1;    /* multiply by two two shift two bits at a time */
    if (rw == READ)
    {
        return (acl & (2 << community));    /* return the correct bit */
    }
    else
    {
        return (acl & (1 << community));
    }
}

int
get_community(sessionid)
u_char  *sessionid;
{
    int count;

    for(count = 0; count < NUM_COMMUNITIES; count++)
    {
        if (!strcmp(communities[count], (char *)sessionid))
            break;
    }
    if (count == NUM_COMMUNITIES)
        return -1;
    return count;
}

int
goodValue(inType, inLen, actualType, actualLen)
u_char  inType, actualType;
int     inLen, actualLen;
{
    if (inLen > actualLen)
        return FALSE;
    return (inType == actualType);
}

setVariable(var_val, var_val_type, var_val_len, statP, statLen)
u_char  *var_val;
u_char  var_val_type;
int     var_val_len;
u_char  *statP;
int     statLen;
{
    int     buffersize = 1000;

    switch(var_val_type)
    {
    case ASN_INTEGER:
        asn_parse_int(var_val, &buffersize, &var_val_type, (long *)statP, statLen);
        break;
    case COUNTER:
    case GAUGE:
    case TIMETICKS:
        asn_parse_unsigned_int(var_val, &buffersize, &var_val_type, (u_long *)statP, statLen);
        break;
    case ASN_OCTET_STR:
    case IPADDRESS:
    case OPAQUE:
        asn_parse_string(var_val, &buffersize, &var_val_type, statP, &statLen);
        break;
    case ASN_OBJECT_ID:
        asn_parse_objid(var_val, &buffersize, &var_val_type, (oid *)statP, &statLen);
        break;
    }
}


