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

		Mount - RPC Disk Mounter program (#1000005)
Facility:

	mount.C - Provide remote mounting service under UDP (RFC 1097)

Abstract:

	Supports the MOUNT protocol (MNT) for the IPACP.  Provides
	RPC program #100005.

Author:

	Bruce R. Miller, CMU NetDev, 12-Nov-1990
	Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

 ***************************************************************************/

#module Mount
#pragma builtins

typedef unsigned int u_int;

#include stdio
#include stdlib
#include ssdef
#include ctype
#include descrip

#include in			/* network defs (defines u_long!) */

#include "rpc_types.h"		/* usefull and relavant definitions */
#include "xdr.h"
#include "auth.h"		/* authorization structs */
#include "rpc_msg.h"		/* protocol for rpc messages */
#include "nfs.h"

#include <netdevices.h>         /* CMU-OpenVMS/IP interface */
#include <netconfig.h>		/* CMU-OpenVMS/IP interface */

/* File related includes */

#include <rms.h>
#include <fibdef.h>
#include <atrdef.h>
#include <iodef.h>

extern int nfs_int();


    /* The IPACP_Interface tells us all about the IPACP.  It gives us   */
    /* entry points, literals and global pointers.  See NETDEVICES.REQ  */
    /* for a complete explaination of this structure.                   */
    /* Note: This pointer must be named "IPACP_Interface"               */
    IPACP_Info_Structure *IPACP_Interface;




/* Mount specific definitions */

#define RPCPROG_MOUNT		100005

#define NPROCS			 6
#define MNT_VERSION		 1
#define MNT_VERSION_LOW	 	 1
#define MNT_VERSION_HIGH	 1

#define MNTPATHLEN		  1024
#define MNTNAMLEN		  80
#define FHSIZE			  32

/* Basic data types */

typedef struct {
	unsigned long length;
	char data[MNTPATHLEN];
	} dirpath;

typedef struct {
	unsigned long length;
	char data[MNTNAMLEN];
	} name;

typedef struct {
    struct mount_record	*next,*prev;
    char	*hostname, *rempath;
    fhandle	hand;
    } mount_record;

struct { mount_record *head,*tail; } mount_queue;

/* Mount specific declarations */

int ((*(mnt_proc_vector[NPROCS]))());

int mnt_int(int x) { int y; XDR$ntov_int(&x,&y); return(y); }



convert_filename_to_fid(fname,fid)
    char *fname;
    fhandle fid;
{
    enum nfs_stat status;
    struct FAB fab;
    struct NAM nam;
    char rsbuff[NAM$C_MAXRSS],esbuff[NAM$C_MAXRSS];

    /* access the file */
    fab = cc$rms_fab;
    fab.fab$b_shr = FAB$M_UPI;			/* allow multiple access */
    fab.fab$b_fac = FAB$M_GET | FAB$M_PUT;	/* whatever...           */
    fab.fab$l_fna = fname;
    fab.fab$b_fns = strlen(fname);
    
    nam = cc$rms_nam;
    nam.nam$l_rsa = rsbuff;
    nam.nam$l_esa = esbuff;
    nam.nam$b_ess = nam.nam$b_rss = NAM$C_MAXRSS;
    fab.fab$l_nam = &nam;

    status = sys$open (&fab, 0, 0);
    if (status != RMS$_NORMAL)
	return(NFSERR_ACCES);

    esbuff[nam.nam$b_esl] = 0;

    memset(fid,0,FHSIZE);    
    memcpy(fid,&nam.nam$t_dvi,FIDSIZE);

    status = sys$close (&fab, 0, 0);
    if (status != RMS$_NORMAL)
	return(NFSERR_ACCES);

   return(NFS_OK);
}




/* NB:  At this point the opaque_auth structs come into play, since
   they are variable length structs, we declare cbody and areply as
   long pointers
*/

int MNT$DISPATCH(uic,hname,username,cbody,areply,len,prog,vers,proc)
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
    XDR$vton_int(&AUTH_NONE,areply++);
    XDR$vton_int(&0,areply++);
    *len = 2*BYTES_PER_XDR_UNIT;

    /* Is the program available? */
    if (prog != RPCPROG_MOUNT) {
	XDR$vton_int(&PROG_UNAVAIL,areply++);		*len += 4;
	return(1);
	}

    /* is the procedure number reasonable? */
    if ((proc >= NPROCS) || (proc <0)) {
	XDR$vton_int(&PROC_UNAVAIL,areply++);		*len += 4;
	return(1);
	}

    /* Is he requesting the right version? */
    /* !!!HACK!!! should this check be a bound instead of inequality? */
    if (vers != MNT_VERSION) {
	XDR$vton_int(&PROG_MISMATCH,areply++);		*len += 4;
	XDR$vton_int(&MNT_VERSION_LOW,areply++);	*len += 4;
	XDR$vton_int(&MNT_VERSION_HIGH,areply++);	*len += 4;
	return(1);
	}

/* We're good to go */
    cbody += 4;		/* jump over rpc_vers,prog,vers, and proc */

    /* ignore credentials */
    flavor = mnt_int(*cbody++);
    cbody += ((RNDUP(mnt_int(*cbody))/4) +1);

    /* ignore verifier */
    flavor = mnt_int(*cbody++);
    cbody += ((RNDUP(mnt_int(*cbody))/4) +1);
    
    result_len = (*mnt_proc_vector[proc])(areply+1,cbody,hname);
    if (result_len < 0) {
	XDR$vton_int(&GARBAGE_ARGS,areply++);		*len += 4;
	return(1);
	}

    /* it worked! */
    XDR$vton_int(&SUCCESS,areply++);		*len += 4;
    *len += result_len;
    return 1;
}



