#include <linux/kernel.h>
#include "vms_fs.h"
#include "vms_fs_sb.h"
#include <linux/fs.h>

#define err0 /**/
#define err1 /**/


void vms_print_header(struct _fh2 *vh) {
	struct _fi2 *id;
	int i;

	i = (vh->fh2$b_idoffset)*2;
	id = (struct _fi2 *)((long)vh + i);

	printk("vms_print_header: \n");
	printk("ident area offset: %i\n",vh->fh2$b_idoffset);
	printk("map area offset: %i\n",vh->fh2$b_mpoffset);
	printk("access control list offset: %i\n",vh->fh2$b_acoffset);
	printk("reserved area offset: %i\n",vh->fh2$b_rsoffset);
	printk("extension segment number: %i\n",vh->fh2$w_seg_num);
	printk("structure level & version: %i\n", vh->fh2$w_struclev);
	printk("file number: %i\n",vh->fh2$w_fid.fid$w_num);
	printk("file sequence no: %i\n",vh->fh2$w_fid.fid$w_seq);
	printk("relative volume number: %i\n",vh->fh2$w_fid.fid$w_rvn);
	printk("extension file number: %i\n",vh->fh2$w_ext_fid.fid$w_num);
	printk("extension file sequence number: %i\n",vh->fh2$w_ext_fid.fid$w_seq);
	printk("extension relative volume number: %i\n",vh->fh2$w_ext_fid.fid$w_rvn);
/* ahem		printk("Moooooooooooooooooo\n"); */
	printk("file characteristics: %i\n",vh->fh2$l_filechar);
	printk("dir: %i\n",((vh->fh2$l_filechar & 0x2000) ? 1 : 0));
	printk("user con. characteristics: %i\n",vh->fh2$l_filechar);
	printk("map words in use: %i\n",vh->fh2$b_map_inuse);
	printk("accessor priv. level: %i\n", vh->fh2$b_acc_mode);
	printk("programmer member number: %i\n",vh->fh2$l_fileowner.uic$w_mem);
	printk("project group number: %i\n",vh->fh2$l_fileowner.uic$w_grp);
	printk("file protection code: %i\n",vh->fh2$w_fileprot);
	//	printk("record protection code: %i\n",vh->h_rpro);

	//	i = vh->h_bfnu[0] + vh->h_bfnu[1]<<8 + vh->h_bfnu[2]<<16 + vh->h_bfnu[3]<<24;
	i=vh->fh2$w_backlink.fid$w_num*65536+vh->fh2$w_backlink.fid$w_seq;
	printk("back link: %i\n",i);
	printk("name: %s\n",id->fi2$t_filename);
	printk("rev #: %i\n",id->fi2$w_revision);
}


/*
 *  Locate the directory whose name is pointed to by "dir"
 */
/*
int finddir() {
        struct header   *hp = &mfdh;
        register char   *p = dir;
        register char   *q;
        char            *strchr();
        int             nch;
        struct filnam   dirfn;
        ushort          dirfnum;
        ushort          search();
        int             gh;

        do {
                for ( q=p; alphnum(*q) || *q == '$'; ++q );
                if ( ( *q && *q!='.' ) || (nch=q-p) == 0 || nch > 39 )
                        err1("Invalid directory ([%s])",dir);
                strncpy(dirfn.f_nam,p,nch);
                dirfn.f_nam[nch] = '\0';
                strcat(dirfn.f_nam,".DIR");
                dirfn.f_ver = 1;
                if ( !(dirfnum=search(hp,&dirfn)) )
                        direrr("Directory [%s] does not exist",dir,q);
                if ( !(gh=gethdr(dirfnum,(hp=(&dirh)))) )
                        direrr("Can't get file header for directory [%s]",dir,q);
                if ( gh == -1 )
                        direrr("No access privilege for directory [%s]",dir,q);
                p = q + 1;
        } while (*q);
        dirfound = 1;
        return(1);
}
*/

/*
 *  Search a directory (identified by dhp) for a filename
 */

/*
ushort
search(dhp,fn)
register struct header  *dhp;
register struct filnam  *fn;
{
        int                             len;
        int                             bod;
        register struct directory       *de;
        struct directory                *getde();
        register struct dirval          *vp;
        register struct dirval          *vplim;

        len = strlen(fn->f_nam);
        for ( bod=1; de=getde(dhp,bod); bod=0 ) {
                if ( de->d_nbc!=len || strncmp(de->d_fname,fn->f_nam,len)!=0 )
                        continue;
                vp = (struct dirval *) ( de->d_fname + ((de->d_nbc+1)&0376) );
                if ( !fn->f_ver )
                        return(vp->d_fid.f_num);
                for ( vplim=(struct dirval *)((char *)(&de->d_vrlm)+de->d_rbc);
vp<vplim; ++vp
 ) {
                        if ( vp->d_ver > fn->f_ver )
                                continue;
                        if ( vp->d_ver == fn->f_ver )
                                return(vp->d_fid.f_num);
                        return(0);
                }
                return(0);
        }
        return(0);
}
*/

/*
 *  List contents of a UFD
 */


