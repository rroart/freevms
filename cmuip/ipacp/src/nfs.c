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

		NFS - RPC Network File Service program (#1000003)
Facility:

	NFS.C - Provide Network File Service under UDP (RFC 1097)

Abstract:

	Supports the NFS protocol for the IPACP.  Provides
	RPC program #100003.

Notes:

	(Mention contruction of the file handles)


Author:

	Bruce R. Miller, CMU NetDev, 13-Nov-1990
	Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

    13-AUG-1991	    Marc A. Shannon	    CMU Group N		V6.6-2
	Thanks to Tom Allebrandi, a problem was fixed where variable
	length records (not IMPLIEDCC) such as .OBJ files would be accessed
	without the recordlength table causing the IPACP to ACCVIO to its
	death.

 ***************************************************************************/

#module NFS
#pragma builtins

typedef unsigned int u_int;

#include stdio
#include unixio
#include ssdef
#include ctype
#include descrip
#include syidef
#include jpidef
#include acldef
#include chpdef
#include armdef
#include errno
#include in			/* network defs (defines u_long!) */

#include "rpc_types.h"		/* usefull and relavant definitions */
#include "xdr.h"
#include "auth.h"		/* authorization structs */
#include "rpc_msg.h"		/* protocol for rpc messages */
#include "nfs.h"

#include <netdevices.h>		/* CMU-OpenVMS/IP interface */
#include <netconfig.h>		/* CMU-OpenVMS/IP interface */

/* File related includes */

#include rms
#include fibdef
#include atrdef
#include iodef
#include stat
#include dvidef
#include "fatdef.h"

#include file

#define MAX_FNAME 256

#define ERROR(x) (!((x)&1))
#define MIN(x,y)    (x < y ? x : y)

typedef unsigned short uword;

extern long GMT_OFFSET;
extern char *NFS_ANONYMOUS;

log_IO_error(int ret1, int ret2, char *oprtn)
{
    FILE *logtest;
    logtest = fopen("CMUIP_ROOT:[SYSMGR]NFSERR.LOG", "a");
    fprintf(logtest, "NFSERR_IO error (%8x or %8x) at %s\n",
	    ret1, ret2, oprtn);
    fclose(logtest);
}



    /* The IPACP_Interface tells us all about the IPACP.  It gives us   */
    /* entry points, literals and global pointers.  See NETDEVICES.REQ  */
    /* for a complete explaination of this structure.                   */
    /* Note: This pointer must be named "IPACP_Interface"               */
    IPACP_Info_Structure *IPACP_Interface;




/* NFS specific definitions */

#define RPCPROG_NFS		100003

#define NPROCS			18
#define NFS_VERSION		 2
#define NFS_VERSION_LOW	 	 2
#define NFS_VERSION_HIGH	 2

#define HTAB_SIZE		17

/* Block caching constants */

#define BLKSIZE 10240

int MAX_CACHE=50;	    /* Default value - changed during NFS$INIT */

/* Basic data types */

typedef struct {
    struct timer_record *next, *prev;
    char *item;
    int referenced;
    } timer_record;

typedef struct {
    unsigned long bytestart;
    unsigned short reclen;
} record_starts;

typedef struct {
    struct file_record *next,*prev;		/* links for cache */
    fhandle	hand;				/* internal NFS file ID */
    int		ref_count;			/* links to file */
    char	filename[MAX_FNAME];		/* file name */

    /* open file information */
    uword	fchan;				/* XQP channel */
    long int	filesize;			/* Unix file size */
    struct dsc$descriptor fib_desc;		/* Descriptor to FIB */
    struct fibdef fib;				/* File Information Block */
    struct atrdef atrblock[2];			/* Attributes pointer block */
    struct fatdef fat;				/* File attributes */

    record_starts *offsets;			/* Array of block offsets */

    timer_record *timerblock;			/* pointer to timer entry */

    } file_record;

int fr_entry_count = 0;

struct hash_entry {
    char *queue_head,*queue_tail;
    int count;
    } htab[HTAB_SIZE];

static struct {
    timer_record *head, *tail;
    } fifo_timer;

unsigned long one_minute_delta[2] = {0xdc3cba00, 0xffffffff};

/* NFS specific declarations */

int ((*(nfs_proc_vector[NPROCS]))());

int nfs_int(int x) { int y; XDR$ntov_int(&x,&y); return(y); }



/* Convert VMS error codes into NFS ones - should be easy, right? :-) */
int VMS_to_NFSERR(int vmscode)
{
    switch (vmscode)
    {
    case SS$_NORMAL:
    case RMS$_NORMAL:
    case RMS$_CONTROLO:
    case RMS$_CONTROLY:
    case RMS$_CREATED:
    case RMS$_SUPERSEDE:
    case RMS$_CONTROLC:
	return (NFS_OK);
	break;

    case RMS$_FNF:
    case RMS$_DNF:
	return (NFSERR_NOENT);

    case RMS$_DNR:
	return (NFSERR_NXIO);

    case RMS$_PRV:
    case SS$_NOPRIV:
	return (NFSERR_ACCES);

    case RMS$_FEX:
	return (NFSERR_EXIST);

    case SS$_NOSUCHDEV:
	return (NFSERR_NODEV);

    case SS$_BADIRECTORY:
	return (NFSERR_NOTDIR);

    case RMS$_FUL:
	return (NFSERR_NOSPC);

    case RMS$_SYN:
    case RMS$_TYP:
	return (NFSERR_NAMETOOLONG);

    case SS$_EXDISKQUOTA:
	return (NFSERR_DQUOT);

    default:
	return (NFSERR_IO);
    }
}


/* NB:  At this point the opaque_auth structs come into play, since
   they are variable length structs, we declare cbody and areply as
   long pointers
*/

int NFS$DISPATCH(uic,hname,username,cbody,areply,len,prog,vers,proc)
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
    if (prog != RPCPROG_NFS) {
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
    if (vers != NFS_VERSION) {
	XDR$vton_int(&PROG_MISMATCH,areply++);		*len += 4;
	XDR$vton_int(&NFS_VERSION_LOW,areply++);	*len += 4;
	XDR$vton_int(&NFS_VERSION_HIGH,areply++);	*len += 4;
	return(1);
	}

/* We're good to go */
    cbody += 4;		/* jump over rpc_vers,prog,vers, and proc */

    /* ignore credentials */
    flavor = nfs_int(*cbody++);
    cbody += ((RNDUP(nfs_int(*cbody))/4) +1);

    /* ignore verifier */
    flavor = nfs_int(*cbody++);
    cbody += ((RNDUP(nfs_int(*cbody))/4) +1);

    if (uic == -2)		/* No match was found in RPC */
    {
        XDR$vton_int(&NFSERR_ACCES, areply+1);
	result_len = 1;
    }
    else
	result_len = (*nfs_proc_vector[proc])(uic,hname,username,areply+1,cbody);
    if (result_len < 0) {
	XDR$vton_int(&GARBAGE_ARGS,areply++);		*len += 4;
	return(1);
	}

    /* Not realy the reply val, just first long word of results... */

    /* it worked! */
    XDR$vton_int(&SUCCESS,areply++);			*len += 4;
    *len += 4 * result_len;
    return 1;
}



/*************************************************************************

			Hash Table Routines

**************************************************************************/

int fr_key_comp(key1,key2)
    fhandle key1,key2;
{
    int i;

    for (i=0; i<FHSIZE; i++)
	if (key1[i] != key2[i]) return FALSE;

    return TRUE;
}

int hash_fr(key)
    fhandle key;
{
   int i,sum=0;

   for (i=0; i<FIDSIZE; i++)
	sum += key[i];
   return(sum%HTAB_SIZE);
}


file_record *find_fr(key)
    fhandle key;
{
   int index;
   file_record *frp;

   index = hash_fr(key);
   frp = htab[index].queue_head;
   while (frp != &htab[index].queue_head)
	if (fr_key_comp(key,&frp->hand)) {
	   frp->timerblock->referenced++;
	   return(frp);
	} else frp = frp->next;

   return NULL;
}

void delete_fr(file_record *frp)
{
   file_record *delfile;
   timer_record *deltimer;
   int index;
   int RC;

   RC = SYS$DASSGN(frp->fchan);
   if (!ERROR(RC))
      fr_entry_count--;

   if (_REMQUE(frp, &delfile) < 2) {
      free(delfile->offsets);
      if (_REMQUE(delfile->timerblock, &deltimer) < 2)
         free(deltimer);
      free(delfile);
   }
}

int insert_fr(frp)
   file_record *frp;
{
   int index;
   timer_record *new_timer, *old_timer;

   index = hash_fr(&frp->hand);
   _INSQUE(frp,&htab[index].queue_head);
   htab[index].count++;

   new_timer = (timer_record *)malloc(sizeof(timer_record));
   new_timer->next = new_timer->prev = NULL;
   new_timer->item = (char *)frp;
   new_timer->referenced = 2;		    /* Give it two minutes to start */
   _INSQUE(new_timer, &fifo_timer.head);

   frp->timerblock = new_timer;

   fr_entry_count++;

   if (fr_entry_count >= MAX_CACHE)
      delete_fr((file_record *)fifo_timer.tail->item);

   return TRUE;
}

int clean_frs()
{
/* Called via $SETIMR to periodically check the status of the open channels */

   timer_record *checkme, *next;
   int RC;

   checkme = fifo_timer.head;

   while (checkme != &fifo_timer.head) {
      if (checkme->referenced > 10)
         checkme->referenced = 10;
      next = checkme->next;
      if (--(checkme->referenced) < 0)
         delete_fr((file_record *)checkme->item);
      checkme = next;
   }

   RC = SYS$SETIMR(0, &one_minute_delta, clean_frs, 0, 0);
}


/*********************************************************************

		Cache Management Routines

**********************************************************************/

rms_to_handle(nam,hand)
    struct NAM *nam;
    fhandle hand;
{
    memset(hand,0,FHSIZE);
    memcpy(hand,&nam->nam$t_dvi,FIDSIZE);
}

handle_to_rms(hand,nam)
    fhandle hand;
    struct NAM *nam;
{
    memcpy(&nam->nam$t_dvi,hand,FIDSIZE);
}

file_record *new_fr(hand)
    fhandle hand;
{
    int RC;
    short len;
    file_record *frp;
    struct dsc$descriptor dev_desc,file_desc;
    unsigned short int QIOSB[4];
    int blockloop, numblocks, bytecount, curvms, curunix, bytestogo, reclen,
        nfsblock, numbytes, tindex;
    char blockbuffer[BLKSIZE], *blockptr;

    frp = (file_record *)malloc(sizeof(*frp));
    if (frp == NULL) return NULL;

    frp->fib_desc.dsc$w_length = sizeof(frp->fib);
    frp->fib_desc.dsc$b_dtype = frp->fib_desc.dsc$b_class = 0;
    frp->fib_desc.dsc$a_pointer = &frp->fib;
    memset(&frp->fib, 0, sizeof(frp->fib));

    dev_desc.dsc$w_length = hand[0];
    dev_desc.dsc$b_dtype = 0;
    dev_desc.dsc$b_class = 0;
    dev_desc.dsc$a_pointer = &hand[1];
    file_desc.dsc$w_length = sizeof(frp->filename);
    file_desc.dsc$b_dtype = 0;
    file_desc.dsc$b_class = 0;
    file_desc.dsc$a_pointer = &frp->filename;

    RC = LIB$FID_TO_NAME(&dev_desc,&hand[16],&file_desc,&len,0,0);
    if (ERROR(RC))
	return NULL;

    frp->filename[len] = 0;

    RC = SYS$ASSIGN(&dev_desc, &frp->fchan, 0, 0);
    if (ERROR(RC))
	return NULL;

    frp->atrblock[0].atr$w_size = sizeof(frp->fat);
    frp->atrblock[0].atr$w_type = ATR$C_RECATTR;
    frp->atrblock[0].atr$l_addr = &frp->fat;
    frp->atrblock[1].atr$w_size = frp->atrblock[1].atr$w_type =
       frp->atrblock[1].atr$l_addr = 0;

    frp->fib.fib$r_acctl_overlay.fib$l_acctl = FIB$M_WRITE | FIB$M_NOLOCK;
    memcpy(&frp->fib.fib$r_fid_overlay.fib$w_fid, &hand[16], 6);

    RC = SYS$QIOW(0, frp->fchan, IO$_ACCESS|IO$M_ACCESS, &QIOSB, 0, 0,
                  &frp->fib_desc, 0, 0, 0, &frp->atrblock, 0);
    if (ERROR(RC) || ERROR(QIOSB[0]))
	return NULL;

    frp->filesize = -1;

    /* Fix up the offsets array to indicate what the file looks like */
    frp->offsets = NULL;
    if (frp->fat.fat$b_rtype.fat$v_rtype == FAT$C_VARIABLE)
    {
	numblocks = (frp->fat.fat$l_efblk.fat$w_efblkl +
	    (frp->fat.fat$l_efblk.fat$w_efblkh * 65536));
        frp->offsets = (record_starts *)
	    malloc(sizeof(record_starts) * numblocks);

	curvms = 0;
	curunix = 0;
	bytestogo = 0;
	reclen = 0;
	nfsblock = 0;
	numbytes = (frp->fat.fat$l_efblk.fat$w_efblkl +
		    (frp->fat.fat$l_efblk.fat$w_efblkh * 65536) - 1) * 512 +
		   frp->fat.fat$w_ffbyte - 1;

	for (blockloop=0;
	     blockloop <= (numblocks / (BLKSIZE / 512));
	     blockloop++)
	{
	    /* Read as many bytes as the buffer will hold */
	    RC = SYS$QIOW(0, frp->fchan, IO$_READVBLK, &QIOSB, 0, 0,
	        &blockbuffer, BLKSIZE,
		blockloop * (BLKSIZE / 512) + 1, 0, 0, 0);
	    if (!ERROR(RC) &&
	        ((QIOSB[0] == SS$_ENDOFFILE) || !ERROR(QIOSB[0])))
	    {
		blockptr = &blockbuffer + (curvms - blockloop * BLKSIZE);

		/*
		 * Change to 65536 from 256 per mail conversation with
		 * Marc Shannon.
		 *
		 * Tom Allebrandi (Allebrandi@Inland.Com) 12-Jul-1991
		 */
		bytecount = QIOSB[1] + QIOSB[2]*65536;

		/* -1 is in case it ends one byte short of the data block */
		while (((blockptr - &blockbuffer) < bytecount - 1) &&
		       (curvms < numbytes - 1))
		{
		    if (!reclen) {
			if (curvms & 1)	    /* Increment to next whole word */
			{
			    curvms++;
			    blockptr++;
			}

			memcpy(&reclen, blockptr, 2);
			reclen &= 0xffff;
			/* Surprize! It seems that a record length of 0xFFFF
			 * is interpreted as "skip to end of block"!!!!!  I
			 * wonder what other demons live near here!
			 *
			 * Tom Allebrandi (Allebrandi@Inland.Com) 3-Sep-1991
			 */
			if (reclen == 0xFFFF)
			{
			    /* Round the block pointer up to the next block
			     * and skip the rest of the loop. (It is tempting
			     * to simply round up `blockptr'. That doesn't
			     * work because the `blockbuffer' is not page
			     * aligned.
			     */
			    tindex = blockptr - blockbuffer;
			    tindex = tindex - (tindex % 512) + 512;
			    blockptr = &blockbuffer[tindex];
			    curvms = curvms - (curvms % 512) + 512;
			    reclen = 0;
			    continue;
			}
			
			blockptr += 2;
			curvms += 2;
			curunix += reclen +
			    (frp->fat.fat$b_rattrib & FAT$M_IMPLIEDCC ? 1 : 0);
		    }

		    while (reclen + 1 > bytestogo)
		    {
			reclen -= bytestogo;
			curvms += bytestogo;
			blockptr += bytestogo;

			frp->offsets[nfsblock].reclen = reclen;
			frp->offsets[nfsblock++].bytestart = curvms;

			bytestogo = 512;
		    }

		    curvms += reclen;
		    bytestogo -= reclen + 1;    /* Add one for \n */

		    blockptr += reclen;
		    reclen = 0;
		}
	    }
	}
	frp->filesize = curunix;
    }

    /* fill in the key */
    memset(&frp->hand,0,FHSIZE);
    memcpy(&frp->hand,hand,FIDSIZE);

    return(frp);
}


file_record *get_fr(hand)
    fhandle hand;
{
   file_record *frp;
   timer_record *movetohead;

   frp = find_fr(hand);
   if (frp == NULL) {
	frp = new_fr(hand);
	if ((frp==NULL) || (!insert_fr(frp))) return(NULL);
	}

/*
 * Check to make sure that the file referenced by the file record
 * exists. If it doesn't, the handle is stale and should be deleted.
 * If we return a valid file record here, downstream is likely to fail
 * for other reasons causing the client to not really know that his
 * handle is no longer valid.
 *
 * Tom Allebrandi (Allebrandi@Inland.Com) 8-Jul-1991
 */
   else if (access(frp->filename,0) == 0) {
	_REMQUE(fifo_timer.tail, &movetohead);
	_INSQUE(movetohead, &fifo_timer.head);
	}
   else {
	delete_fr(frp);
	frp = NULL;
	}

   return frp;
}



/**********************************************************************

			File Access Routines

***********************************************************************/



char *updir(char *path)
{
    /* updir takes the path as an argument and returns its address as its
       value after modifying the string such that the last directory name
       specified becomes the parent's directory and the old directory as the
       file in the parent directory. */
    char *dot, *leftbracket, *rightbracket;

    rightbracket = strrchr(path, ']');
    if (rightbracket == NULL)
        return NULL;

    *rightbracket = 0;
    dot = strrchr(path, '.');
    if (dot == NULL)
    {
        leftbracket = strrchr(path, '[');
	strcpy(leftbracket + 8, leftbracket + 1);   /* Move directory name to
						       filename position */
	strncpy(leftbracket, "[000000]", 8);	    /* Don't null-terminate */
    } else
	*dot = ']';
    strcat(path, ".DIR");
    return path;
}



make_handle(fname,fid)
    char *fname;
    fhandle fid;
{
    int status;
    struct FAB fab;
    struct NAM nam;
    char esbuff[NAM$C_MAXRSS],rsbuff[NAM$C_MAXRSS];

    /* access the file */
    fab = cc$rms_fab;
    fab.fab$l_fna = fname;
    fab.fab$b_fns = strlen(fname);
    fab.fab$l_nam = &nam;

    nam = cc$rms_nam;
    nam.nam$l_rsa = rsbuff;
    nam.nam$l_esa = esbuff;
    nam.nam$b_ess = nam.nam$b_rss = NAM$C_MAXRSS;

    status = sys$parse (&fab, 0, 0);
    if (status != RMS$_NORMAL)
	return(VMS_to_NFSERR(status));

    status = sys$search (&fab, 0, 0);
    if (status != RMS$_NORMAL)
	return(VMS_to_NFSERR(status));

    rms_to_handle(&nam,fid);

    return(NFS_OK);
}


int get_file_attributes(frp,attribs,hostname)
    file_record *frp;
    fattr *attribs;
    char *hostname;
{
    int RC;
    stat_t stats;
    unsigned int uic;
    short uid,gid;

    RC = stat(&frp->filename,&stats);

    if ((stats.st_mode & S_IFMT) == S_IFDIR) attribs->type = NFDIR;
    else attribs->type = NFREG;

    attribs->mode	= stats.st_mode;
    attribs->nlink	= stats.st_nlink;
    attribs->nlink	= 1;

    uid = (short)stats.st_uid;  gid = (short)stats.st_gid;
    uic = uid + (gid<<16);
    if (!map_unix(uic,&attribs->uid,&attribs->gid,hostname)) {
	attribs->uid = -2;
	attribs->gid = -2;
	}
    attribs->uid &= 0x0000ffff;
    attribs->gid &= 0x0000ffff;

    attribs->size	= (frp->filesize != -1) ? frp->filesize : stats.st_size;
    attribs->blocksize	= 512;
    attribs->rdev	= stats.st_rdev;

    attribs->blocks = (frp->fat.fat$l_hiblk.fat$w_hiblkh * 65536) +
                      (frp->fat.fat$l_hiblk.fat$w_hiblkl);
    attribs->fsid	= stats.st_dev;
    attribs->fileid	= (stats.st_ino[0] * 65536) + stats.st_ino[1];

    attribs->atime.seconds	= stats.st_atime - GMT_OFFSET;
    attribs->atime.useconds	= 0;
    attribs->mtime.seconds	= stats.st_mtime - GMT_OFFSET;
    attribs->mtime.useconds	= 0;
    attribs->ctime.seconds	= stats.st_ctime - GMT_OFFSET;
    attribs->ctime.useconds	= 0;

    return 1;
}

int set_file_attributes(frp,sattribs,hostname)
    file_record *frp;
    sattr *sattribs;
    char *hostname;
{
    int RC;
    unsigned short int QIOSB[4];
    char filename[255];
    unsigned long newuic;
    stat_t curstats;

    stat(&frp->filename, &curstats);

    /* We don't care about group information */
    if ((short)nfs_int(sattribs->uid) != -1)
	if (map_uic(&newuic, nfs_int(sattribs->uid), nfs_int(sattribs->gid),
	    hostname))
	    chown(&frp->filename, newuic & 0xffff, newuic>>16);
	else
	    return NFSERR_ACCES;

    if (nfs_int(sattribs->size) != -1) {
       frp->fib.fib$r_exctl_overlay.fib$w_exctl = FIB$M_TRUNC;
       frp->fib.fib$l_exsz = 0;
       frp->fib.fib$l_exvbn = (nfs_int(sattribs->size) + 511) / 512 + 1;
       frp->fat.fat$l_efblk.fat$w_efblkh =
	    (((nfs_int(sattribs->size) / 512) + 1) / 65536);
       frp->fat.fat$l_efblk.fat$w_efblkl =
	    (((nfs_int(sattribs->size) / 512) + 1) % 65536);
       frp->fat.fat$w_ffbyte = (nfs_int(sattribs->size) % 512);
       frp->filesize = nfs_int(sattribs->size);

       if (!nfs_int(sattribs->size))
           frp->fat.fat$b_rtype.fat$v_rtype = FAT$C_STREAMLF;

       RC = SYS$QIOW(0, frp->fchan, IO$_MODIFY, &QIOSB, 0, 0,
                     &frp->fib_desc, 0, 0, 0, &frp->atrblock, 0);
       if (ERROR(RC) || ERROR(QIOSB[0]))
          return NFSERR_IO;
    }

    if ((short)nfs_int(sattribs->mode) != -1)
        chmod(&frp->filename, nfs_int(sattribs->mode));

/* !!!HACK!!! sattribs->?time : how to change file times??? */

    return NFS_OK;

}

int has_access(char *username, char *hname, char *filename, unsigned access)
{
    struct dsc$descriptor objnam, usrnam;
    struct {
	unsigned short buflen, itmcod;
	unsigned long bufadr, retlenadr, null;
    } itmlst;
    unsigned long vmsuic;
    stat_t filestats;
    int RC;

    if (stat(filename, &filestats) == -1)
	return NFSERR_NOENT;

    objnam.dsc$b_dtype = usrnam.dsc$b_dtype = DSC$K_DTYPE_T;
    objnam.dsc$b_class = usrnam.dsc$b_class = DSC$K_CLASS_S;
    objnam.dsc$w_length = strlen(filename);
    objnam.dsc$a_pointer = filename;

    if (!strcmp(username, "*"))
    {
        usrnam.dsc$a_pointer = NFS_ANONYMOUS;
	usrnam.dsc$w_length = strlen(NFS_ANONYMOUS);
    }
    else {
        usrnam.dsc$w_length = strlen(username);
	usrnam.dsc$a_pointer = username;
    }

    itmlst.buflen = sizeof(access);
    itmlst.itmcod = CHP$_ACCESS;
    itmlst.bufadr = &access;
    itmlst.retlenadr = itmlst.null = 0;

    RC = SYS$CHECK_ACCESS(&ACL$C_FILE, &objnam, &usrnam, &itmlst);

    /*
     * Special case: SYS$CHECK_ACCESS() returns "record not found" (RMS$_RNF)
     * if the user name is invalid. It seems wrong to do a general mapping
     * of this error code since it really doesn't mean a file problem in this
     * context. Trap it here.
     *
     * Tom Allebrandi (Allebrandi@Inland.Com) 8-Jul-1991
     */
    if (RC == RMS$_RNF)
	return NFSERR_ACCES;
    else
	return (VMS_to_NFSERR(RC));
}

/*******************************************************************

			XDR output routines

	N.B.  All these routines return sizes in terms of longs.

********************************************************************/

int write_XDR_stat(stat,buf)
    long stat;
    long **buf;
{
    XDR$vton_int(&stat,(*buf)++);
    return 1;
}

int write_XDR_fhandle(hand,buf)
    fhandle hand;
    long **buf;
{
    memcpy(*buf,hand,FHSIZE);
    *buf += FHSIZE/4;

    return FHSIZE/4;
}



int write_XDR_file_attributes(attribs,buf)
    fattr *attribs;
    long **buf;
{
    long *buff_start = *buf;
    XDR$vton_int(&attribs->type,(*buf)++);
    XDR$vton_uint(&attribs->mode,(*buf)++);
    XDR$vton_uint(&attribs->nlink,(*buf)++);
    XDR$vton_uint(&attribs->uid,(*buf)++);
    XDR$vton_uint(&attribs->gid,(*buf)++);
    XDR$vton_uint(&attribs->size,(*buf)++);
    XDR$vton_uint(&attribs->blocksize,(*buf)++);
    XDR$vton_uint(&attribs->rdev,(*buf)++);
    XDR$vton_uint(&attribs->blocks,(*buf)++);
    XDR$vton_uint(&attribs->fsid,(*buf)++);
    XDR$vton_uint(&attribs->fileid,(*buf)++);

    /* times */
    XDR$vton_uint(&attribs->atime.seconds,(*buf)++);
    XDR$vton_uint(&attribs->atime.useconds,(*buf)++);
    XDR$vton_uint(&attribs->mtime.seconds,(*buf)++);
    XDR$vton_uint(&attribs->mtime.useconds,(*buf)++);
    XDR$vton_uint(&attribs->ctime.seconds,(*buf)++);
    XDR$vton_uint(&attribs->ctime.useconds,(*buf)++);

    return (*buf - buff_start);

/*    return RNDUP(sizeof(*attribs))/4; */
}



file_to_dir(s)
    char *s;
{
    /* convert [foo]bar.dir into [foo.bar] */
    while (*s && (*s != ']')) s++;
    if (!*s) return 0; else *s++ = '.';
    while (*s && (*s != '.')) s++;
    if (!*s) return 0;
    *s++ = ']'; *s = 0;
}

file_record *build_fname(dargs,fname,dirp)
    diropargs *dargs;
    char *fname;
    int dirp;
{
    file_record *drp;
    int fnlen;
    char *s,newname[MAXNAMLEN+1];
    char *endofname;
    stat_t filestat;

    /* find_fr the directory */
    drp = get_fr(dargs->dir);
    if (drp == NULL)
	return NULL;

    /* convert [foo]bar.dir into [foo.bar] */
    fnlen = nfs_int(dargs->name.length); dargs->name.data[fnlen] = 0;
    strcpy(fname,&drp->filename); s = file_to_dir(fname);

    /* convert remote name to local name */
    /* Try directory specification first */
    FNAME_NET_TO_VMS(&dargs->name.data, nfs_int(dargs->name.length),
        newname, 1);
    strcpy(s, newname);
    strcat(s, "DIR");
    if (!dirp && ((stat(fname, &filestat) == -1) ||
        (filestat.st_mode & S_IFDIR == 0)))
    {
        FNAME_NET_TO_VMS(&dargs->name.data, nfs_int(dargs->name.length),
	    newname, 0);
	    strcpy(s, newname);
    }

    return drp;
}



/**********************************************************************

NFS procedure #0	- Do Nothing

*/

int NFSPROC_NULL(uic,hname,username,reply)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
{
    return 0;
}



/**********************************************************************

NFS procedure #1	- Get File Attributes

*/

int NFSPROC_GETATTR(uic,hname,username,reply,hand)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    fhandle hand;
{
    int RC,len;
    file_record *frp;
    fattr attribs;

    /* find_fr the file */
    frp = get_fr(hand);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    if (!(RC = has_access(username, hname, frp->filename, ARM$M_READ)))
    {
        if (!get_file_attributes(frp, &attribs, hname))
	    return write_XDR_stat(NFSERR_ACCES, &reply);
	write_XDR_stat(NFS_OK, &reply);
	return 1 + write_XDR_file_attributes(&attribs, &reply);
    }
    return write_XDR_stat(RC, &reply);
}



/**********************************************************************

NFS procedure #2	- Set File Attributes

*/

int NFSPROC_SETATTR(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    int RC,len;
    file_record *frp;
    fattr attribs;
    struct {
	fhandle file;
	sattr attribs;
	} sat_args;

    /* Load the arguments */
    XDR$ntov_arb(&args, &sat_args.file, sizeof(sat_args.file));
    XDR$ntov_arb(&args, &sat_args.attribs, sizeof(sat_args.attribs));

    /* find_fr the file */
    frp = get_fr(&sat_args.file);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    /* Check access... */
    if (RC = has_access(username, hname, frp->filename, ARM$M_WRITE))
        return write_XDR_stat(RC, &reply);

    RC = set_file_attributes(frp,&sat_args.attribs,hname);
    if (RC != NFS_OK)
        return write_XDR_stat(RC, &reply);

    if (!get_file_attributes(frp,&attribs, hname))
	return write_XDR_stat(NFSERR_ACCES,&reply);

    write_XDR_stat(NFS_OK,&reply);
    return 1 + write_XDR_file_attributes(&attribs,&reply);
}



/**********************************************************************

NFS procedure #3	- Get Filesystem Root (!!!OBSELETE!!!)

*/
int NFSPROC_ROOT(uic,hname,username,reply)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
{
    return write_XDR_stat(NFSERR_NXIO, &reply);
}



/**********************************************************************

NFS procedure #4	- Look Up File Name

*/

NFSPROC_LOOKUP(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    diropargs dop_args;
    file_record *drp,*frp;
    int RC,len=0;
    fhandle hand;
    char *s,fname[MAXNAMLEN+1], dirname[MAXNAMLEN+1];
    fattr attribs;
    char *closebracket, *lastdot, *colon;

    /* Load the arguments */
    XDR$ntov_arb(&args, &dop_args.dir, sizeof(dop_args.dir));
    XDR$ntov_arb(&args, &dop_args.name.length, sizeof(dop_args.name.length));
    XDR$ntov_arb(&args, &dop_args.name.data, nfs_int(dop_args.name.length));
    dop_args.name.data[nfs_int(dop_args.name.length)] = 0;

    /* find the directory */
    drp = build_fname(&dop_args,fname,0);
    if (drp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    /* Specially handle the "." and ".." directories */
    if (!strcmp(".", dop_args.name.data))
        updir(fname);
    if (!strcmp("..", dop_args.name.data))
        updir(updir(fname));

    /* get the frp */
    RC = make_handle(fname,hand);
    if (RC != NFS_OK) return write_XDR_stat(RC,&reply);
    frp = get_fr(hand);
    if (frp == NULL) return write_XDR_stat(NFSERR_NOENT,&reply);

    strcpy(dirname, fname);
    updir(dirname);

    if (!(RC = has_access(username, hname, dirname, ARM$M_READ)) ||
	!(RC = has_access(username, hname, dirname, ARM$M_EXECUTE)))
    {
	/* get attributes */
	RC = get_file_attributes(frp,&attribs, hname);
	if (!RC) return write_XDR_stat(NFSERR_ACCES,&reply);

	len = write_XDR_stat(NFS_OK,&reply);
	len += write_XDR_fhandle(hand,&reply);	/* add another handle */
	len += write_XDR_file_attributes(&attribs,&reply);
    }
    else len = write_XDR_stat(RC, &reply);

    return len;
}



/**********************************************************************

NFS procedure #5	- Read From Symbolic Link

*/
NFSPROC_READLINK(uic,hname,username,reply,hand)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    fhandle hand;
{
    file_record *frp;

    /* find_fr the link */
    frp = get_fr(hand);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    return write_XDR_stat(NFSERR_NOENT,&reply);
}



/**********************************************************************

NFS procedure #6	- Read From file

*/
NFSPROC_READ(uic,hname,username,reply,rargs)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    struct {
	fhandle file;
	unsigned offset;
	unsigned count;
	unsigned totalcount;
	} *rargs;
{
    int RC,len,i,nread;
    file_record *frp;
    unsigned offset,count,out_count,total_count,transcount,recordlen,
        EOFbyte, byteswanted, bytesnotwanted;
    fattr attribs;
    char buffer[BLKSIZE], transbuffer[BLKSIZE], *tbuff;
    unsigned short QIOSB[4];

    /* find_fr the file */
    frp = get_fr(&rargs->file);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    /* Read data from disk */
    XDR$ntov_uint(&rargs->offset,&offset);
    XDR$ntov_uint(&rargs->count,&byteswanted);
    if (byteswanted > BLKSIZE) byteswanted = BLKSIZE;

    if (frp->fchan == 0) return write_XDR_stat(NFSERR_NXIO,&reply);

    /* Check attributes */
    if ((RC = has_access(username, hname, frp->filename, ARM$M_READ)) ||
	!get_file_attributes(frp,&attribs, hname))
	return write_XDR_stat(RC,&reply);

    /* Is the file of a "foreign" type which is not handled (anything other
       than FAT$C_SEQUENTIAL)? */
    if (frp->fat.fat$b_rtype.fat$v_fileorg != FAT$C_SEQUENTIAL)
        return write_XDR_stat(NFSERR_NXIO, &reply);

    if (frp->offsets)
        RC = SYS$QIOW(0, frp->fchan, IO$_READVBLK, &QIOSB, 0, 0,
	    &buffer, BLKSIZE, (frp->offsets[offset / 512].bytestart / 512) + 1,
	    0, 0, 0);
    else
	RC = SYS$QIOW(0, frp->fchan, IO$_READVBLK, &QIOSB, 0, 0,
            &buffer, BLKSIZE, (offset / 512) + 1, 0, 0, 0);
    if (ERROR(RC) || ((QIOSB[0] != SS$_ENDOFFILE) && ERROR(QIOSB[0])))
        return write_XDR_stat(NFSERR_IO,&reply);

    /* Check to see if data needs to be converted.  This will be true for
       almost all data formats except for STREAMLF.  For all files, the FAT
       must indicate that there is carriage control for us to insert LF's into
       the stream - if not, we send the data through blindly (since it doesn't
       make any difference).  The translation methods for each type are:
	FAT$C_FIXED	pure binary
	FAT$C_VARIABLE	first two bytes represent length of current record
			round length up to next even value for padded length
	FAT$C_VFC	not handled yet
	FAT$C_UNDEFINED	pure binary
	FAT$C_STREAM	CRLF stream (what this means, I don't know)
	FAT$C_STREAMLF	LF stream - handled as pure binary data since it's
			what the other side expects
	FAT$C_STREAMCR	replace the CR terminators with LF for remote side */

    /*
     * Change to 65536 from 256 per mail conversation with
     * Marc Shannon.
     *
     * Tom Allebrandi (Allebrandi@Inland.Com) 12-Jul-1991
     */
    count = QIOSB[1] + QIOSB[2]*65536;
    EOFbyte = (frp->fat.fat$l_efblk.fat$w_efblkl +
	       (frp->fat.fat$l_efblk.fat$w_efblkh * 65536) - 1) * 512 +
	      frp->fat.fat$w_ffbyte - 1;


    /*
     * Fix a benign typo (remove & from transbuffer)
     *
     * Tom Allebrandi (Allebrandi@Inland.Com) 12-Jul-1991
     */
    tbuff = transbuffer; out_count = 0;
    switch (frp->fat.fat$b_rtype.fat$v_rtype)
    {
    case FAT$C_VARIABLE:
	recordlen = frp->offsets[offset / 512].reclen;
	bytesnotwanted = offset % 512;
        for (transcount = frp->offsets[offset / 512].bytestart % 512;
	     (transcount < count) &&
	     (transcount +
	       frp->offsets[offset / 512].bytestart / 512 * 512 < EOFbyte) &&
	     (out_count < byteswanted);)
	{
	    if ((recordlen == 0) || (recordlen == 0xFFFFFFFF))
	    {
	        if (recordlen == 0xFFFFFFFF)
		    ;
		else if (bytesnotwanted)
		    bytesnotwanted--;
		else
		    if (frp->fat.fat$b_rattrib & FAT$M_IMPLIEDCC) {
			*tbuff++ = '\n';
			out_count++;
		    }

		if (transcount & 1)
		    transcount++;

		memcpy(&recordlen, &buffer[transcount], 2);
		recordlen &= 0xffff;
		/* Surprize! It seems that a record length of 0xFFFF is
		 * interpreted as "skip to end of block"!!!!!  I wonder what
		 * other demons live near here!
		 *
		 * Tom Allebrandi (Allebrandi@Inland.Com) 3-Sep-1991
		 */
		if (recordlen == 0xFFFF)
		{
		    /* Round the block pointer up to the next block and skip
		     * the rest of the loop.
		     */
		    transcount = transcount - (transcount % 512) + 512;
		    recordlen = 0xFFFFFFFF; /* Can't use 0 since that might
					       cause a newline on the next
					       pass */
		    continue;
		}
		
		transcount += 2;
	    }

	    if (bytesnotwanted && (recordlen > bytesnotwanted))
	    {
	        recordlen -= bytesnotwanted;
		transcount += bytesnotwanted;
		bytesnotwanted = 0;
	    }

	    if (bytesnotwanted) {
	        bytesnotwanted -= recordlen;
		transcount += recordlen;
		recordlen = 0;
	    }
	    else {
	        if (out_count + recordlen > byteswanted)
		{
		    if (byteswanted > out_count) {
		        memcpy(tbuff, &buffer[transcount],
			       byteswanted - out_count);
			out_count += byteswanted - out_count;
		    }
		    break;
		}

		if (transcount + recordlen > count)
		{
		    memcpy(tbuff, &buffer[transcount], count-transcount);
		    out_count += count - transcount;
		    break;
		}

		memcpy(tbuff, &buffer[transcount], recordlen);
		tbuff += recordlen;
		transcount += recordlen;
		out_count += recordlen;
		recordlen = 0;
	    }
	}

	/*
	 * Newline does not get simulated at end of file. Catch it
	 * here as a special case.
	 *
	 * Tom Allebrandi (Allebrandi@Inland.Com) 12-Jul-1991
	 */
	if (transcount + frp->offsets[offset / 512].bytestart / 512 * 512
								>= EOFbyte)
	    {
	    *tbuff++ = '\n';
	    out_count++;
	    }
        break;

    case FAT$C_STREAMCR:
        for (transcount = 0;transcount < count;transcount++)
	    if (buffer[transcount] == '\r')
	        buffer[transcount] = '\n';
    case FAT$C_STREAM:
    case FAT$C_FIXED:
    case FAT$C_STREAMLF:
    case FAT$C_UNDEFINED:
	count = MIN(byteswanted, MIN(count - (offset % 512),
		EOFbyte - offset + 1));
        memcpy(tbuff, buffer + (offset % 512), count);
	out_count = count;
        break;
    default:
	return write_XDR_stat(NFSERR_NXIO, &reply);
        break;
    }

    out_count = MIN(out_count, byteswanted);

    /* return it all */
    len = write_XDR_stat(NFS_OK,&reply);
    len += write_XDR_file_attributes(&attribs,&reply);
    XDR$vton_uint(&out_count,reply++); len++;
    memcpy(reply,transbuffer,out_count);  len += RNDUP(out_count)/4;
    return len;
}



/**********************************************************************

NFS procedure #7	- Write to Cache (!!!NYI!!!)

*/
NFSPROC_WRITECACHE(uic,hname,username,reply)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
{
    return write_XDR_stat(NFSERR_NXIO, &reply);
}



/**********************************************************************

NFS procedure #8	- Write to File

*/
NFSPROC_WRITE(uic,hname,username,reply,wargs)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    struct {
	fhandle file;
	unsigned beginoffset;
	unsigned offset;
	unsigned totalcount;
	nfsdata data;
	} *wargs;
{
    int RC,len,nwrote;
    file_record *frp;
    unsigned offset,count;
    fattr attribs;
    unsigned short QIOSB[4];
    unsigned long oldEOF, newEOF, HIEOF;

    /* find_fr the file */
    frp = get_fr(&wargs->file);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    /* write data to disk */
    XDR$ntov_uint(&wargs->offset,&offset);
    XDR$ntov_uint(&wargs->data.length,&count);

    /* Implement a file descriptor cache!  NOW BOY! */
    if (frp->fchan == 0) {
	return write_XDR_stat(NFSERR_NXIO,&reply);
	}

    /* Check attributes */
    if ((RC = has_access(username, hname, frp->filename, ARM$M_WRITE)) ||
	!get_file_attributes(frp,&attribs, hname))
	return write_XDR_stat(RC,&reply);

    /* If it's not a handle-able file type, refuse the write */
    if ((frp->fat.fat$b_rtype.fat$v_fileorg != FAT$C_SEQUENTIAL) ||
	((frp->fat.fat$b_rtype.fat$v_rtype != FAT$C_STREAMLF) &&
	 (frp->fat.fat$b_rtype.fat$v_rtype != FAT$C_UNDEFINED)))
        return write_XDR_stat(NFSERR_NXIO, &reply);

    RC = SYS$QIOW(0, frp->fchan, IO$_ACCESS, &QIOSB, 0, 0,
                  &frp->fib_desc, 0, 0, 0, &frp->atrblock, 0);

    if (ERROR(RC) || ERROR(QIOSB[0]))
       return write_XDR_stat(NFSERR_IO,&reply);

    newEOF = offset + count;

    oldEOF = ((frp->fat.fat$l_efblk.fat$w_efblkh*65536 +
               frp->fat.fat$l_efblk.fat$w_efblkl) - 1) * 512 +
             frp->fat.fat$w_ffbyte;

    HIEOF = (frp->fat.fat$l_hiblk.fat$w_hiblkh*65536 +
             frp->fat.fat$l_hiblk.fat$w_hiblkl) * 512;

    if (newEOF > HIEOF) {
       frp->fib.fib$r_exctl_overlay.fib$w_exctl = FIB$M_EXTEND;
       frp->fib.fib$l_exsz = ((newEOF - HIEOF) + 511) / 512;
       frp->fib.fib$l_exvbn = 0;
    } else
       frp->fib.fib$l_exsz = 0;

    if (newEOF > oldEOF) {
       frp->fat.fat$l_efblk.fat$w_efblkh = (((newEOF / 512) + 1) / 65536);
       frp->fat.fat$l_efblk.fat$w_efblkl = (((newEOF / 512) + 1) % 65536);
       frp->fat.fat$w_ffbyte = (newEOF % 512);
       frp->filesize = newEOF;
    }

    if ((newEOF > oldEOF) || (frp->fib.fib$l_exsz != 0)) {
       RC = SYS$QIOW(0, frp->fchan, IO$_MODIFY, &QIOSB, 0, 0,
                     &frp->fib_desc, 0, 0, 0, &frp->atrblock, 0);

       /* Clear out the command bits to prevent them from showing up again */
       frp->fib.fib$r_exctl_overlay.fib$w_exctl = 0;

       if (ERROR(RC))
          return write_XDR_stat(NFSERR_NXIO,&reply);

       RC = QIOSB[0];
       if (ERROR(RC)) {
	  if (RC == SS$_EXDISKQUOTA)
	      return write_XDR_stat(NFSERR_DQUOT, &reply);

          return write_XDR_stat(NFSERR_NXIO,&reply);
       }
    }

    if ((offset % 512) || (count & 1))
    {
        /* Block does not start on an even block boundary.  Read in the
	   current block and append the data to it.  This will give us the
	   ability to start writing in the middle of a block.  (Sigh...) */

	/* Changed because UDA50 and KDA50 (and the like) controllers require
	   that data be written with even number of bytes...Therefore, we call
	   this routine whenever the count is not even. */

	char *inoutbuf;
	int bytesneeded;

	bytesneeded = (offset % 512) +		/* Pre-block data */
		      count;			/* Given data */
	bytesneeded = (bytesneeded + 511) / 512 * 512;	    /* Round up */

	inoutbuf = (char *)malloc(bytesneeded);

	RC = SYS$QIOW(0, frp->fchan, IO$_READVBLK, &QIOSB, 0, 0,
		      inoutbuf, bytesneeded, (offset / 512) + 1, 0, 0, 0);
	if (ERROR(RC) || ERROR(QIOSB[0])) {
	    free(inoutbuf);
	    return write_XDR_stat(NFSERR_IO, &reply);
	}

	memcpy(inoutbuf + (offset % 512), &wargs->data.data, count);

	RC = SYS$QIOW(0, frp->fchan, IO$_WRITEVBLK, &QIOSB, 0, 0,
		      inoutbuf, bytesneeded, (offset / 512) + 1, 0, 0, 0);

	free(inoutbuf);

	if (ERROR(RC) || ERROR(QIOSB[0]))
	    return write_XDR_stat(NFSERR_IO, &reply);
    }
    else {
	RC = SYS$QIOW(0, frp->fchan, IO$_WRITEVBLK, &QIOSB, 0, 0,
                      &wargs->data.data, count, (offset / 512) + 1, 0, 0, 0);
	if (ERROR(RC) || ERROR(QIOSB[0]))
	    return write_XDR_stat(NFSERR_IO, &reply);
    }

    /* return it all */
    len = write_XDR_stat(NFS_OK,&reply);
    get_file_attributes(frp, &attribs, hname);
    len += write_XDR_file_attributes(&attribs,&reply);
    return len;
}



/**********************************************************************

NFS procedure #9	- Create File

*/
NFSPROC_CREATE(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    struct {
	diropargs where;
	sattr attribs;
	} cargs;
    file_record *drp,*frp;
    int RC,len=0,fd;
    fhandle hand;
    char fname[MAXNAMLEN+1], dirname[MAXNAMLEN+1];
    fattr attribs;
    int fnamelen;
    char *brkpos, *dotpos;

    /* Load the create arguments */
    XDR$ntov_arb(&args, &cargs.where.dir, sizeof(cargs.where.dir));
    XDR$ntov_arb(&args, &cargs.where.name.length,
	sizeof(cargs.where.name.length));
    XDR$ntov_arb(&args, &cargs.where.name.data,
	nfs_int(cargs.where.name.length));
    cargs.where.name.data[nfs_int(cargs.where.name.length)] = 0;
    XDR$ntov_arb(&args, &cargs.attribs, sizeof(cargs.attribs));

    /* find_fr the directory */
    drp = build_fname(&cargs.where,fname,0);
    if (drp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    brkpos = strrchr(fname, ']') + 1;
    dotpos = strrchr(fname, '.');
    if (dotpos < brkpos)
	dotpos = &fname + strlen(fname);

    if ((dotpos - brkpos > 39) || (strlen(dotpos) > 39))
        return write_XDR_stat(NFSERR_NAMETOOLONG, &reply);

    /* Just do it */
    /* Make sure the user has write access to the directory */
    strcpy(dirname, fname);
    updir(dirname);
    if ((RC = has_access(username, hname, dirname, ARM$M_WRITE)))
        return write_XDR_stat(RC, &reply);

    fd = creat(fname,0);
    if (fd < 0)
	write_XDR_stat(errno,&reply);

    close(fd);
    chown(fname, uic & 0xffff, uic>>16);

    RC = make_handle(fname, hand);
    if (RC != NFS_OK) return write_XDR_stat(RC,&reply);
    frp = get_fr(hand);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_NOENT,&reply);

    /* Set the new file's attributes */
    RC = set_file_attributes(frp, &cargs.attribs, hname);
    if (RC != NFS_OK)
        return write_XDR_stat(RC, &reply);

    /* get attributes */
    RC = get_file_attributes(frp,&attribs, hname);
    if (!RC)
	return write_XDR_stat(NFSERR_ACCES,&reply);

    len = write_XDR_stat(NFS_OK,&reply);
    len += write_XDR_fhandle(hand,&reply);	/* add another handle */
    len += write_XDR_file_attributes(&attribs,&reply);

    return len;
}



/**********************************************************************

NFS procedure #10	- Remove File

*/

NFSPROC_REMOVE(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    diropargs dop_args;
    file_record *drp,*frp;
    int RC,len=0;
    fhandle hand;
    char fname[MAXNAMLEN+1], dirname[MAXNAMLEN+1];

    /* Load the arguments */
    XDR$ntov_arb(&args, &dop_args.dir, sizeof(dop_args.dir));
    XDR$ntov_arb(&args, &dop_args.name.length,
	sizeof(dop_args.name.length));
    XDR$ntov_arb(&args, &dop_args.name.data, nfs_int(dop_args.name.length));
    dop_args.name.data[nfs_int(dop_args.name.length)] = 0;

    /* find_fr the directory */
    drp = build_fname(&dop_args,fname,0);
    if (drp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    make_handle(fname, &hand);
    frp = find_fr(hand);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_NOENT,&reply);

    /* Make sure the user has write access to the directory */
    strcpy(dirname, fname);
    updir(dirname);
    if ((RC = has_access(username, hname, dirname, ARM$M_WRITE)))
        return write_XDR_stat(RC, &reply);

    delete_fr(frp);

    RC = delete(fname);
    if (RC < 0)
	return write_XDR_stat(NFSERR_ACCES,&reply);

    return write_XDR_stat(NFS_OK,&reply);
}



/**********************************************************************

NFS procedure #11	- Rename File

*/

NFSPROC_RENAME(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    struct {
	diropargs from;
	diropargs to;
	} ren_args;
    int RC, fromnamelen, tonamelen;
    char from_name[MAXNAMLEN+1],to_name[MAXNAMLEN+1], from_dir[MAXNAMLEN+1],
        to_dir[MAXNAMLEN+1];
    file_record *sdrp,*ddrp,*frp;
    fhandle fhand;
    struct dsc$descriptor oldfname, newfname;
    struct FAB oldFAB, newFAB;
    struct NAM oldNAM, newNAM;
    char *brkpos, *dotpos;

    /* Load up the arguments */
    XDR$ntov_arb(&args, &ren_args.from.dir, sizeof(ren_args.from.dir));
    XDR$ntov_arb(&args, &ren_args.from.name.length,
	sizeof(ren_args.from.name.length));
    XDR$ntov_arb(&args, &ren_args.from.name.data,
	nfs_int(ren_args.from.name.length));
    ren_args.from.name.data[nfs_int(ren_args.from.name.length)] = 0;
    XDR$ntov_arb(&args, &ren_args.to.dir, sizeof(ren_args.to.dir));
    XDR$ntov_arb(&args, &ren_args.to.name.length,
	sizeof(ren_args.to.name.length));
    XDR$ntov_arb(&args, &ren_args.to.name.data,
	nfs_int(ren_args.to.name.length));
    ren_args.to.name.data[nfs_int(ren_args.to.name.length)] = 0;

    sdrp = build_fname(&ren_args.from,from_name,0);
    if (sdrp == NULL )return write_XDR_stat(NFSERR_STALE,&reply);
    ddrp = build_fname(&ren_args.to,to_name,0);
    if (ddrp == NULL )return write_XDR_stat(NFSERR_STALE,&reply);

    brkpos = strrchr(to_name, ']') + 1;
    dotpos = strrchr(to_name, '.');
    if (dotpos < brkpos)
	dotpos = &to_name + strlen(to_name);

    if ((dotpos - brkpos > 39) || (strlen(dotpos) > 39))
	return write_XDR_stat(NFSERR_NAMETOOLONG, &reply);

    /* Check appropriate access... */
    strcpy(from_dir, from_name);
    updir(from_dir);
    strcpy(to_dir, to_name);
    updir(to_dir);
    if ((RC = has_access(username, hname, from_dir, ARM$M_WRITE)) ||
	(RC = has_access(username, hname, to_dir, ARM$M_WRITE)))
        return write_XDR_stat(RC, &reply);

    RC = make_handle(from_name,fhand);
    if (RC != NFS_OK) return write_XDR_stat(RC,&reply);

    frp = get_fr(fhand);
    if (frp == NULL) return write_XDR_stat(NFSERR_NOENT,&reply);

    delete_fr(frp);   /* Delete it from the cache */

    oldFAB = newFAB = cc$rms_fab;
    oldNAM = newNAM = cc$rms_nam;

    oldFAB.fab$l_fna = &from_name;         newFAB.fab$l_fna = &to_name;
    oldFAB.fab$b_fns = strlen(from_name);  newFAB.fab$b_fns = strlen(to_name);
    oldFAB.fab$l_nam = &oldNAM;            newFAB.fab$l_nam = &newNAM;

    oldNAM.nam$b_ess = newNAM.nam$b_ess = NAM$C_MAXRSS;
    oldNAM.nam$l_esa = (char *)malloc(NAM$C_MAXRSS);
    newNAM.nam$l_esa = (char *)malloc(NAM$C_MAXRSS);

    RC = SYS$RENAME(&oldFAB, 0, 0, &newFAB);
    if (ERROR(RC))
	return write_XDR_stat(NFSERR_IO, &reply);

    free(oldNAM.nam$l_esa);
    free(newNAM.nam$l_esa);
    return write_XDR_stat(NFS_OK,&reply);
}



/**********************************************************************

NFS procedure #12	- Create Link to File

*/
NFSPROC_LINK(uic,hname,username,reply)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
{
    return write_XDR_stat(NFSERR_NXIO, &reply);
}



/**********************************************************************

NFS procedure #13	- Create Symbolic Link

*/
NFSPROC_SYMLINK(uic,hname,username,reply)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
{
    return write_XDR_stat(NFSERR_NXIO, &reply);
}



/**********************************************************************

NFS procedure #14	- Create Directory

*/
NFSPROC_MKDIR(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    struct {
	diropargs where;
	sattr settribs;
	} mkdirargs;
    file_record *drp,*frp;
    int RC,len=0,fnlen;
    fhandle hand;
    char *s,fname[MAXNAMLEN+1],nfname[MAXNAMLEN+1],dirname[MAXNAMLEN+1],
        *dirspot;
    fattr attribs;

    /* Load up the arguments */
    XDR$ntov_arb(&args, &mkdirargs.where.dir, sizeof(mkdirargs.where.dir));
    XDR$ntov_arb(&args, &mkdirargs.where.name.length,
	sizeof(mkdirargs.where.name.length));
    XDR$ntov_arb(&args, &mkdirargs.where.name.data,
	nfs_int(mkdirargs.where.name.length));
    mkdirargs.where.name.data[nfs_int(mkdirargs.where.name.length)] = 0;
    XDR$ntov_arb(&args, &mkdirargs.settribs, sizeof(mkdirargs.settribs));

    /* find_fr the directory */
    drp = get_fr(&mkdirargs.where.dir);
    if (drp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    /* convert [foo]bar.dir into [foo.bar] */
    strcpy(fname,&drp->filename);
    s = file_to_dir(fname) - 1;  *s++ = '.';

    /* append given file name */
    fnlen = FNAME_NET_TO_VMS(&mkdirargs.where.name.data,
			     nfs_int(mkdirargs.where.name.length),
			     nfname,1);

    if (nfname[fnlen - 1] == '.')
        fnlen--;		/* Remove trailing dot from dir name */

    memcpy(s,nfname,fnlen);
    s[fnlen++] = ']';     s[fnlen] = 0;

    strcpy(dirname, fname);
    updir(updir(dirname));	    /* Get parent directory name as file */
    if ((RC = has_access(username, hname, dirname, ARM$M_WRITE)))
        return write_XDR_stat(RC, &reply);

    RC = mkdir(fname,0777);
    if (RC < 0)
	write_XDR_stat(errno,&reply);

    dirspot = strrchr(fname, '.');
    *dirspot = ']';
    dirspot = strrchr(fname, ']');
    strcpy(dirspot, ".DIR");
    RC = make_handle(fname, hand);
    if (RC != NFS_OK)
	return write_XDR_stat(RC,&reply);
    frp = get_fr(hand);
    if (frp == NULL)
	return write_XDR_stat(NFSERR_NOENT,&reply);

    /* Set the new directory's attributes */
    RC = set_file_attributes(frp, &mkdirargs.settribs, hname);
    if (RC != NFS_OK)
        return write_XDR_stat(RC, &reply);

    RC = get_file_attributes(frp, &attribs, hname);
    if (!RC)
	return write_XDR_stat(NFSERR_ACCES,&reply);

    len = write_XDR_stat(NFS_OK,&reply);
    len += write_XDR_fhandle(hand,&reply);
    len += write_XDR_file_attributes(&attribs,&reply);

    return len;
}



/**********************************************************************

NFS procedure #15	- Remove Directory

*/
NFSPROC_RMDIR(uic,hname,username,reply,args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    char *args;
{
    diropargs dop_args;
    file_record *drp,*frp;
    int RC,len=0,fnlen;
    fhandle hand;
    char *s,fname[MAXNAMLEN+1], dirname[MAXNAMLEN+1];
    fattr attribs;

    /* Load the arguments */
    XDR$ntov_arb(&args, &dop_args.dir, sizeof(dop_args.dir));
    XDR$ntov_arb(&args, &dop_args.name.length, sizeof(dop_args.name.length));
    XDR$ntov_arb(&args, &dop_args.name.data, nfs_int(dop_args.name.length));
    dop_args.name.data[nfs_int(dop_args.name.length)] = 0;

    /* find_fr the directory */
    drp = build_fname(&dop_args,fname,1);
    if (drp == NULL)
	return write_XDR_stat(NFSERR_STALE,&reply);

    strcpy(dirname, fname);
    updir(dirname);
    if (has_access(username, hname, fname, ARM$M_DELETE) ||
        has_access(username, hname, dirname, ARM$M_WRITE))
        return write_XDR_stat(NFSERR_ACCES, &reply);

    RC = delete(fname);
    if (RC < 0)
	write_XDR_stat(errno,&reply);

    return write_XDR_stat(NFS_OK,&reply);
}



/**********************************************************************

NFS procedure #16	- Read From Directory

*/

NFSPROC_READDIR(uic,hname,username,reply,rd_args)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    readdirargs *rd_args;
{
    int RC,len=0,nfiles;
    unsigned cookie,count,EOdir;
    file_record *frp;
    struct FAB fab;
    struct NAM nam;
    char esbuff[NAM$C_MAXRSS],rsbuff[NAM$C_MAXRSS];
    char *s,fname_str[MAXNAMLEN+1];
    long *repbuf = reply;
    fattr attribs;
    stat_t fstats;
    unsigned long inode;

    /* get_fr the file */
    frp = get_fr(rd_args->dir);
    if (frp == NULL) return write_XDR_stat(NFSERR_STALE,&reply);

    XDR$ntov_uint(&rd_args->cookie,&cookie);
    XDR$ntov_uint(&rd_args->count,&count);

    /* convert [foo]bar.dir into [foo.bar] */
    strcpy(fname_str,&frp->filename); s = fname_str;
    while (*s && (*s != ']')) s++;
    if (!*s) return 0; else *s++ = '.';
    while (*s && (*s != '.')) s++;
    if (!*s) return 0;
    *s++ = ']'; *s = 0;

    /* Setup for the RMS directory search */
    fab = cc$rms_fab;
    nam = cc$rms_nam;
    fab.fab$l_dna = fname_str;
    fab.fab$b_dns = strlen(fname_str);
    fab.fab$l_fna = "*.*";
    fab.fab$b_fns = 3;
    nam.nam$l_rsa = rsbuff;
    nam.nam$l_esa = esbuff;
    nam.nam$b_ess = nam.nam$b_rss = NAM$C_MAXRSS;
    fab.fab$l_nam = &nam;

    RC = sys$parse(&fab,0,0);
    if (ERROR(RC))
	return 0;

    esbuff[nam.nam$b_esl] = 0;

    len = write_XDR_stat(NFS_OK,&reply);
    nfiles = 2;		/* Count the "." and ".." entries */
    if (!cookie)
    {
	char dirname[MAX_FNAME];
	fhandle dirhand;

        /* We're starting off a new directory listing.  Include here, and this
	   is bad since we don't really know whether or not the packet is big
	   enough, but we assume that it is for 32 bytes, the directories "."
	   and ".." for this directory. */
	XDR$vton_int(&1, reply++);  len++;	    /* Data here - "." */
	XDR$vton_int(&frp->fib.fib$r_fid_overlay, reply++);	len++;
	XDR$vton_int(&1, reply++);  len++;	    /* Length of "." */
	memset(reply, 0, RNDUP(1));
	strncpy(reply, ".", 1);
	reply += (1 + RNDUP(1))/4;  len += (1 + RNDUP(1))/4;
	cookie++;
	XDR$vton_int(&cookie, reply++);  len++;	    /* Cookie number 1 */

	strcpy(dirname, frp->filename);
	updir(dirname);
	RC = make_handle(dirname, dirhand);
	if (RC == NFS_OK)
	{
	    /* Now write out the parent's directory information */
	    XDR$vton_int(&1, reply++);  len++;	    /* More data - ".." */
	    XDR$vton_int(&dirhand[16], reply++);    len++;
	    XDR$vton_int(&2, reply++);  len++;	    /* Length of ".." */
	    memset(reply, 0, RNDUP(2));
	    strncpy(reply, "..", 2);
	    reply += (1 + RNDUP(2))/4;  len += (1 + RNDUP(2))/4;
	    cookie++;
	    XDR$vton_int(&cookie, reply++);  len++;	/* Cookie number 2 */
	}
    }

    /* All done with directory faking, now do the real directory... */
    while (TRUE) {
        char *nstr; int nlen=0;

	RC = sys$search(&fab,0,0);
	if ((RC == RMS$_NMF) || (RC == RMS$_FNF)) break;
	if (ERROR(RC))
	    return write_XDR_stat(NFSERR_IO,&repbuf);

	if (nfiles++ < cookie) continue;
	cookie++;

	rsbuff[nam.nam$b_rsl] = 0;
        s=rsbuff;
	while (*s && (*s++ != ']'));  nstr = s;  /* nstr=start of fname */
	while (*s && (*s++ != ';')) nlen++;	/* size of it */

	stat(rsbuff, &fstats);
	inode = (fstats.st_ino[0] * 65536) + fstats.st_ino[1];

	/* Pretty it up a bit */
	nlen = FNAME_VMS_TO_NET(nstr,nlen,fname_str,0,0);

	/* too much data? */
	if (  (4*(len-1) + 4*4 + RNDUP(nlen)/4)   >   count )
	    { RC = SS$_NORMAL; break; }

	/* write this entry into the reply buffer */
	XDR$vton_int(&1,reply++); len++;	     /* optional data: yes */
	XDR$vton_int(&inode,reply++); len++;			    /* fid */

	/* filename */
	XDR$vton_int(&nlen,reply++); len++;		/* filename length */
	memset(reply,0,RNDUP(nlen)); strncpy(reply,fname_str,nlen);

	reply += (1 + RNDUP(nlen))/4; len += (1 + RNDUP(nlen))/4;
	XDR$vton_int(&cookie,reply++); len++;/* WANT COOOOOOOKIE!!! */
	}

/*    if (RC != SS$_NORMAL) {
	get_file_attributes(frp,&attribs, hname);
	XDR$vton_int(&attribs.size,reply-1);
	}
*/
    XDR$vton_int(&0,reply++); len++;		/* optional data: no */
    EOdir = (RC != SS$_NORMAL);
    XDR$vton_int(&EOdir, reply++); len++;
    return len;
}



/**********************************************************************

NFS procedure #17	- Get Filesystem Statistics

*/

struct item {
    short buff_len;
    short item_code;
    long buff_addr;
    long ret_len_addr;
    };

NFSPROC_STATFS(uic,hname,username,reply,hand)
    unsigned uic;
    char *hname;
    char *username;
    long *reply;
    fhandle hand;
{
/*!!!HACK!!! don't hardwire this stuff! */

    int RC,length;
    struct dsc$descriptor desc;
    struct item items[3];
    long max_blocks,free_blocks;

    desc.dsc$w_length	= hand[0];
    desc.dsc$b_dtype	= 0;
    desc.dsc$b_dtype	= 0;
    desc.dsc$a_pointer	= &hand[1];

    items[0].buff_len	= 4;
    items[0].item_code	= DVI$_MAXBLOCK;
    items[0].buff_addr	= &max_blocks;
    items[0].ret_len_addr	= &length;

    items[1].buff_len	= 4;
    items[1].item_code	= DVI$_FREEBLOCKS;
    items[1].buff_addr	= &free_blocks;
    items[1].ret_len_addr	= &length;

    /* longword of zeros terminates the item list */
    items[2].buff_len	= 0;
    items[2].item_code	= 0;

    RC = sys$getdviw(0,0,&desc,items,0,0,0,0);
    if (ERROR(RC)) {
	if (RC == SS$_NOSUCHDEV)
	    XDR$vton_int(&NFSERR_NODEV,reply);
	else XDR$vton_int(&NFSERR_ACCES,reply);
	return 4;
	}

    write_XDR_stat(NFS_OK,&reply);

    XDR$vton_int(&8192,reply++);	/* optimal transfer size */
    XDR$vton_int(&512,reply++);		/* FS block size */
    XDR$vton_int(&max_blocks,reply++);	/* total blocks on disk */
    XDR$vton_int(&free_blocks,reply++);	/* available blocks on disk */
    XDR$vton_int(&free_blocks,reply++);	/* available blocks on disk */

    return 1 + 5;
}



/**********************************************************************/

/* Initialize the module.  This should be an entry point. */

int NFS$INIT(IPACP_Int)
    void *IPACP_Int;
{
    int i;
    struct {
        unsigned short buflen, itmcod;
	unsigned long bufadr, retlenadr;
	unsigned long null;
    } itemlist;
    unsigned long remfiles, maxchannels;

    IPACP_Interface = IPACP_Int;

    for (i=0; i<HTAB_SIZE; i++) {
	htab[i].queue_head = &htab[i].queue_head;
	htab[i].queue_tail = &htab[i].queue_head;
	htab[i].count = 0;
	}

    fifo_timer.head = fifo_timer.tail = &fifo_timer.head;

    itemlist.buflen = sizeof(remfiles);
    itemlist.itmcod = JPI$_FILCNT;
    itemlist.bufadr = &remfiles;
    itemlist.retlenadr = 0;
    itemlist.null = 0;

    SYS$GETJPIW(0, 0, 0, &itemlist, 0, 0, 0);

    remfiles -= 5;			/* Allow room for log files and such */

    itemlist.buflen = sizeof(maxchannels);
    itemlist.itmcod = SYI$_CHANNELCNT;
    itemlist.bufadr = &maxchannels;
    itemlist.retlenadr = 0;
    itemlist.null = 0;

    SYS$GETSYIW(0, 0, 0, &itemlist, 0, 0, 0);

    maxchannels -= 32;			/* Allow room for added system chans */

    MAX_CACHE = (remfiles > maxchannels) ? maxchannels : remfiles;

    clean_frs();    /* Start up the NFS timer cleaner */

    nfs_proc_vector[0] = NFSPROC_NULL;
    nfs_proc_vector[1] = NFSPROC_GETATTR;
    nfs_proc_vector[2] = NFSPROC_SETATTR;
    nfs_proc_vector[3] = NFSPROC_ROOT;
    nfs_proc_vector[4] = NFSPROC_LOOKUP;
    nfs_proc_vector[5] = NFSPROC_READLINK;
    nfs_proc_vector[6] = NFSPROC_READ;
    nfs_proc_vector[7] = NFSPROC_WRITECACHE;
    nfs_proc_vector[8] = NFSPROC_WRITE;
    nfs_proc_vector[9] = NFSPROC_CREATE;
    nfs_proc_vector[10] = NFSPROC_REMOVE;
    nfs_proc_vector[11] = NFSPROC_RENAME;
    nfs_proc_vector[12] = NFSPROC_LINK;
    nfs_proc_vector[13] = NFSPROC_SYMLINK;
    nfs_proc_vector[14] = NFSPROC_MKDIR;
    nfs_proc_vector[15] = NFSPROC_RMDIR;
    nfs_proc_vector[16] = NFSPROC_READDIR;
    nfs_proc_vector[17] = NFSPROC_STATFS;

    return(NFS$DISPATCH);
}