mount_record *find_mr(str)
    char *str;
{
    mount_record *mrp;

    mrp = mount_queue.head;
    while (mrp != &mount_queue.head)
	if (!strcmp(&mrp->rempath,str)) return mrp;
	else mrp = mrp->next;

    return NULL;
}

insert_mr(mrp)
    mount_record *mrp;
{
    _INSQUE(mrp,&mount_queue.head);
}

remove_mr(mrp)
    mount_record *mrp;
{
    if (_REMQUE(mrp,&mrp) < 2) {
	free(mrp->rempath);
	free(mrp->hostname);
        free(mrp);
    }
}

mount_record *new_mr(rempath, vmspath, hostname)
    char *rempath;
    char *vmspath;
    char *hostname;
{
    int RC;
    mount_record *mrp;

    mrp = (mount_record *)malloc(sizeof(*mrp));
    if (!mrp)
        return NULL;
	
    RC = convert_filename_to_fid(vmspath,&mrp->hand);
    if (RC != NFS_OK)
	return NULL;

    /* fill out the mount record */
    mrp->rempath = (char *)malloc(strlen(rempath) + 1);
    strcpy(mrp->rempath, rempath);
    mrp->hostname = (char *)malloc(strlen(hostname) + 1);
    strcpy(mrp->hostname, hostname);

    return mrp;
}

mount_record *get_mr(rempath, vmspath, hostname)
    char *rempath;
    char *vmspath;
    char *hostname;
{
    mount_record *mrp;

    mrp = find_mr(rempath);
    if (mrp == NULL) {
	mrp = new_mr(rempath, vmspath, hostname);
	if ((mrp==NULL) || (!insert_mr(mrp))) return NULL;
	}

    return mrp;
}



/**********************************************************************

MNT procedure #0	- Do nothing

*/

int MNTPROC_NULL(reply)
    char *reply;
{
    return 0;
}



/**********************************************************************

MNT procedure #1	- Add Mount Entry

*/

int MNTPROC_MNT(reply,args,remhost)
    long *reply;
    char *args;
    char *remhost;
{
    dirpath dir;
    int i,size;
    enum nfs_stat status;
    fhandle fid;
    mount_record *mrp;
    char mountpath[MNTPATHLEN+1], vmspath[MNTPATHLEN+1], *convptr,
        *vmsptr;

    /* Load arguments into dir */
    XDR$ntov_arb(&args, &dir.length, sizeof(dir.length));
    XDR$ntov_arb(&args, &dir.data, nfs_int(dir.length));
    dir.data[nfs_int(dir.length)] = 0;

    /* produce C string */
    XDR$ntov_uint(&dir.length,&size);
    strcpy(mountpath, dir.data);
    
    if (check_export(mountpath, remhost))
    {
	strcpy(mountpath, dir.data);		    /* Clear out upcasing */

        convptr = strchr(mountpath + 1, '/');	    /* Skip leading `/' */
	if (!convptr)
	{
	    strcat(mountpath, "/000000");
	    convptr = strchr(mountpath + 1, '/');
	}
	strncpy(vmspath, mountpath + 1, (convptr - (mountpath + 1)));
	vmspath[(convptr - (mountpath + 1))] = 0;
	strcat(vmspath, ":[000000.");
	strtok(mountpath + 1, "/");	/* Skip the first element (device) */
	vmsptr = vmspath + strlen(vmspath);
	while (convptr = strtok(NULL, "/"))
	{
	    vmsptr += FNAME_NET_TO_VMS(convptr, strlen(convptr), vmsptr, 1);
        }
	*(vmsptr - 1) = 0;
	vmsptr = strrchr(vmspath, '.');
	if (vmsptr)
	    *vmsptr = ']';

	strcat(vmspath, ".DIR");
	mrp = get_mr(&dir.data, &vmspath, remhost);
	if (mrp==NULL) {
	    XDR$ntov_uint(&NFSERR_NOENT,reply);
	    return 4;
	    }

	/* return results */
	XDR$ntov_uint(&NFS_OK,reply++);
	memset(reply,0,FHSIZE);
	memcpy(reply,&mrp->hand,FIDSIZE);
	return 4+FHSIZE;
    }
    else {
	XDR$ntov_uint(&NFSERR_ACCES,reply++);
	return 4;
    }
}



