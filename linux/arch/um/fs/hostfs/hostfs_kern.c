/* 
 * Copyright (C) 2000, 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <linux/stddef.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/blkdev.h>
#include <asm/uaccess.h>
#include "hostfs.h"
#include "kern_util.h"
#include "kern.h"
#include "user_util.h"
#include "2_5compat.h"

#define file_hostfs_i(file) (&(file)->f_dentry->d_inode->u.hostfs_i)

int hostfs_d_delete(struct dentry *dentry)
{
	return(1);
}

struct dentry_operations hostfs_dentry_ops = {
	d_delete:		hostfs_d_delete,
};

static char *root_ino = "/";

#define HOSTFS_SUPER_MAGIC 0x00c0ffee

static struct inode_operations hostfs_iops;
static struct address_space_operations hostfs_link_aops;

static char *dentry_name(struct dentry *dentry, int extra)
{
	struct dentry *parent;
	char *root, *name;
	int len;

	len = 0;
	parent = dentry;
	while(parent->d_parent != parent){
		len += parent->d_name.len + 1;
		parent = parent->d_parent;
	}
	
	root = parent->d_inode->u.hostfs_i.host_filename;
	len += strlen(root);
	name = kmalloc(len + extra + 1, GFP_KERNEL);
	if(name == NULL) return(NULL);

	name[len] = '\0';
	parent = dentry;
	while(parent->d_parent != parent){
		len -= parent->d_name.len + 1;
		name[len] = '/';
		strncpy(&name[len + 1], parent->d_name.name, 
			parent->d_name.len);
		parent = parent->d_parent;
	}
	strncpy(name, root, strlen(root));
	return(name);
}

static char *inode_name(struct inode *ino, int extra)
{
	struct dentry *dentry;

	dentry = list_entry(ino->i_dentry.next, struct dentry, d_alias);
	return(dentry_name(dentry, extra));
}

static int read_name(struct inode *ino, char *name)
{
	/* The non-int inode fields are copied into ints by stat_file and
	 * then copied into the inode because passing the actual pointers
	 * in and having them treated as int * breaks on big-endian machines
	 */
	int err;
	int i_dev, i_mode, i_nlink, i_blksize;
	unsigned long long i_size;
	unsigned long long i_ino;
	unsigned long long i_blocks;
	err = stat_file(name, &i_dev, &i_ino, &i_mode, &i_nlink, 
			&ino->i_uid, &ino->i_gid, &i_size, &ino->i_atime, 
			&ino->i_mtime, &ino->i_ctime, &i_blksize, &i_blocks);
	if(err) return(err);
	ino->i_ino = i_ino;
	ino->i_dev = i_dev;
	ino->i_mode = i_mode;
	ino->i_nlink = i_nlink;
	ino->i_size = i_size;
	ino->i_blksize = i_blksize;
	ino->i_blocks = i_blocks;
	if(kdev_same(ino->i_sb->s_dev, ROOT_DEV) && (ino->i_uid == getuid()))
		ino->i_uid = 0;
	return(0);
}

static int read_inode(struct inode *ino)
{
	char *name;
	int err;

	name = inode_name(ino, 0);
	if(name == NULL) return(-ENOMEM);
	err = read_name(ino, name);
	kfree(name);
	return(err);
}

void hostfs_delete_inode(struct inode *ino)
{
	if(ino->u.hostfs_i.host_filename) kfree(ino->u.hostfs_i.host_filename);
	ino->u.hostfs_i.host_filename = NULL;
	if(ino->u.hostfs_i.fd != -1) close_file(&ino->u.hostfs_i.fd);
	ino->u.hostfs_i.mode = 0;
	clear_inode(ino);
}

int hostfs_statfs(struct super_block *sb, struct statfs *sf)
{
	/* do_statfs uses struct statfs64 internally, but the linux kernel
	 * struct statfs still has 32-bit versions for most of these fields,
	 * so we convert them here
	 */
	int err;
	long long f_blocks;
	long long f_bfree;
	long long f_bavail;
	long long f_files;
	long long f_ffree;

	err = do_statfs(sb->s_root->d_inode->u.hostfs_i.host_filename,
			&sf->f_bsize, &f_blocks, &f_bfree, &f_bavail, &f_files,
			&f_ffree, &sf->f_fsid, sizeof(sf->f_fsid), 
			&sf->f_namelen, sf->f_spare);
	if(err) return(err);
	sf->f_blocks = f_blocks;
	sf->f_bfree = f_bfree;
	sf->f_bavail = f_bavail;
	sf->f_files = f_files;
	sf->f_ffree = f_ffree;
	sf->f_type = HOSTFS_SUPER_MAGIC;
	return(0);
}

