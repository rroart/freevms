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
/****************************************************************************

		RPC - Remote Procedure Call module
Facility:

	RPC.C - Provide port mapping service under UDP (RFC 1057)

Abstract:

	Implements Sun's remote procedure call mechanism

Author:

	Bruce R. Miller, CMU NetDev, 12-Nov-1990
	Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

 ***************************************************************************/

#if 0
#module RPC
#endif

#define globaldef

typedef unsigned int u_int;

#include <stdio.h>
#include <stdlib.h>
#include <ssdef.h>
#include <ctype.h>
#include <descrip.h>
#if 0
// not yet
#include <uaidef.h>
#else
#define UAI$_UIC 6
#endif
#include <string.h>
#include <opcdef.h>

#include <netinet/in.h>			/* network defs (defines u_long!) */

#include "rpc_types.h"		/* usefull and relavant definitions */
#include "xdr.h"
#include "auth.h"		/* authorization structs */
#include "rpc_msg.h"		/* protocol for rpc messages */
#include "rpc.h"
#include <nettcpip.h>		/* TCP/IP suite definitions */



#if 0
#define RPC_VERSION		 2
#define RPC_VERSION_LOW		 2
#define RPC_VERSION_HIGH	 2
#define MAX_RPC_SRV		10
#else
static const int RPC_VERSION = 2;
static const int RPC_VERSION_LOW = 2;
static const int RPC_VERSION_HIGH = 2;
static const int MAX_RPC_SRV = 2;
static const int rpc_mismatch = RPC_MISMATCH;
static const int msg_denied = MSG_DENIED;
static const int msg_accepted = MSG_ACCEPTED;
static const int reply = REPLY;
static const int auth_error = AUTH_ERROR;
static const int auth_none = AUTH_NONE;
static const int auth_badcred = AUTH_BADCRED;
static const int auth_badverf = AUTH_BADVERF;
static const int prog_unavail = PROG_UNAVAIL;
static const int prog_mismatch = PROG_MISMATCH;
static const int proc_unavail = PROC_UNAVAIL;
static const int garbage_args = GARBAGE_ARGS;
static const int success = SUCCESS;
#define DEF_MAX_RPC_SRV		10
#endif

#define RPC_MAX_REPLY_CACHE	10

long GMT_OFFSET = 0;
char *NFS_ANONYMOUS = NULL;

char *strupr(char *s)
{
    char *_ptr = s;
    while (*_ptr)
    {
        *_ptr = toupper(*_ptr);
        _ptr++;
    }
    return s;
}

struct mapping
{
    unsigned int prog;
    unsigned int vers;
    unsigned int prot;
    unsigned int port;
};



struct service_record
{
    struct service_record *next;
    struct service_record *prev;
    unsigned int prog;
    unsigned int vers;
    unsigned int prot;
    unsigned int port;
    int (*init_routine)();
    int (*dispatch_routine)();
    char name[20];
    char image[40];
};



globaldef int RPC_SERVICE = 0;
int RPCsrv_count = 0;
struct service_record RPCsrv_tab[DEF_MAX_RPC_SRV];

int xdr_int(int x)
{
    int y;
    XDR$ntov_int(&x,&y);
    return(y);
}

extern IPACP_Int;



/*********************************************************************

			UIC translation routines

**********************************************************************/

typedef struct
{
    struct uicmap_entry *next;
    int remuid, remgid;
    char *remhost;
    unsigned long localuser;
    char *localusername;
} uicmap_entry;

struct
{
    uicmap_entry *head, *tail;
} UICMAP_table = {NULL, NULL};


int add_uic_translation(int uid, int gid, char *hname, char *username)
{
    struct dsc$descriptor usrnam;
    struct
    {
        unsigned short buflen, itmcod;
        char *bufadr, *retlenadr;
        unsigned long null;
    } uailist;
    int RC;

    if (!UICMAP_table.head)
        UICMAP_table.head = UICMAP_table.tail =
                                (uicmap_entry *)malloc(sizeof(uicmap_entry));
    else
    {
        UICMAP_table.tail->next =
            (uicmap_entry *)malloc(sizeof(uicmap_entry));
        UICMAP_table.tail = UICMAP_table.tail->next;
    }
    UICMAP_table.tail->next = NULL;
    UICMAP_table.tail->remuid = uid;
    UICMAP_table.tail->remgid = gid;
    UICMAP_table.tail->remhost = (char *)malloc(strlen(hname) + 1);
    strcpy(UICMAP_table.tail->remhost, hname);
    UICMAP_table.tail->localusername = (char *)malloc(strlen(username) + 1);
    strcpy(UICMAP_table.tail->localusername, username);
    if (!strcmp(username, "*"))
        UICMAP_table.tail->localuser = -1;
    else
    {
        usrnam.dsc$w_length = strlen(username);
        usrnam.dsc$b_dtype = DSC$K_DTYPE_T;
        usrnam.dsc$b_class = DSC$K_CLASS_S;
        usrnam.dsc$a_pointer = username;

        uailist.buflen = sizeof(UICMAP_table.tail->localuser);
        uailist.itmcod = UAI$_UIC;
        uailist.bufadr = &UICMAP_table.tail->localuser;
        uailist.retlenadr = 0;
        uailist.null = 0;

        RC = SYS$GETUAI(0, 0, &usrnam, &uailist, 0, 0, 0);
        if (!(RC & 0x1))	    /* Map unknown users to -1,-1 (denied) */
            UICMAP_table.tail->localuser = -1;
    }

    return TRUE;
}