/*
listdir()
{
        register int                    bodge;
        register struct directory       *de;
        struct directory                *getde();
        register struct dirval          *vp;
        register struct dirval          *vplim;

        for ( bodge=1; de=getde(&dirh,bodge); bodge=0 ) {
                vp = (struct dirval *) ( de->d_fname + ((de->d_nbc+1)&0376) );
                vplim = (struct dirval *) ((char *)(&de->d_vrlm)+de->d_rbc);
                for ( ; vp<vplim; ++vp )
                        prtfn(de,vp);
        }
}
*/


/*
 *  Return pointer to next directory entry
 */

/*
struct _dir *getde(struct _fh2 *dhp,int bodge) {
#define recsize (*((ushort *)de))
#define STOP    ((ushort)0177777)
        static long             vb;
        static long             eofblk;
        static char             *limit;
        static char             dirbuf[BUFSIZE];
        static char             *de;

        if ( bodge ) {
                vb = 0;
                eofblk = ( (long)dhp->h_rms.f_heof[0] << 16 ) + dhp->h_rms.f_heof[1];
                limit = &dirbuf[BUFSIZE];
        }
        if ( bodge || (de+=(recsize+2))>=limit || recsize==STOP ) {
                if ( ++vb == eofblk )
                        limit = &dirbuf[dhp->h_rms.f_ffby];
                if ( !getvb(vb,dirbuf,dhp) || (*((ushort *)dirbuf)) == STOP)
                        return((struct directory *)0);
                de = dirbuf;
        }
        if ( de >= limit )
                return((struct directory *)0);
        return((struct directory *)de);
}
*/


/*
 *  Routine to get specified virtual block from a file.  Returns 0
 *  on EOF, 1 otherwise.  Note that vbn is 1-based, not 0-based.
 */

getvb(long vbn,char *buf,struct _fh2 *hp,kdev_t dev) {
#define WTPMASK 0140000
#define WTP00   0000000
#define WTP01   0040000
#define WTP10   0100000
#define WTP11   0140000
        register ushort         *rp;
        register long           block;
        register ushort         *limit;
        register ushort         wtype;
        register long           lbn;
        register long           size;
        ushort                  getsize();

        rp = (ushort *)hp + (hp->fh2$b_mpoffset&0377);
        block = 1;
        limit = rp + (hp->fh2$b_map_inuse & 0377);                /* ntw */
        while ( rp <= limit && vbn >= ( block + (size=getsize(rp)) ) ) {
                wtype = (*rp) & WTPMASK;
                switch (wtype) {
                        case WTP00:     rp += 1; break;
                        case WTP01:     rp += 2; break;
                        case WTP10:     rp += 3; break;
                        case WTP11:     rp += 4; break;
                }
                block += size;
        }
	printk("limit: %i(%i)\n",limit,rp);
        if ( rp > limit )
                return(0);
        lbn = lbnbase(rp) + vbn - block;
	printk("getvb: %i,%i\n",vbn,lbn);
        return(getlb(lbn,buf,dev));
}


/*
 *  Return number of blocks mapped by the current window
 */

ushort getsize(ushort *rp) {
        register ushort wtype;

        wtype = (*rp) & WTPMASK;
        switch (wtype) {
                case WTP00:     return(0);
                case WTP01:     return(((*((char *)rp))&0377)+1);
                case WTP10:     return(((*rp)&037777)+1);
                case WTP11:     return(((((long)(*rp)&037777)<<16)+rp[1])+1);
        }
}


/*
 *  Return base lbn mapped by the current window
 */

long lbnbase(ushort *rp) {
        register ushort wtype;

        wtype = (*rp)&WTPMASK;
        switch ( wtype ) {
                case WTP00:     return(0L);
                case WTP01:     return(((((char *)rp)[1]&077L)<<16)+rp[1]);
                case WTP10:     return((((long)rp[2])<<16)+(long)rp[1]);
                case WTP11:     return((((long)rp[3])<<16)+(long)rp[2]);
        }
}


/*
 *  Get block from the filesystem, given the logical block number
 */

int getlb(long lbn,char *buf,kdev_t dev) {
	struct buffer_head *bh;

	if(!(bh = bread(dev,lbn,VMS_BLOCKSIZE))) {
		return(0);
	}

	memcpy(buf,bh->b_data,VMS_BLOCKSIZE);
	brelse(bh);
	return(1);
}


void lgethdr(int bn,struct _fh2 *vh,kdev_t dev)
{
	char buf[VMS_BLOCKSIZE];
	char catchment[VMS_BLOCKSIZE];

	if(!getlb(bn,buf,dev)) {
		printk("VMS_fs: lgethdr: can't read file header.\n");
		return;
	}

	printk("copying %i header bytes\n",sizeof(struct _fh2));
	memcpy(vh,buf,sizeof(struct _fh2));
/*	vms_print_header(vh);*/
}


void vgethdr(int fn,struct _fh2 *vh,struct vms_home_info *vhi,kdev_t dev) {
	char buf[VMS_BLOCKSIZE];
	char catchment[VMS_BLOCKSIZE];
	int bn;

	bn = fn + vhi->ibvb + vhi->ibsz - 1;
	if(!getvb(bn,buf,&vhi->indexh,dev)) {
		printk("VMS_fs: vgethdr: can't read file header.\n");
		return;
	}

	printk("copying %i header bytes\n",sizeof(struct _fh2));
	memcpy(vh,buf,sizeof(struct _fh2));
/*	vms_print_header(vh);*/
}