static struct super_operations hostfs_sbops = { 
	put_inode:	force_delete,
	delete_inode:	hostfs_delete_inode,
	statfs:		hostfs_statfs,
};

int hostfs_readdir(struct file *file, void *ent, filldir_t filldir)
{
	void *dir;
	char *name;
	unsigned long long next, ino;
	int error, len;

	name = dentry_name(file->f_dentry, 0);
	if(name == NULL) return(-ENOMEM);
	dir = open_dir(name, &error);
	kfree(name);
	if(dir == NULL) return(-error);
	next = file->f_pos;
	while((name = read_dir(dir, &next, &ino, &len)) != NULL){
		error = (*filldir)(ent, name, len, file->f_pos, 
				   ino, DT_UNKNOWN);
		if(error) break;
		file->f_pos = next;
	}
	close_dir(dir);
	return(0);
}

int hostfs_file_open(struct inode *ino, struct file *file)
{
	char *name;
	int mode = 0, r = 0, w = 0, fd;

	mode = file->f_mode & (FMODE_READ | FMODE_WRITE);
	if((mode & ino->u.hostfs_i.mode) == mode) return(0);

	if(ino->u.hostfs_i.fd != -1){
		close_file(&ino->u.hostfs_i.fd);
		ino->u.hostfs_i.fd = -1;
	}
	ino->u.hostfs_i.mode |= mode;
	if(ino->u.hostfs_i.mode & FMODE_READ) r = 1;
	if(ino->u.hostfs_i.mode & FMODE_WRITE) w = 1;
	if(w) r = 1;
	name = dentry_name(file->f_dentry, 0);
	if(name == NULL) return(-ENOMEM);
	fd = open_file(name, r, w);
	kfree(name);
	if(fd < 0) return(fd);
	file_hostfs_i(file)->fd = fd;
	return(0);
}

int hostfs_dir_open(struct inode *ino, struct file *file)
{
	return(0);	
}

int hostfs_dir_release(struct inode *ino, struct file *file)
{
	return(0);
}

int hostfs_fsync(struct file *file, struct dentry *dentry, int datasync)
{
	return(0);
}

static struct file_operations hostfs_file_fops = {
	owner:		NULL,
	read:		generic_file_read,
	write:		generic_file_write,
	mmap:		generic_file_mmap,
	open:		hostfs_file_open,
	release:	NULL,
	fsync:		hostfs_fsync,
};

static struct file_operations hostfs_dir_fops = {
	owner:		NULL,
	readdir:	hostfs_readdir,
	open:		hostfs_dir_open,
	release:	hostfs_dir_release,
	fsync:		hostfs_fsync,
};

int hostfs_writepage(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	char *buffer;
	unsigned long long base;
	int count = PAGE_CACHE_SIZE;
	int end_index = inode->i_size >> PAGE_CACHE_SHIFT;
	int err;

	if (page->index >= end_index)
		count = inode->i_size & (PAGE_CACHE_SIZE-1);

	buffer = kmap(page);
	base = ((unsigned long long) page->index) << PAGE_CACHE_SHIFT;

	err = write_file(inode->u.hostfs_i.fd, &base, buffer, count);
	if(err != count){
		ClearPageUptodate(page);
		goto out;
	}

	if (base > inode->i_size)
		inode->i_size = base;

	if (PageError(page))
		ClearPageError(page);	
	err = 0;

 out:	
	kunmap(page);

	UnlockPage(page);
	return err; 
}

int hostfs_readpage(struct file *file, struct page *page)
{
	char *buffer;
	long long start;
	int err = 0;

	start = (long long) page->index << PAGE_CACHE_SHIFT;
	buffer = kmap(page);
	err = read_file(file_hostfs_i(file)->fd, &start, buffer,
			PAGE_CACHE_SIZE);
	if(err < 0) goto out;

	flush_dcache_page(page);
	SetPageUptodate(page);
	if (PageError(page)) ClearPageError(page);
	err = 0;
 out:
	kunmap(page);
	UnlockPage(page);
	return(err);
}

