/*
 * vms dir.c
 */

#include <linux/fs.h>
#include <asm/byteorder.h>
#include "vms_fs.h"
#include "vms_fs_sb.h"
#include "vms_fs_i.h"

/* XXX */
extern int vms_lookup (struct inode *, const char *, int, struct inode **);
extern int vms_bmap (struct inode *, int);
extern void vms_print_inode (struct inode *);

/*
 * this is fucking (can I say fucking in the kernel?) messy
 */
static int
vms_readdir (struct inode * inode, struct file * filp, void * dirent,
	     filldir_t filldir)
{
#define recsize (*((ushort *)de))
#define STOP	((ushort)0177777)

	int offset,fpos,block,ino,entries,i;
	int eofblk;
	struct super_block *s;
	struct _dir *vmd;
	struct vms_home_info *vhi;
	struct vms_inode_info *vii;
	struct _fh2 *dhp,*dh;
	char *dirbuf[VMS_BLOCKSIZE];

	printk(". (%i) ",filp->f_pos);
        if (!inode || !inode->i_sb || !S_ISDIR(inode->i_mode))
                return -EBADF;
	s = inode->i_sb;

 	vhi = (struct vms_home_info *)s->u.generic_sbp;
	dhp = &vhi->indexh;

	vii = (struct vms_inode_info *)inode->u.generic_ip;
	dh = &vii->header;

	offset = filp->f_pos & (VMS_BLOCKSIZE-1);

	entries = 0;

	if(filp->f_pos == 0) {
		if(filldir(dirent,".",1,0,inode->i_ino,DT_DIR) < 0) {
			return(0);
		}
		filp->f_pos++;
		entries++;
	}
	if(filp->f_pos == 1) {
		if(filldir(dirent,"..",2,1,VMS_ROOT_INO,DT_DIR) < 0) {
			return(0);
		}
		filp->f_pos++;
		entries++;
	}

	//	eofblk = (dh->fh2$w_recattr.f_heof[0]<<16) + dh->fh2$w_recattr.f_heof[1];
	eofblk = (dh->fh2$w_recattr.fat$l_efblk);

	fpos = filp->f_pos - 2;
	block = fpos / VMS_BLOCKSIZE + 1;
	offset = fpos & (VMS_BLOCKSIZE-1);

	printk("starting block: %i (%i,%i)\n",block,fpos,offset);

	while(block <= eofblk) {	/* XXX is this right? */
		struct _dir1 *dv;
		int nextblock;

		if(!getvb(block,dirbuf,dh,s->s_dev)) {
			printk("getvb returned error\n");
			return(0);
		}

		nextblock = 0;
		fpos = (block-1)*VMS_BLOCKSIZE;

		while(!nextblock) {
			vmd = (struct _dir *)(&((char *)dirbuf)[offset]);
			if(vmd->dir$w_size == (ushort)-1) {
				block++;
				offset = 0;
				printk("whopping\n");
				nextblock = 1;
				continue;
			}

			if(vmd->dir$w_size == 0) {
				filp->f_pos = fpos + 2;
				return(entries);
			}

/*
			printk("%p[%p]:%p:%p %i{\n",dirbuf,offset,vmd,vmd->dir$w_size,fpos);
			for(i=0;i<35;i++) {
				printk("%c",((char *)vmd)[i]);
			}
			printk("\n}\n");
*/

			dv = (struct _dir1 *)(vmd->dir$t_name + vmd->dir$b_namecount);
			ino = dv->dir$fid.fid$w_num;
/*			printk("%s\n",vmd->dir$t_name);*/
			filp->f_pos = fpos + 2;
			if(filldir(dirent,vmd->dir$t_name,vmd->dir$b_namecount,filp->f_pos,ino,DT_DIR) < 0) {
				return(0);
			}

			offset += vmd->dir$w_size+2;
			fpos += vmd->dir$w_size+2;
			entries++;

/* shouldn't need this..
			if(offset >= VMS_BLOCKSIZE) {
				block++;
				offset -= VMS_BLOCKSIZE;
				nextblock = 1;
			}
*/
		}
	}

	return(entries);
}

static struct file_operations vms_dir_operations = {
	NULL,			/* lseek */
	NULL,			/* read */
	NULL,			/* write */
	vms_readdir,		/* readdir */
	NULL,			/* select */
	NULL,			/* ioctl */
	NULL,			/* mmap */
	NULL,			/* open */
	NULL,			/* release */
	file_fsync,		/* fsync */
	NULL,			/* fasync */
	NULL,			/* check_media_change */
	NULL,			/* revalidate */
};

struct inode_operations vms_dir_inode_operations = {
	&vms_dir_operations,	/* default directory file operations */
	NULL,			/* create */
	vms_lookup,		/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	NULL,			/* readpage */
	NULL,			/* writepage */
	NULL,			/* bmap */
	NULL,			/* truncate */
	NULL,			/* permission */
	NULL,			/* smap */
};