int compare_hosts(char *hostname, char *hostpattern)
{
    /* hostpattern may contain many strings - we use strtok() on it checking
       for comma-delimited possibilities... */
    char *thispattern;
    struct dsc$descriptor candidate, pattern;
    char hostcopy[513];

    /*
     * The caller supplied hostpattern is often the live data from places like
     * the exports table.  The strtok()'s and STR$UPCASE()'s are destructive
     * which means that this routine has been destroying live data!
     *
     * Tom Allebrandi (Allebrandi@Inland.Com) 11-Oct-1991
     */
    if (strlen(hostpattern) < sizeof(hostcopy) - 1)
        strcpy(hostcopy, hostpattern);
    else
    {
        strncpy(hostcopy, hostpattern, sizeof(hostcopy) - 1);
        hostcopy[sizeof(hostcopy) - 1] = '\0';
    }

    thispattern = strtok(hostcopy, ",");

    while (thispattern)
    {
        candidate.dsc$w_length = strlen(hostname);
        pattern.dsc$w_length = strlen(thispattern);
        candidate.dsc$b_dtype = pattern.dsc$b_dtype = DSC$K_DTYPE_T;
        candidate.dsc$b_class = pattern.dsc$b_class = DSC$K_CLASS_S;
        candidate.dsc$a_pointer = hostname;
        pattern.dsc$a_pointer = thispattern;

        STR$UPCASE(&candidate, &candidate);
        STR$UPCASE(&pattern, &pattern);

        if (STR$MATCH_WILD(&candidate, &pattern) & 0x1)
            return 1;

        thispattern = strtok(NULL, ",");
    }

    return 0;
}

/* Translate Unix authentification (uid,gid,hname) into VMS UIC. */

unsigned map_uic(unsigned *uic, int uid, int gid, char *hname)
{
    uicmap_entry *uicptr;

    uicptr = UICMAP_table.head;
    while (uicptr)
    {
        /* Comparison against group removed due to problems with remote
           systems not necessarily having group information (such as AFS
           workstations) */
        if (((uicptr->remuid == -2) || (uicptr->remuid == uid)) &&
                /*  ((uicptr->remgid == -2) || (uicptr->remgid == gid)) && */
                (compare_hosts(hname, uicptr->remhost)))
        {
            *uic = uicptr->localuser;
            return uicptr->localusername;
        }
        uicptr = uicptr->next;
    }
    return FALSE;
}


/* Translate VMS UIC (with hostname) into Unix authentification (uid,gid). */

unsigned map_unix(unsigned uic, unsigned short *uid, unsigned short *gid,
                  char *hostname)
{
    uicmap_entry *uicptr;

    uicptr = UICMAP_table.head;
    while (uicptr)
    {
        if (((uicptr->localuser == -1) || (uic == uicptr->localuser)) &&
                (compare_hosts(hostname, uicptr->remhost)))
        {
            *uid = uicptr->remuid;
            *gid = uicptr->remgid;
            return TRUE;
        }
        uicptr = uicptr->next;
    }
    return FALSE;
}

/*  Information kept about exported filesystems  */
typedef struct
{
    struct export_entry *next;
    char *path, *rempath;
    char *remhost;
} export_entry;

struct
{
    export_entry *head, *tail;
} EXPORT_table = { NULL, NULL };

add_export_path(char *path, char *remhost)
{
    char *dirpart, *tmpdir;

    if (!EXPORT_table.head)
        EXPORT_table.head = EXPORT_table.tail =
                                (export_entry *)malloc(sizeof(export_entry));
    else
    {
        EXPORT_table.tail->next =
            (export_entry *)malloc(sizeof(export_entry));
        EXPORT_table.tail = EXPORT_table.tail->next;
    }

    EXPORT_table.tail->next = NULL;
    EXPORT_table.tail->path = (char *)malloc(strlen(path) + 1);
    strcpy(EXPORT_table.tail->path, path);
    EXPORT_table.tail->rempath = (char *)malloc(strlen(path) + 1);
    *(EXPORT_table.tail->rempath) = 0;
    tmpdir = (char *)malloc(strlen(path) + 1);
    *tmpdir = 0;

    dirpart = strtok(path, "/");
    while (dirpart)
    {
        FNAME_VMS_TO_NET(dirpart, strlen(dirpart), tmpdir, 1, 1);
        strcat(EXPORT_table.tail->rempath, "/");
        strcat(EXPORT_table.tail->rempath, tmpdir);
        dirpart = strtok(NULL, "/");
    }

    if (!*(EXPORT_table.tail->rempath))
        strcpy(EXPORT_table.tail->rempath, "/");

    EXPORT_table.tail->remhost = (char *)malloc(strlen(remhost) + 1);
    strcpy(EXPORT_table.tail->remhost, remhost);

    free(tmpdir);

    return TRUE;
}