int hostfs_prepare_write(struct file *file, struct page *page, 
			 unsigned int from, unsigned int to)
{
	char *buffer;
	long long start, tmp;
	int err;

	start = (long long) page->index << PAGE_CACHE_SHIFT;
	buffer = kmap(page);
	if(from != 0){
		tmp = start;
		err = read_file(file_hostfs_i(file)->fd, &tmp, buffer,
				from);
		if(err < 0) goto out;
	}
	if(to != PAGE_CACHE_SIZE){
		start += to;
		err = read_file(file_hostfs_i(file)->fd, &start, buffer + to,
				PAGE_CACHE_SIZE - to);
		if(err < 0) goto out;		
	}
	err = 0;
 out:
	kunmap(page);
	return(err);
}

int hostfs_commit_write(struct file *file, struct page *page, unsigned from,
		 unsigned to)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	char *buffer;
	long long start;
	int err = 0;

	start = (long long) (page->index << PAGE_CACHE_SHIFT) + from;
	buffer = kmap(page);
	err = write_file(file_hostfs_i(file)->fd, &start, buffer + from, 
			 to - from);
	if(err > 0) err = 0;
	if(!err && (start > inode->i_size))
		inode->i_size = start;

	kunmap(page);
	return(err);
}

static struct address_space_operations hostfs_aops = {
	writepage: 	hostfs_writepage,
	readpage:	hostfs_readpage,
	prepare_write:	hostfs_prepare_write,
	commit_write:	hostfs_commit_write
};

static struct inode *get_inode(struct super_block *sb, struct dentry *dentry,
			       int *error)
{
	struct inode *inode;
	char *name;
	int type, err = 0, rdev;

	inode = get_empty_inode();
	if(inode == NULL) return(NULL);
	inode->u.hostfs_i.host_filename = NULL;
	inode->u.hostfs_i.fd = -1;
	inode->u.hostfs_i.mode = 0;
	if(error) *error = 0;
	insert_inode_hash(inode);
	if(dentry){
		name = dentry_name(dentry, 0);
		if(name == NULL){
			err = -ENOMEM;
			goto out;
		}
		type = file_type(name, &rdev);
		kfree(name);
	}
	else type = HOSTFS_DIR;
	inode->i_sb = sb;

	if(type == HOSTFS_SYMLINK)
		inode->i_op = &page_symlink_inode_operations;
	else inode->i_op = &hostfs_iops;

	if(type == HOSTFS_DIR) inode->i_fop = &hostfs_dir_fops;
	else inode->i_fop = &hostfs_file_fops;

	if(type == HOSTFS_SYMLINK) inode->i_mapping->a_ops = &hostfs_link_aops;
	else inode->i_mapping->a_ops = &hostfs_aops;

	switch (type) {
	case HOSTFS_CHARDEV:
		init_special_inode(inode, S_IFCHR, rdev);
		break;
	case HOSTFS_BLOCDEV:
		init_special_inode(inode, S_IFBLK, rdev);
		break;
	case HOSTFS_FIFO:
		init_special_inode(inode, S_IFIFO, 0);
		break;
	case HOSTFS_SOCK:
		init_special_inode(inode, S_IFSOCK, 0);
		break;
	}
	
	return(inode);
 out:
	iput(inode);
	if(error) *error = err;
	return(NULL);
}

int hostfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;
	char *name;
	int error;

	inode = get_inode(dir->i_sb, dentry, &error);
	if(error) return(error);
	name = dentry_name(dentry, 0);
	if(name == NULL){
		iput(inode);
		return(-ENOMEM);
	}
	error = file_create(name, 
			    mode | S_IRUSR, mode | S_IWUSR, mode | S_IXUSR, 
			    mode | S_IRGRP, mode | S_IWGRP, mode | S_IXGRP, 
			    mode | S_IROTH, mode | S_IWOTH, mode | S_IXOTH);
	if(!error) error = read_name(inode, name);
	kfree(name);
	if(error){
		iput(inode);
		return(error);
	}
	d_instantiate(dentry, inode);
	return(0);
}
 
struct dentry *hostfs_lookup(struct inode *ino, struct dentry *dentry)
{
	struct inode *inode;
	char *name;
	int error;

	inode = get_inode(ino->i_sb, dentry, &error);
	if(error != 0) return(ERR_PTR(error));
	name = dentry_name(dentry, 0);
	if(name == NULL) return(ERR_PTR(-ENOMEM));
	error = read_name(inode, name);
	kfree(name);
	if(error){
		iput(inode);
		if(error == -ENOENT) inode = NULL;
		else return(ERR_PTR(error));
	}
	d_add(dentry, inode);
	dentry->d_op = &hostfs_dentry_ops;
	return(NULL);
}

