/*
 *  linux/fs/ufs/ufs_inode.c
 *
 * Copyright (C) 1996
 * Adrian Rodriguez (adrian@franklins-tower.rutgers.edu)
 * Laboratory for Computer Science Research Computing Facility
 * Rutgers, The State University of New Jersey
 *
 * $Id$
 *
 */

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/locks.h>
#include "vms_fs.h"
#include "vms_fs_sb.h"
#include "vms_fs_i.h"
#include <linux/sched.h>


extern struct inode_operations vms_dir_inode_operations;

/*
void ufs_print_inode(struct inode * inode)
{
	printk("ino %lu  mode 0%6.6o  lk %d  uid %d  gid %d  sz %lu  blks %lu  cnt %u\n",
	       inode->i_ino, inode->i_mode, inode->i_nlink, inode->i_uid, inode->i_gid, inode->i_size, inode->i_blocks, inode->i_count);
	printk("  db <0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x>\n",
	       inode->u.ufs_i.ui_db[0], inode->u.ufs_i.ui_db[1],
	       inode->u.ufs_i.ui_db[2], inode->u.ufs_i.ui_db[3],
	       inode->u.ufs_i.ui_db[4], inode->u.ufs_i.ui_db[5],
	       inode->u.ufs_i.ui_db[6], inode->u.ufs_i.ui_db[7],
	       inode->u.ufs_i.ui_db[8], inode->u.ufs_i.ui_db[9],
	       inode->u.ufs_i.ui_db[10], inode->u.ufs_i.ui_db[11]);
	printk("  gen 0x%8.8x ib <0x%x 0x%x 0x%x>\n",
	       inode->u.ufs_i.ui_gen, inode->u.ufs_i.ui_ib[0],
	       inode->u.ufs_i.ui_ib[1], inode->u.ufs_i.ui_ib[2]);
}
*/


int vms_is_dir(struct _fh2 *vh) {
  return(vh->fh2$l_filechar & FH2$M_DIRECTORY);
}


void vms_read_inode(struct inode * inode)
{
	int bn;
	char buf[VMS_BLOCKSIZE];
	char catchment[VMS_BLOCKSIZE];
	struct super_block *s;
	struct vms_home_info *vhi;
	struct _fh2 fileh;
	struct vms_inode_info *vii;

	inode->i_op = 0;
	inode->i_mode = 0;

	printk("vms_read_inode: reading inode %i\n",inode->i_ino);

	s = inode->i_sb;
	vhi = (struct vms_home_info *)s->u.generic_sbp;
	/* XXX check the returned header */
	if(!vgethdr(inode->i_ino,&fileh,vhi,s->s_dev)) {
		printk("vms_read_inode: can't read file header.\n");
		return(-EACCES);
	}

	if(!inode->u.generic_ip) {
		/* XXX hmm should prolly clean this up afterwards :) */
		vii = (struct vms_inode_info *) __get_free_page(GFP_KERNEL);
		if(!vii) {
			printk("vms_read_inode: get_free_page() failed\n");
			return(0);
		}
		inode->u.generic_ip = vii;
		memcpy(&vii->header,&fileh,sizeof(struct _fh2));
	}

	inode->i_uid = fileh.fh2$l_fileowner.uic$w_mem;
	inode->i_gid = fileh.fh2$l_fileowner.uic$w_grp;
	inode->i_mode = 0755;
	if(vms_is_dir(&fileh)) {
		inode->i_mode |= S_IFDIR;
		inode->i_op = (struct inode_operations *) &vms_dir_inode_operations;
		inode->i_size = VMS_BLOCKSIZE * fileh.fh2$b_map_inuse;
		inode->i_nlink = 1;
	} else {
		inode->i_mode |= S_IFREG;
/*		inode->i_op = (struct inode_operations *) &vms_file_ops;*/
		inode->i_nlink = 1;
		inode->i_blksize = VMS_BLOCKSIZE;
	}
/*	inode->i_op = */
}

void vms_put_inode (struct inode * inode)
{
	if (inode->i_nlink)
	        return;

	if(inode->u.generic_ip) free_page(inode->u.generic_ip);
/*
	printk("ufs_put_inode: nlink == 0 for inum %lu on dev %d/%d\n",
	       inode->i_ino, MAJOR(inode->i_dev), MINOR(inode->i_dev));
	ufs_print_inode(inode);
	panic("ufs_put_inode: fs is read only, and nlink == 0");

	/* XXX - this code goes here eventually
	inode->i_size = 0;
	if (inode->i_blocks)
	        ufs_truncate(inode);
	ufs_free_inode(inode);
	*/

	return;
}