int compare_paths(char *rempath, char *hostpath)
/* In order for the paths to be "equal", `rempath' *must* contain `hostpath'
   as a possible abbreviation. */
{
    if (strlen(rempath) >= strlen(hostpath))
        return (!strncmp(hostpath, rempath, strlen(hostpath)));
    else
        return FALSE;
}

int check_export(char *path, char *remhost)
/* check_export checks to see if `remhost' is permitted to mount `path' by
   seeing if `path' is a proper subset of one of the entries in the
   EXPORT_table list for that host */
{
    export_entry *exptr;

    exptr = EXPORT_table.head;
    while (exptr)
    {
        if (compare_paths(path, exptr->rempath) &&
                compare_hosts(remhost, exptr->remhost))
            return TRUE;
        exptr = exptr->next;
    }
    return FALSE;
}

RPC$get_exports(char *buffer)
{
    export_entry *exptr;
    char *curremhost;
    char remhostcopy[513];

    *buffer = 0;
    exptr = EXPORT_table.head;

    while (exptr)
    {
        strcat(buffer, exptr->rempath);
        strcat(buffer, ":");
        /*
         * The original code here did strtok()'s on the live data.  Since
         * strtok() is destructive, this is not a real good idea.
         *
         * Tom Allebrandi (Allebrandi@Inland.Com) 11-Oct-1991
         */
        if (strlen(exptr->remhost) < sizeof(remhostcopy) - 1)
            strcpy(remhostcopy, exptr->remhost);
        else
        {
            strncpy(remhostcopy, exptr->remhost, sizeof(remhostcopy) - 1);
            remhostcopy[sizeof(remhostcopy) - 1] = '\0';
        }

        curremhost = strtok(remhostcopy, ",");
        while (curremhost)
        {
            strcat(buffer, curremhost);
            strcat(buffer, ":");
            curremhost = strtok(NULL, ",");
        }
        strcat(buffer, " :");
        exptr = exptr->next;
    }
}

/*
	RPC$CONFIG - Add an entry to the RPC services table'

    Called by the CONFIG module when an RPC command is seen, this routine adds
    the new RPC entry to the table.  Returns table index of new entry, or
    else -1 if addition fails.
*/

int RPC$CONFIG(name_desc,prog,vers,prot,port,imname_desc)
struct dsc$descriptor *name_desc,*imname_desc;
int prog,vers,prot,port;
{
    int idx,len;

    /* Make sure there is room for this entry */

    if (RPCsrv_count >= MAX_RPC_SRV)
        return(-1);
    idx = RPCsrv_count++;

    /* Fill in the WKS entry fields */

    /* Do the numbers first */
    RPCsrv_tab[idx].prog = prog;
    RPCsrv_tab[idx].vers = vers;
    RPCsrv_tab[idx].prot = prot;
    RPCsrv_tab[idx].port = port;

    /* Process Name */
    len = name_desc->dsc$w_length;
    strncpy(&RPCsrv_tab[idx].name,name_desc->dsc$a_pointer,len);
    RPCsrv_tab[idx].name[len]=0;

    /* Process image file */
    len = imname_desc->dsc$w_length;
    strncpy(&RPCsrv_tab[idx].image,imname_desc->dsc$a_pointer,len);
    RPCsrv_tab[idx].name[len]=0;

    return idx;
}



/*
	RPC$CONFIG_AUTH - Add an authorization entry to the RPC table'

    Called by the CONFIG module when an AUTH command is seen, this routine adds
    the new AUTH entry to the table.  Returns -1 if addition fails.
*/

int RPC$CONFIG_AUTH(uic, uid, gid, hostname_desc)
struct dsc$descriptor *hostname_desc;
int uic,uid,gid;
{
    char tbuff[256];	/* !!!HACK!!! hardwired constant. */

    /*    memcpy(hostname_desc->dsc$a_pointer,tbuff,hostname_desc->dsc$w_length);
        tbuff[hostname_desc->dsc$w_length] = 0;

        if (!add_uic_translation(uic,uid,gid,tbuff))
    	return -1; */
    return 1;
}



extern int PMAP$INIT();
extern int MNT$INIT();
extern int NFS$INIT();
int PCNFSD$INIT(), PCNFSDPROC_NULL(), PCNFSDPROC_AUTH(), PCNFSD$DISPATCH();