static char *inode_dentry_name(struct inode *ino, struct dentry *dentry)
{
        char *file;
	int len;

	file = inode_name(ino, dentry->d_name.len + 1);
	if(file == NULL) return(NULL);
        strcat(file, "/");
	len = strlen(file);
        strncat(file, dentry->d_name.name, dentry->d_name.len);
	file[len + dentry->d_name.len] = '\0';
        return(file);
}

int hostfs_link(struct dentry *to, struct inode *ino, struct dentry *from)
{
        char *from_name, *to_name;
        int err;

        if((from_name = inode_dentry_name(ino, from)) == NULL) 
                return(-ENOMEM);
        to_name = dentry_name(to, 0);
	if(to_name == NULL){
		kfree(from_name);
		return(-ENOMEM);
	}
        err = link_file(to_name, from_name);
        kfree(from_name);
        kfree(to_name);
        return(err);
}

int hostfs_unlink(struct inode *ino, struct dentry *dentry)
{
	char *file;
	int err;

	if((file = inode_dentry_name(ino, dentry)) == NULL) return(-ENOMEM);
	err = unlink_file(file);
	kfree(file);
	return(err);
}

int hostfs_symlink(struct inode *ino, struct dentry *dentry, const char *to)
{
	char *file;
	int err;

	if((file = inode_dentry_name(ino, dentry)) == NULL) return(-ENOMEM);
	err = make_symlink(file, to);
	kfree(file);
	return(err);
}

int hostfs_mkdir(struct inode *ino, struct dentry *dentry, int mode)
{
	char *file;
	int err;

	if((file = inode_dentry_name(ino, dentry)) == NULL) return(-ENOMEM);
	err = do_mkdir(file, mode);
	kfree(file);
	return(err);
}

int hostfs_rmdir(struct inode *ino, struct dentry *dentry)
{
	char *file;
	int err;

	if((file = inode_dentry_name(ino, dentry)) == NULL) return(-ENOMEM);
	err = do_rmdir(file);
	kfree(file);
	return(err);
}

int hostfs_mknod(struct inode *ino, struct dentry *dentry, int mode, int dev)
{
	char *file;
	int err;

	if((file = inode_dentry_name(ino, dentry)) == NULL) return(-ENOMEM);
	err = do_mknod(file, mode, dev);
	kfree(file);
	return(err);
}

int hostfs_rename(struct inode *from_ino, struct dentry *from,
		  struct inode *to_ino, struct dentry *to)
{
	char *from_name, *to_name;
	int err;

	if((from_name = inode_dentry_name(from_ino, from)) == NULL)
		return(-ENOMEM);
	if((to_name = inode_dentry_name(to_ino, to)) == NULL){
		kfree(from_name);
		return(-ENOMEM);
	}
	err = rename_file(from_name, to_name);
	kfree(from_name);
	kfree(to_name);
	return(err);
}

void hostfs_truncate(struct inode *ino)
{
	not_implemented();
}

int hostfs_permission(struct inode *ino, int desired)
{
	char *name;
	int r = 0, w = 0, x = 0, err;

	if(desired & MAY_READ) r = 1;
	if(desired & MAY_WRITE) w = 1;
	if(desired & MAY_EXEC) x = 1;
	name = inode_name(ino, 0);
	if(name == NULL) return(-ENOMEM);
	err = access_file(name, r, w, x);
	kfree(name);
	if(!err) err = vfs_permission(ino, desired);
	return(err);
}

int hostfs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct hostfs_iattr attrs;
	char *name;
	int err;

	attrs.ia_valid = 0;
	if(attr->ia_valid & ATTR_MODE){
		attrs.ia_valid |= HOSTFS_ATTR_MODE;
		attrs.ia_mode = attr->ia_mode;
	}
	if(attr->ia_valid & ATTR_UID){
		attrs.ia_valid |= HOSTFS_ATTR_UID;
		attrs.ia_uid = attr->ia_uid;
	}
	if(attr->ia_valid & ATTR_GID){
		attrs.ia_valid |= HOSTFS_ATTR_GID;
		attrs.ia_gid = attr->ia_gid;
	}
	if(attr->ia_valid & ATTR_SIZE){
		attrs.ia_valid |= HOSTFS_ATTR_SIZE;
		attrs.ia_size = attr->ia_size;
	}
	if(attr->ia_valid & ATTR_ATIME){
		attrs.ia_valid |= HOSTFS_ATTR_ATIME;
		attrs.ia_atime = attr->ia_atime;
	}
	if(attr->ia_valid & ATTR_MTIME){
		attrs.ia_valid |= HOSTFS_ATTR_MTIME;
		attrs.ia_mtime = attr->ia_mtime;
	}
	if(attr->ia_valid & ATTR_CTIME){
		attrs.ia_valid |= HOSTFS_ATTR_CTIME;
		attrs.ia_ctime = attr->ia_ctime;
	}
	if(attr->ia_valid & ATTR_ATIME_SET){
		attrs.ia_valid |= HOSTFS_ATTR_ATIME_SET;
	}
	if(attr->ia_valid & ATTR_MTIME_SET){
		attrs.ia_valid |= HOSTFS_ATTR_MTIME_SET;
	}
	name = dentry_name(dentry, 0);
	if(name == NULL) return(-ENOMEM);
	err = set_attr(name, &attrs);
	kfree(name);
	return(err);
}

