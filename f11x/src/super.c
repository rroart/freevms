/*
 * vms super.c
 *
 * a filesystem is a thing of beauty
 * well .. except when it's DEC FILES11 (aka ODS2)
 */


#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include "vms_fs.h"
#include "vms_fs_sb.h"
#include <linux/locks.h>

#include <asm/segment.h>

struct super_block * vms_read_super(struct super_block * s, void * data, int silent); 
void vms_put_super (struct super_block * s);
void vms_statfs(struct super_block * s, struct statfs * buf, int bufsize);

extern void vms_read_inode(struct inode * inode);
extern void vms_put_inode(struct inode * inode);

static struct super_operations vms_super_ops = {
	vms_read_inode,
	NULL,			/* notify_change() */
	NULL,			/* XXX - vms_write_inode() */
	vms_put_inode,
	vms_put_super,
	NULL,			/* XXX - vms_write_super() */
	vms_statfs,
	NULL,			/* XXX - vms_remount() */
};

static DECLARE_FSTYPE_DEV(vms_fs_type, "ods2", vms_read_super);

#if 0
static struct file_system_type vms_fs_type = {
	vms_read_super, "vms", 1, NULL
};
#endif 

int __init init_vms_fs(void) {
	return(register_filesystem(&vms_fs_type));
}

#ifdef MODULE
int init_module(void)
{
	int status;

	if ((status = init_vms_fs()) == 0)
		register_symtab(0);
	return status;
}

void cleanup_module(void)
{
	unregister_filesystem(&vms_fs_type);
}
#endif

#if 0 /* unused */
static void
ufs_print_super_stuff(struct super_block * s, struct ufs_superblock * usb)
{

	printk("fs_sblkno: 0x%8.8x\n", usb->fs_sblkno);
	printk("fs_size: 0x%8.8x\n", usb->fs_size);
	printk("fs_ncg: 0x%8.8x\n", usb->fs_ncg);
	printk("fs_bsize: 0x%8.8x\n", usb->fs_bsize);
	printk("fs_frag: 0x%8.8x\n", usb->fs_frag);
	printk("fs_nindir: 0x%8.8x\n", usb->fs_nindir);
	printk("fs_inopb: 0x%8.8x\n", usb->fs_inopb);
	printk("fs_optim: 0x%8.8x\n", usb->fs_optim);
	printk("fs_ncyl: 0x%8.8x\n", usb->fs_ncyl);
	printk("fs_state: 0x%8.8x\n", usb->fs_state);
	printk("fs_magic: 0x%8.8x\n", usb->fs_magic);
	printk("fs_fsmnt: `%s'\n", usb->fs_fsmnt);

	return;
}
#endif

struct super_block *
vms_read_super(struct super_block * s, void * data, int silent)
{
	struct _hm2 *vhb;
	struct vms_home_info *vhi;
	struct _fh2 idx;
	struct buffer_head *bh;
	kdev_t dev = s->s_dev;
	int ifhbn;

	MOD_INC_USE_COUNT;
	lock_super(s);
	set_blocksize(dev,VMS_BLOCKSIZE);

	if(!(bh = bread(dev,1,VMS_BLOCKSIZE))) {
                s->s_dev = 0;
                unlock_super(s);
		if(!silent) printk("VMS-fs: unable to read *home*block\n");
                MOD_DEC_USE_COUNT;
                return NULL;
        }
 
	vhb = (struct _hm2 *) bh->b_data;

/* !  free this */
	vhi = (struct vms_home_info *) __get_free_page(GFP_KERNEL);
	if(vhi == NULL) {
		s->s_dev = 0;
		unlock_super(s);
		printk("vms_read_super: get_free_page() failed\n");
                MOD_DEC_USE_COUNT;
                return NULL;
	}

	if(vhb->hm2$l_homelbn != 1 || strncmp(vhb->hm2$t_format,"DECFILE11",9)) {
/*		printk("VMS-fs: [%s]\n",vhb->hm2$t_format);*/
		s->s_dev = 0;
                unlock_super(s);
                brelse(bh);
                if (!silent)
                        printk("VFS: Can't find a FILES11B filesystem on dev "
				"%s.\n", kdevname(dev));
                MOD_DEC_USE_COUNT;
                return NULL;
 	}
	/* XXX - make everything read only for testing */
	s->s_flags |= MS_RDONLY;
	s->s_blocksize = 512;
	s->s_blocksize_bits = 9;
	s->s_magic = VMS_SUPER_MAGIC;
	s->u.generic_sbp = vhi;
	((struct vms_home_info *)s->u.generic_sbp)->ibvb = vhb->hm2$w_ibmapvbn;
	((struct vms_home_info *)s->u.generic_sbp)->ibsz = vhb->hm2$w_ibmapsize;

	//	ifhbn = ((long)vhb->hm2$l_ibmaplbn[1]<<16) + ((long)vhb->hm2$l_ibmaplbn[0]) + vhb->hm2$w_ibmapsize;
	ifhbn = vhb->hm2$l_ibmaplbn + vhb->hm2$w_ibmapsize;
	printk("index file header: %i\n",ifhbn);
	lgethdr(ifhbn,&idx,s->s_dev);
	memcpy(&vhi->indexh,&idx,sizeof(struct _fh2));
	lgethdr(ifhbn+3,&idx,s->s_dev);
	memcpy(&vhi->mfdh,&idx,sizeof(struct _fh2));

	((struct vms_home_info *)s->u.generic_sbp)->vhb = vhb;
	s->dq_op = 0;
	s->s_op = &vms_super_ops;
//	s->s_mounted = iget(s,VMS_ROOT_INO);

	if(!silent) printk("VMS-fs: phwee isn't VMS fun...\n");
	unlock_super(s);
	return(s);
}

void vms_put_super (struct super_block * s)
{

/*	printk("vms_put_super\n"); /* XXX */

	lock_super (s);
	/* XXX - sync fs data, set state to ok, and flush buffers */
	s->s_dev = 0;

	/* XXX - free allocated kernel memory */

	unlock_super (s);
	MOD_DEC_USE_COUNT;

	return;
}

void vms_statfs(struct super_block * s, struct statfs * buf, int bufsiz)
{
	struct statfs tmp;

/*	printk("vms_statfs\n"); /* XXX */
	tmp.f_type = s->s_magic;
	tmp.f_bsize = PAGE_SIZE;
	tmp.f_blocks = 0;
	tmp.f_bfree = 69;
	tmp.f_bavail = 0;
	tmp.f_files = 1;
	tmp.f_ffree = 0;
/*	tmp.f_blocks = sb->u.ufs_sb.s_raw_sb->fs_dsize;
	tmp.f_bfree = sb->u.ufs_sb.s_raw_sb->fs_cstotal.cs_nbfree;
	tmp.f_bavail =  sb->u.ufs_sb.s_raw_sb->fs_cstotal.cs_nbfree; 
	tmp.f_files = sb->u.ufs_sb.s_ncg * sb->u.ufs_sb.s_ipg;
	tmp.f_ffree = sb->u.ufs_sb.s_raw_sb->fs_cstotal.cs_nifree;
	tmp.f_fsid.val[0] = sb->u.ufs_sb.s_raw_sb->fs_id[0];
	tmp.f_fsid.val[1] = sb->u.ufs_sb.s_raw_sb->fs_id[1];*/
	tmp.f_namelen = VMS_MAXNAMLEN;
/*        tmp.f_spare[6] */

	memcpy(buf, &tmp, bufsiz);

	return;
}

EXPORT_NO_SYMBOLS;

module_init(init_vms_fs)