int RPC$INIT()
{
    int idx;
    char *pmap_name = "PORTMAPPER";
    char *mnt_name = "MOUNT";
    char *nfs_name = "NFS";
    char *pcnfsd_name = "PCNFSD";
    $DESCRIPTOR(imname_desc,"");
    FILE *NFSconfig;
    char NFSconfigline[100], *parameter, *newline;
    int remuid, remgid;
    char remhost[80], locuser[32], exportpath[80];

    if ((NFSconfig = fopen("NFS$CONFIG", "r")) != NULL)
    {
        while (fgets(NFSconfigline, sizeof(NFSconfigline), NFSconfig))
        {
            newline = strrchr(NFSconfigline, '\n');
            if (newline)
                *newline = 0;
            if ((!strlen(NFSconfigline)) || (NFSconfigline[0] == ';'))
                continue;

            /* Not a comment or blank line - check for keywords */
            parameter = strupr(strtok(NFSconfigline, ":"));
            if (!strcmp(parameter, "AUTH"))
            {
                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                if (!strcmp(parameter, "*"))
                    remuid = -2;
                else
                    remuid = atoi(parameter);

                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                if (!strcmp(parameter, "*"))
                    remgid = -2;
                else
                    remgid = atoi(parameter);

                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                strcpy(remhost, parameter);

                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                strcpy(locuser, parameter);

                add_uic_translation(remuid, remgid, remhost, locuser);
            }
            else if (!strcmp(parameter, "EXPORT"))
            {
                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                strcpy(exportpath, parameter);

                parameter = strtok(NULL, ":");
                if (parameter == NULL)
                    continue;
                strcpy(remhost, parameter);

                add_export_path(exportpath, remhost);
            }
            else if (!strcmp(parameter, "GMT_OFFSET"))
            {
                parameter = strtok(NULL, ":");
                GMT_OFFSET = (long)(atof(parameter) * 3600.0);
            }
            else if (!strcmp(parameter, "ANONYMOUS_USER"))
            {
                parameter = strupr(strtok(NULL, ":"));
                NFS_ANONYMOUS = (char *)malloc(strlen(parameter) + 1);
                strcpy(NFS_ANONYMOUS, parameter);
            }
        }
        fclose(NFSconfig);
    }


    /******** internal PORT MAPPER ********************************/

    /* Make sure there is room for this entry */
    if (RPCsrv_count >= MAX_RPC_SRV)
        return(0);
    idx = RPCsrv_count++;

    /* Fill in the RPC entry fields */

    /* Do the numbers first */
    RPCsrv_tab[idx].prog = RPCPROG_PMAP;
    RPCsrv_tab[idx].vers = 2;		/* !!!HACK!!! */
    RPCsrv_tab[idx].prot = UDP_Protocol;
    RPCsrv_tab[idx].port = UDP_PORT_SUNRPC;

    /* Process Name */
    strncpy(RPCsrv_tab[idx].name,pmap_name,strlen(pmap_name));

    /* Process image file */
    RPCsrv_tab[idx].name[0] = 0;

    RPCsrv_tab[idx].init_routine = PMAP$INIT;
    RPCsrv_tab[idx].dispatch_routine =
        (*RPCsrv_tab[idx].init_routine)(&IPACP_Int);



    /******** internal MOUNT ********************************/

    /* Make sure there is room for this entry */
    if (RPCsrv_count >= MAX_RPC_SRV)
        return(0);
    idx = RPCsrv_count++;

    /* Fill in the RPC entry fields */

    /* Do the numbers first */
    RPCsrv_tab[idx].prog = RPCPROG_MOUNT;
    RPCsrv_tab[idx].vers = 1;		/* !!!HACK!!! */
    RPCsrv_tab[idx].prot = UDP_Protocol;
    RPCsrv_tab[idx].port = 2000;

    /* Process Name */
    strncpy(RPCsrv_tab[idx].name,mnt_name,strlen(mnt_name));

    /* Process image file */
    RPCsrv_tab[idx].name[0] = 0;

    RPCsrv_tab[idx].init_routine = MNT$INIT;
    RPCsrv_tab[idx].dispatch_routine =
        (*RPCsrv_tab[idx].init_routine)(&IPACP_Int);

    /******** internal NFS ********************************/

    /* Make sure there is room for this entry */
    if (RPCsrv_count >= MAX_RPC_SRV)
        return(0);
    idx = RPCsrv_count++;

    /* Fill in the RPC entry fields */

    /* Do the numbers first */
    RPCsrv_tab[idx].prog = RPCPROG_NFS;
    RPCsrv_tab[idx].vers = 2;		/* !!!HACK!!! */
    RPCsrv_tab[idx].prot = UDP_Protocol;
    RPCsrv_tab[idx].port = UDP_PORT_NFS;

    /* Process Name */
    strncpy(RPCsrv_tab[idx].name,nfs_name,strlen(nfs_name));

    /* Process image file */
    RPCsrv_tab[idx].name[0] = 0;

    RPCsrv_tab[idx].init_routine = NFS$INIT;
    RPCsrv_tab[idx].dispatch_routine =
        (*RPCsrv_tab[idx].init_routine)(&IPACP_Int);

    /********   internal PCNFSD ****************************/

    /* Make sure there is room for yet another entry */
    if (RPCsrv_count >= MAX_RPC_SRV)
        return(0);
    idx = RPCsrv_count++;

    /* Fill in the RPC entry fields */

    /* Do the numbers first */
    RPCsrv_tab[idx].prog = RPCPROG_PCNFSD;
    RPCsrv_tab[idx].vers = 1;
    RPCsrv_tab[idx].prot = UDP_Protocol;
    RPCsrv_tab[idx].port = UDP_PORT_AUTH;

    /* Process Name */
    strncpy(RPCsrv_tab[idx].name, pcnfsd_name, strlen(pcnfsd_name));

    /* Process image file */
    RPCsrv_tab[idx].name[0] = 0;

    RPCsrv_tab[idx].init_routine = PCNFSD$INIT;
    RPCsrv_tab[idx].dispatch_routine =
        (*RPCsrv_tab[idx].init_routine)(&IPACP_Int);
}