int hostfs_getattr(struct dentry *dentry, struct iattr *attr)
{
	not_implemented();
	return(-EINVAL);
}

static struct inode_operations hostfs_iops = {
	create:		hostfs_create,
	lookup:		hostfs_lookup,
	link:		hostfs_link,
	unlink:		hostfs_unlink,
	symlink:	hostfs_symlink,
	mkdir:		hostfs_mkdir,
	rmdir:		hostfs_rmdir,
	mknod:		hostfs_mknod,
	rename:		hostfs_rename,
	truncate:	hostfs_truncate,
	permission:	hostfs_permission,
	setattr:	hostfs_setattr,
	getattr:	hostfs_getattr,
};

int hostfs_link_readpage(struct file *file, struct page *page)
{
	char *buffer, *name;
	long long start;
	int err;

	start = page->index << PAGE_CACHE_SHIFT;
	buffer = kmap(page);
	name = inode_name(page->mapping->host, 0);
	if(name == NULL) return(-ENOMEM);
	err = do_readlink(name, buffer, PAGE_CACHE_SIZE);
	kfree(name);
	if(err == 0){
		flush_dcache_page(page);
		SetPageUptodate(page);
		if (PageError(page)) ClearPageError(page);
	}
	kunmap(page);
	UnlockPage(page);
	return(err);
}

static struct address_space_operations hostfs_link_aops = {
	readpage:	hostfs_link_readpage,
};

static struct super_block *hostfs_read_super_common(struct super_block *sb, 
						    char *data)
{
	struct inode * root_inode;
	char *name;

	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 10;
	sb->s_magic = HOSTFS_SUPER_MAGIC;
	sb->s_op = &hostfs_sbops;
	if((data == NULL) || (*((char *) data) == '\0')) data = root_ino;
	name = kmalloc(strlen(data) + 1, GFP_KERNEL);
	if(name == NULL) return(NULL);
	strcpy(name, data);
	root_inode = get_inode(sb, NULL, NULL);
	if(root_inode == NULL){
		kfree(name);
		return(NULL);
	}
	root_inode->u.hostfs_i.host_filename = name;
	sb->s_root = d_alloc_root(root_inode);
	if(read_inode(root_inode)){
		iput(root_inode);
		return(NULL);
	}
	return(sb);
}

struct super_block *hostfs_read_super(struct super_block *sb, void *data, 
				      int silent)
{
	return(hostfs_read_super_common(sb, data));
}

struct super_block *hostfs_root_read_super(struct super_block *sb, void *data, 
					   int silent)
{
	struct buffer_head * bh;
	struct super_block *ret = NULL;
	kdev_t dev = sb->s_dev;
	int blocksize = get_hardsect_size(dev);

	if(blocksize == 0) blocksize = BLOCK_SIZE;
	set_blocksize (dev, blocksize);
	if(!(bh = bread (dev, 0, blocksize))) return NULL;
	if(strncmp(bh->b_data, "HOSTFS:", strlen("HOSTFS:"))) goto out;
	ret = hostfs_read_super_common(sb, bh->b_data + strlen("HOSTFS:"));
 out:
	brelse (bh);
	return(ret);
}

DECLARE_FSTYPE(hostfs_type, "hostfs", hostfs_read_super, 0);
DECLARE_FSTYPE_DEV(hostfs_root_type, "root-hostfs", hostfs_root_read_super);

static int __init init_hostfs(void)
{
	return(register_filesystem(&hostfs_type) ||
	       register_filesystem(&hostfs_root_type));
}

static void __exit exit_hostfs(void)
{
	unregister_filesystem(&hostfs_type);
	unregister_filesystem(&hostfs_root_type);
}

module_init(init_hostfs)
module_exit(exit_hostfs)

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