/**********************************************************************

MNT procedure #2	- Return Mount Entries

*/
MNTPROC_DUMP(reply, args, remhost)
    char *reply;
    char *args;
    char *remhost;
{
    /* Simply dump the queue of mounted directories to the network... (isn't
       this a bit on the under-secure side)? */

    mount_record *curmrp = mount_queue.head;
    int len = 0, stringlen;

    while (curmrp != &mount_queue.head)
    {
	XDR$vton_uint(&1, reply);	/* Mark there as being some data */
	reply += 4;	len += 4;

	stringlen = strlen(curmrp->hostname);
        XDR$vton_uint(&stringlen, reply);
	reply += 4;	len += 4;
	memset(reply, 0, RNDUP(stringlen));
	memcpy(reply, curmrp->hostname, stringlen);
	reply += RNDUP(stringlen);
	len += RNDUP(stringlen);
	
	stringlen = strlen(curmrp->rempath);
	XDR$vton_uint(&stringlen, reply);
	reply += 4;	len += 4;
	memset(reply, 0, RNDUP(stringlen));
	memcpy(reply, curmrp->rempath, stringlen);
	reply += RNDUP(stringlen);
	len += RNDUP(stringlen);

	curmrp = curmrp->next;
    }
    
    XDR$vton_uint(&0, reply);		/* No more data available */
    reply += 4;	    len += 4;
    return len;
}



/**********************************************************************

MNT procedure #3	- Remove Mount Entry

*/
int MNTPROC_UMNT(reply,args,remhost)
    long *reply;
    char *args;
    char *remhost;
{
    dirpath dir;
    int i,size;
    enum nfs_stat status;
    mount_record *mrp;

    /* Load the argument into dir */
    XDR$ntov_arb(&args, &dir.length, sizeof(dir.length));
    XDR$ntov_arb(&args, &dir.data, nfs_int(dir.length));
    dir.data[nfs_int(dir.length)] = 0;

    /* find the mount record */
    mrp = find_mr(&dir.data);
    if (mrp==NULL)
	return 0;

    /* fill out the mount record */
    remove_mr(mrp);

    /* return results */
    return 0;
}



/**********************************************************************

MNT procedure #4	- Remove All Mount Entries (for this host)

*/
MNTPROC_UMNTALL(reply,args,remhost)
    char *reply;
    char *args;
    char *remhost;
{
    mount_record *nextmrp, *curmrp = mount_queue.head;

    while (curmrp != &mount_queue.head)
    {
	nextmrp = curmrp->next;

        if (!strcmp(remhost, curmrp->hostname))
	{
	    nextmrp = curmrp->next;
	    remove_mr(curmrp);
	}

	curmrp = nextmrp;
    }

    return 0;		/* All OK (we hope; at least we tried) */
}



/**********************************************************************

MNT procedure #5	- Return Export List

*/
MNTPROC_EXPORT(reply,args,remhost)
    char *reply;
    char *args;
    char *remhost;
{
    char exportlist[8192], *curexport, *curgroup;
    int len = 0, stringlen;

    RPC$get_exports(&exportlist);

    curexport = strtok(exportlist, ":");
    while (curexport)
    {
        XDR$vton_uint(&1, reply);
	reply += 4;	len += 4;
	stringlen = strlen(curexport);
	XDR$vton_uint(&stringlen, reply);
	reply += 4;	len += 4;
	memset(reply, 0, RNDUP(stringlen));
	memcpy(reply, curexport, stringlen);
	reply += RNDUP(stringlen);
	len += RNDUP(stringlen);

	curgroup = strtok(NULL, ":");
	while (curgroup && (stringlen = strlen(curgroup)) &&
	    strcmp(curgroup, " "))
	{
	    XDR$vton_uint(&1, reply);
	    reply += 4;	    len += 4;
	    XDR$vton_uint(&stringlen, reply);
	    reply += 4;	    len += 4;
	    memset(reply, 0, RNDUP(stringlen));
	    memcpy(reply, curgroup, stringlen);
	    reply += RNDUP(stringlen);
	    len += RNDUP(stringlen);

	    curgroup = strtok(NULL, ":");
	}
	XDR$vton_uint(&0, reply);
	reply += 4;	len += 4;
	curexport = strtok(NULL, ":");
    }
    XDR$vton_uint(&0, reply);
    reply += 4;	    len += 4;

    return len;
}



/* Initialize the module.  This should be an entry point. */
int MNT$INIT(IPACP_Int)
   char *IPACP_Int;
{
    int i;

    mnt_proc_vector[0] = MNTPROC_NULL;
    mnt_proc_vector[1] = MNTPROC_MNT;
    mnt_proc_vector[2] = MNTPROC_DUMP;
    mnt_proc_vector[3] = MNTPROC_UMNT;
    mnt_proc_vector[4] = MNTPROC_UMNTALL;
    mnt_proc_vector[5] = MNTPROC_EXPORT;

    mount_queue.tail = mount_queue.head = &mount_queue.head;

    return(MNT$DISPATCH);
}