/* Returns index of RPC service at given port.  Returns -1 if
   there is no service at that port. */

int RPC$CHECK_PORT(port)
{
    int i;

    for (i=0; i<RPCsrv_count; i++)
        if (RPCsrv_tab[i].port == port)
            return i;
    return -1;
}



/*
    Main input routine.  UDP datagrams to RPC serviced ports are sent here.
*/

int RPC$INPUT ( index, SrcAddr , DstAdr , SrcPrt , DstPrt ,
                Size , Buff , out, len)
int index;
unsigned int	SrcAddr, DstAdr;
short unsigned int	SrcPrt, DstPrt;
int Size, *len;
struct rpc_msg *Buff,*out;
{
    long unsigned int xid, auth_code;
    enum msg_type direction;
    struct call_body *call_body;
    struct reply_body *reply_body;
    int result_len,RC;
    long *body;
    unsigned short uid=0, gid=0;
    unsigned char hname[256] = {0};
    unsigned uic;
    int checkcache;
    static struct
    {
        int RC;
        long unsigned int xid, len;
        int index;
        unsigned char hname[256];
        struct rpc_msg *out;
    } oldreplies[RPC_MAX_REPLY_CACHE];
    char *username;
    int authmechanism;
    static int nextcache, initialized = 0;

    if (!initialized)
    {
        int loop;
        for (loop = 0; loop < RPC_MAX_REPLY_CACHE; loop++)
        {
            oldreplies[loop].RC, oldreplies[loop].xid, oldreplies[loop].len = 0;
            oldreplies[loop].index = 0;
            oldreplies[loop].out = NULL;
        }
        initialized++;
        nextcache = 0;
    }

    /* decode call and set up reply */
    XDR$ntov_uint(&Buff->rm_xid,&xid);
    out->rm_xid = Buff->rm_xid;
    XDR$ntov_int(&Buff->rm_direction,&direction);
    XDR$vton_int(&reply,&out->rm_direction);
    *len = 8;

    if (direction != CALL) return(0);
    call_body = &Buff->rm_call;

    /* we now have a call */
    XDR$ntov_uint(&call_body->cb_rpcvers,&call_body->cb_rpcvers);
    XDR$ntov_uint(&call_body->cb_prog,&call_body->cb_prog);
    XDR$ntov_uint(&call_body->cb_vers,&call_body->cb_vers);
    XDR$ntov_uint(&call_body->cb_proc,&call_body->cb_proc);

    if (call_body->cb_rpcvers != RPC_VERSION)
    {
        XDR$vton_int(&msg_denied,&out->rm_reply.rp_stat);
        *len += 4;
        XDR$vton_int(&rpc_mismatch,&out->rm_reply.rp_rjct.rj_stat);
        *len += 4;
        XDR$vton_int(&RPC_VERSION_LOW, &out->rm_reply.rp_rjct.rj_vers.low);
        XDR$vton_int(&RPC_VERSION_HIGH, &out->rm_reply.rp_rjct.rj_vers.high);
        *len += 8;
        return 1;
    }

    body = &call_body->cb_cred;

    /* Check out those credentials... */
    switch (authmechanism = xdr_int(*body++))
    {
    case AUTH_NULL :
    {
        body += (RNDUP(xdr_int(*body))/4) + 1;
        break;
    }

    case AUTH_UNIX :
    {
        long *tmp;
        unsigned int stamp,unix_len,name_len,unix_uid,unix_gid;

        XDR$vton_uint(body++,&unix_len);
        tmp = body;
        body += RNDUP(unix_len)/4;
        if (tmp==body) break;

        XDR$vton_uint(tmp++,&stamp);
        XDR$vton_uint(tmp++,&name_len);
        memcpy(hname,tmp,name_len);
        hname[name_len]=0;
        strupr(hname);
        tmp += RNDUP(name_len)/4;
        XDR$vton_uint(tmp++,&unix_uid);
        XDR$vton_uint(tmp++,&unix_gid);
        uid=unix_uid;
        gid=unix_gid;
        break;
    }

    case AUTH_SHORT :
    {
        body += (RNDUP(xdr_int(*body))/4) + 1;
        break;
    }

    default :
    {
        XDR$vton_int(&msg_denied,&out->rm_reply.rp_stat);
        XDR$vton_int(&auth_error,&out->rm_reply.rp_rjct.rj_stat);
        XDR$vton_int(&auth_badcred,&out->rm_reply.rp_rjct.rj_why);
        *len += 12;
        return(1);
    }
    }

