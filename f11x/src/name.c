/*
 * vms name.c
 */

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/ctype.h>
#include "vms_fs.h"
#include "vms_fs_i.h"

/*extern unsigned int vms_bmap(struct inode * inode, int block); /* XXX */

/*
 * NOTE! unlike strncmp, ext2_match returns 1 for success, 0 for failure.
 * stolen from ext2fs
 */
static int vms_match (int len, const char * const name, struct _dir * d) {
	int i;

	if (!d || len > VMS_MAXNAMLEN)
		return 0;
	/*
	 * "" means "." ---> so paths like "/usr/lib//libc.a" work
	 */
	if (!len && (d->dir$b_namecount == 1) && (d->dir$t_name[0] == '.') &&
	   (d->dir$t_name[1] == '\0'))
		return 1;
	if (len != d->dir$b_namecount)
		return 0;

	for(i=0;i<len;i++) {
		if(toupper(name[i]) != toupper(d->dir$t_name)) return(0);
	}
	return(1);
}

/* XXX - this is a mess, especially for endianity */
int vms_lookup (struct inode * dir, const char * name, int len,
	        struct inode ** result)
{
	int offset,block,ino,eofblk,fpos;
	struct super_block *s;
	struct _dir *vmd;
	struct vms_inode_info *vii;
	struct _fh2 *dh;
	char dirbuf[VMS_BLOCKSIZE];

	s = dir->i_sb;

	printk("vms_lookup: '%s' - ",name);

	*result = NULL;

	if(!dir || !S_ISDIR(dir->i_mode)) return(-ENOENT);

	if(name[0] == '.') {
		if(name[1] == 0) {
			printk(".\n");
			if(!(*result = iget(s,dir->i_ino))) {
				printk("fuck\n");
				iput(dir);
				return(-EACCES);
			}
			return(0);
		}
		if(name[1] == '.' && name[2] == 0) {
			printk("..\n");
			if(!(*result = iget(s,VMS_ROOT_INO))) {
				printk("fuck\n");
				iput(dir);
				return(-EACCES);
			}
			return(0);
		}
	}

	vii = (struct vms_inode_info *)dir->u.generic_ip;
	dh = &vii->header;

	//	eofblk = (dh->h_rms.f_hvbn[0]<<16) + dh->h_rms.f_hvbn[1];
	eofblk = (dh->fh2$w_recattr.fat$l_efblk);
	fpos = 0;
	block = 1;
	offset = 0;

	while(block <= eofblk) {	/* XXX is this right? */
		struct _dir1 *dv;
		int nextblock;

		if(!getvb(block,dirbuf,dh,s->s_dev)) {
			printk("vms_lookup: getvb returned error\n");
			return(0);
		}

		nextblock = 0;
		fpos = (block-1)*VMS_BLOCKSIZE;

		while(!nextblock) {
			vmd = (struct _dir *)(&((char *)dirbuf)[offset]);
			if(vmd->dir$w_size == (ushort)-1) {
				block++;
				offset = 0;
				nextblock = 1;
				continue;
			}

			if(vmd->dir$w_size == 0) {
				return(-ENOENT);
			}

			dv = (struct _dir1 *)(vmd->dir$t_name + vmd->dir$b_namecount);
			ino = dv->dir$fid.fid$w_num;
			if(len == vmd->dir$b_namecount) {
				if(vms_match(len,name,vmd)) {
					*result = iget(dir->i_sb,dv->dir$fid.fid$w_num);
					return(0);
				}
			}

			offset += vmd->dir$w_size+2;
			fpos += vmd->dir$w_size+2;
		}
	}

	printk("'%s'?\n",name);
	iput(dir);
	return(-ENOENT);
}