    /* Check the verifier */
    switch (xdr_int(*body++))
    {
    case AUTH_NULL :
    {
        body += (RNDUP(xdr_int(*body))/4) + 1;
        break;
    }

    case AUTH_UNIX :
    {
        long *tmp;
        unsigned int stamp,unix_len,name_len,unix_uid,unix_gid;

        XDR$vton_uint(body++,&unix_len);
        tmp = body;
        body += (RNDUP(unix_len)/4);
        if (tmp==body) break;

        XDR$vton_uint(tmp++,&stamp);
        XDR$vton_uint(tmp++,&name_len);
        tmp += RNDUP(name_len)/4;
        XDR$vton_uint(tmp++,&uid);
        XDR$vton_uint(tmp++,&gid);
        break;
    }

    case AUTH_SHORT :
    {
        body += (RNDUP(xdr_int(*body))/4) + 1;
        break;
    }

    default :
    {
        XDR$vton_int(&msg_denied,&out->rm_reply.rp_stat);
        XDR$vton_int(&auth_error,&out->rm_reply.rp_rjct.rj_stat);
        XDR$vton_int(&auth_badverf,&out->rm_reply.rp_rjct.rj_why);
        *len += 12;
        return(1);
    }
    }

    for (checkcache = 0; checkcache < RPC_MAX_REPLY_CACHE; checkcache++)
        /*
         * Add test to qualify the xid based on the port index.  It looks
         * like the Ultrix client has a bug where it passes the same xid
         * for two different port values.  This causes us to reply with
         * a meaningless answer to the second request.
         *
         * Tom Allebrandi (Allebrandi@Inland.Com) 11-Oct-1991
         */
        if ((oldreplies[checkcache].xid == xid) &&
                (oldreplies[checkcache].index == index) &&
                (!strcmp(oldreplies[checkcache].hname,hname)))
        {
            *len = oldreplies[checkcache].len;
            memcpy(out, oldreplies[checkcache].out, *len);
            RC = oldreplies[checkcache].RC;
            return(RC);
        }

    if (!(username = map_uic(&uic,uid,gid,hname)))
    {
        struct _opcdef OPC_buffer;
        struct dsc$descriptor OPC_descript;

        if (authmechanism == AUTH_UNIX)
        {
            /* Send an operator message telling them of the problem */
            OPC_buffer.opc$b_ms_type = OPC$_RQ_RQST;
            *((int *)&OPC_buffer.opc$b_ms_target) = OPC$M_NM_OPER12;
            sprintf(&OPC_buffer.opc$l_ms_text,
                    "RPC: User %d (group %d) from host %s (%d\.%d\.%d\.%d) failed \
authentication",
                    uid, gid, hname, SrcAddr & 0xff,
                    ((SrcAddr & 0x0000ff00) / 0x00000100),
                    ((SrcAddr & 0x00ff0000) / 0x00010000), SrcAddr / 0x01000000);
            OPC_descript.dsc$w_length =
                strlen(&OPC_buffer.opc$l_ms_text) +
                ((int)&OPC_buffer.opc$l_ms_text -
                 (int)&OPC_buffer);
            OPC_descript.dsc$b_class = DSC$K_CLASS_S;
            OPC_descript.dsc$b_dtype = DSC$K_DTYPE_T;
            OPC_descript.dsc$a_pointer = &OPC_buffer;
            RC = SYS$SNDOPR(&OPC_descript, 0);
        }

        uic = -2;	/* For security - set to unused value */
    }

    /* ok, we've accepted it. */
    XDR$vton_int(&msg_accepted,&out->rm_reply.rp_stat);
    *len += 4;
    RC = (*RPCsrv_tab[index].dispatch_routine)(
             uic,
             hname,
             username,
             call_body,
             &out->rm_reply.rp_acpt,&result_len,
             call_body->cb_prog,
             call_body->cb_vers,
             call_body->cb_proc
         );
    *len += result_len;

    nextcache = (nextcache + 1) % RPC_MAX_REPLY_CACHE;
    if (oldreplies[nextcache].len != 0)
    {
        free(oldreplies[nextcache].out);
    }

    oldreplies[nextcache].xid = xid;
    oldreplies[nextcache].len = *len;
    oldreplies[nextcache].index = index;
    strcpy(oldreplies[nextcache].hname, hname);
    oldreplies[nextcache].out = (struct rpc_msg *)malloc(*len);
    memcpy(oldreplies[nextcache].out, out, *len);
    oldreplies[nextcache].RC = RC;

    return(RC);
}



/****************************************************************************

		PortMap - RPC port mapper program (#1000000)
Facility:

	ProtMap.C - Provide port mapping service under UDP (RFC 1057)

Abstract:

	Supports the Port Mapper protocol (PMAP) for the IPACP.  Provides
	RPC program #100000.

Author:

	Bruce R. Miller, CMU NetDev, 12-Nov-1990
	Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

 ***************************************************************************/

/* Port Map specific definitions */

#define NPROCS			 6
#define PMAP_VERSION		 2
#define PMAP_VERSION_LOW	 2
#define PMAP_VERSION_HIGH	 2
#define NAUTHPROCS		 2
#define PCNFSD_VERSION		 1
#define PCNFSD_VERSION_LOW	 1
#define PCNFSD_VERSION_HIGH      1

static const int pmap_version_low = PMAP_VERSION_LOW;
static const int pmap_version_high = PMAP_VERSION_HIGH;
static const int pcnfsd_version_low = PCNFSD_VERSION_LOW;
static const int pcnfsd_version_high = PCNFSD_VERSION_HIGH;

int ((*(pmap_proc_vector[NPROCS]))());
int ((*(pcnfsd_proc_vector[NAUTHPROCS]))());



/* NB:  At this point the opaque_auth structs come into play, since
   they are variable length structs, we declare cbody and areply as
   long pointers
*/

int PMAP$DISPATCH(uic,hname,username,cbody,areply,len,prog,vers,proc)
unsigned uic;
char *hname;
char *username;
long *cbody,*areply;
int *len;
unsigned int prog,vers,proc;
{
    long flavor;
    int result_len;

    /* fill in the authorization stuff */
    XDR$vton_int(&auth_none,areply++);
    static const int null = 0;
    XDR$vton_int(&null,areply++);
    *len = 2*BYTES_PER_XDR_UNIT;

    /* Is the program available? */
    if (prog != RPCPROG_PMAP)
    {
        XDR$vton_int(&prog_unavail,areply++);
        *len += 4;
        return(1);
    }

    /* is the procedure number reasonable? */
    if ((proc >= NPROCS) || (proc <0))
    {
        XDR$vton_int(&proc_unavail,areply++);
        *len += 4;
        return(1);
    }

    /* Is he requesting the right version? */
    /* !!!HACK!!! should this check be a bound instead of inequality? */
    if (vers != PMAP_VERSION)
    {
        XDR$vton_int(&prog_mismatch,areply++);
        *len += 4;
        XDR$vton_int(&pmap_version_low,areply++);
        *len += 4;
        XDR$vton_int(&pmap_version_high,areply++);
        *len += 4;
        return(1);
    }

    /* We're good to go */
    cbody += 4;		/* jump over rpc_vers,prog,vers, and proc */

    /* ignore credentials */
    flavor = xdr_int(*cbody++);
    cbody += ((RNDUP(xdr_int(*cbody))/4) +1);

    /* ignore verifier */
    flavor = xdr_int(*cbody++);
    cbody += ((RNDUP(xdr_int(*cbody))/4) +1);

    result_len = (*pmap_proc_vector[proc])(areply+1,cbody);
    if (result_len < 0)
    {
        XDR$vton_int(&garbage_args,areply++);
        *len += 4;
        return(1);
    }

    /* it worked! */
    XDR$vton_int(&success,areply++);
    *len += 4;
    *len += result_len;
    return 1;
}



/**********************************************************************

PMAP procedure #0	- Do nothing

*/

int PMAPPROC_NULL(reply)
long *reply;
{
    return 0;
}



/**********************************************************************

PMAP procedure #1	-

*/

int PMAPPROC_SET(reply,map)
long *reply;
struct mapping *map;
{
    return 0;
}



/**********************************************************************

PMAP procedure #2

*/

int PMAPPROC_UNSET(reply,map)
long *reply;
struct mapping *map;
{
    return 0;
}



/**********************************************************************

PMAP procedure #3	-

*/

int PMAPPROC_GETPORT(reply,map)
long *reply;
struct mapping *map;
{
    int i;

    XDR$ntov_uint(&map->prog,&map->prog);
    XDR$ntov_uint(&map->vers,&map->vers);
    XDR$ntov_uint(&map->prot,&map->prot);

    for (i=0; i<RPCsrv_count; i++)
    {
        if ((RPCsrv_tab[i].prog == map->prog) &&
                (RPCsrv_tab[i].vers == map->vers) &&
                (RPCsrv_tab[i].prot == map->prot))
            break;
    }
    if (i>=RPCsrv_count) return -1;	/* not found */

    XDR$vton_uint(&RPCsrv_tab[i].port,reply);
    return 4;	/* size of an int */
}



/**********************************************************************

PMAP procedure #4	- Dump a list of known RPC services

*/

int PMAPPROC_DUMP(reply)
long *reply;
{
    int i;

    for (i=0; i<RPCsrv_count; i++)
    {
        /* optional data, a one (TRUE) means there is data */
        static const int one = 1;
        XDR$vton_uint(&one,reply++);

        /* write out a mappping */
        XDR$vton_uint(&RPCsrv_tab[i].prog,reply++);
        XDR$vton_uint(&RPCsrv_tab[i].vers,reply++);
        XDR$vton_uint(&RPCsrv_tab[i].prot,reply++);
        XDR$vton_uint(&RPCsrv_tab[i].port,reply++);
    }
    /* end the list with a 0 */
    static const int zero = 0;
    XDR$vton_uint(&zero,reply++);

    return(RPCsrv_count*5*4 + 4);	/* size of an int */
}



/**********************************************************************

PMAP procedure #5

*/
int PMAPPROC_CALLIT(reply,call_args)
long *reply;
struct call_args *call_args;
{
    return 0;
}



/* Initialize the module.  This should be an entry point. */
int PMAP$INIT()
{
    pmap_proc_vector[0] = PMAPPROC_NULL;
    pmap_proc_vector[1] = PMAPPROC_SET;
    pmap_proc_vector[2] = PMAPPROC_UNSET;
    pmap_proc_vector[3] = PMAPPROC_GETPORT;
    pmap_proc_vector[4] = PMAPPROC_DUMP;
    pmap_proc_vector[5] = PMAPPROC_CALLIT;

    return(PMAP$DISPATCH);
}

enum arstat
{
    AUTH_RES_OK, AUTH_RES_FAKE, AUTH_RES_FAIL
};

typedef struct
{
    unsigned length;
    unsigned data[];
} stringdata;

struct auth_args
{
    stringdata aa_ident;
    stringdata aa_password;
};

struct auth_results
{
    enum arstat ar_stat;
    long        ar_uid;
    long        ar_gid;
};

#define zchar 0x5b

scramble(char *s1, char *s2)
{
    while (*s1)
    {
        *s2++ = (*s1 ^ zchar) & 0x7f;
        s1++;
    }
    *s2 = 0;
}


/* Initialize PCNFSD...At least we'll try... */
int PCNFSD$INIT()
{
    pcnfsd_proc_vector[0] = PCNFSDPROC_NULL;
    pcnfsd_proc_vector[1] = PCNFSDPROC_AUTH;

    return(PCNFSD$DISPATCH);
}

int PCNFSD$DISPATCH(uic, hname, username, cbody, areply, len, prog, vers, proc)
unsigned uic;
char *hname;
char *username;
long *cbody, *areply;
int *len;
unsigned int prog, vers, proc;

{
    int flavor, result_len;
    XDR$vton_int(&auth_none, areply++);
    static const int zero = 0;
    XDR$vton_int(&zero, areply++);
    *len = 2*BYTES_PER_XDR_UNIT;

    /* Is the program available? */
    if (prog != RPCPROG_PCNFSD)
    {
        XDR$vton_int(&prog_unavail, areply++);
        *len += 4;
        return(1);
    }

    /* Is the procedure number reasonable? */
    if ((proc >= NAUTHPROCS) || (proc < 0))
    {
        XDR$vton_int(&proc_unavail, areply++);
        *len += 4;
        return(1);
    }

    /* Is he requesting the right version? */
    if ((vers < PCNFSD_VERSION_LOW) || (vers > PCNFSD_VERSION_HIGH))
    {
        XDR$vton_int(&prog_mismatch, areply++);
        *len += 4;
        XDR$vton_int(&pcnfsd_version_low, areply++);
        *len += 4;
        XDR$vton_int(&pcnfsd_version_high, areply++);
        *len += 4;
        return(1);
    }

    cbody += 4;

    flavor = xdr_int(*cbody++);
    cbody += ((RNDUP(xdr_int(*cbody))/4)+1);

    flavor = xdr_int(*cbody++);
    cbody += ((RNDUP(xdr_int(*cbody))/4)+1);

    result_len = (*pcnfsd_proc_vector[proc])(areply+1, cbody);
    if (result_len < 0)
    {
        XDR$vton_int(&garbage_args, areply++);
        *len += 4;
        return(1);
    }

    XDR$vton_int(&success, areply++);
    *len += 4;
    *len += result_len;
    return 1;
}

int PCNFSDPROC_NULL(long *reply)
{
    return 0;
}

int PCNFSDPROC_AUTH(long *reply, struct auth_args *a)
{
    static struct auth_results r;
    char username[32];
    char password[64];
    int c1, c2, tmplen;

    XDR$ntov_arb(&a, &tmplen, sizeof(tmplen));
    XDR$ntov_arb(&a, &username, nfs_int(tmplen));
    username[nfs_int(tmplen)] = 0;
    XDR$ntov_arb(&a, &tmplen, sizeof(tmplen));
    XDR$ntov_arb(&a, &password, nfs_int(tmplen));
    password[nfs_int(tmplen)] = 0;

    r.ar_stat = AUTH_RES_FAIL;
    scramble(username, username);
    scramble(password, password);

    static const int auth_res_fail = AUTH_RES_FAIL;
    XDR$vton_uint(&auth_res_fail, reply++);
    static const int zero = 0;
    XDR$vton_uint(&zero, reply++);
    XDR$vton_uint(&zero, reply++);

    return(3*4);
}
