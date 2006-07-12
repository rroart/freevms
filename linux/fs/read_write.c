/*
 *  linux/fs/read_write.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <linux/slab.h> 
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/uio.h>
#include <linux/smp_lock.h>
#include <linux/dnotify.h>

#include <asm/uaccess.h>
#include <dyndef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <misc_routines.h>
#include <exe_routines.h>

#ifndef CONFIG_VMS
struct file_operations generic_ro_fops = {
	llseek:		generic_file_llseek,
	read:		generic_file_read,
	mmap:		generic_file_mmap,
};
#endif

ssize_t generic_read_dir(struct file *filp, char *buf, size_t siz, loff_t *ppos)
{
	return -EISDIR;
}

loff_t generic_file_llseek(struct file *file, loff_t offset, int origin)
{
	long long retval;

	switch (origin) {
		case 2:
			offset += file->f_dentry->d_inode->i_size;
			break;
		case 1:
			offset += file->f_pos;
	}
	retval = -EINVAL;
	if (offset>=0 && offset<=file->f_dentry->d_inode->i_sb->s_maxbytes) {
		if (offset != file->f_pos) {
			file->f_pos = offset;
			file->f_reada = 0;
			file->f_version = ++event;
		}
		retval = offset;
	}
	return retval;
}

loff_t no_llseek(struct file *file, loff_t offset, int origin)
{
	return -ESPIPE;
}

loff_t default_llseek(struct file *file, loff_t offset, int origin)
{
	long long retval;

	switch (origin) {
		case 2:
			offset += file->f_dentry->d_inode->i_size;
			break;
		case 1:
			offset += file->f_pos;
	}
	retval = -EINVAL;
	if (offset >= 0) {
		if (offset != file->f_pos) {
			file->f_pos = offset;
			file->f_reada = 0;
			file->f_version = ++event;
		}
		retval = offset;
	}
	return retval;
}

static inline loff_t llseek(struct file *file, loff_t offset, int origin)
{
	loff_t (*fn)(struct file *, loff_t, int);
	loff_t retval;

	fn = default_llseek;
	if (file->f_op && file->f_op->llseek)
		fn = file->f_op->llseek;
	lock_kernel();
	retval = fn(file, offset, origin);
	unlock_kernel();
	return retval;
}

asmlinkage off_t sys_lseek(unsigned int fd, off_t offset, unsigned int origin)
{
	off_t retval;
	struct file * file;

	retval = -EBADF;
	file = fget(fd);
#ifndef CONFIG_VMS
	if (!file)
		goto bad;
	retval = -EINVAL;
	if (origin <= 2) {
		loff_t res = llseek(file, offset, origin);
		retval = res;
		if (res != (loff_t)retval)
			retval = -EOVERFLOW;	/* LFS: should only happen on 32 bit platforms */
	}
	fput(file);
bad:
	return retval;
#else
#if 0
	struct _fcb * fcb = file;
	switch (origin) {
	case 2:
	  /*SEEK_END*/ 
	  offset += fcb->fcb$l_filesize;
	  break;
	case 1:
	  /*SEEK_CUR*/
	  offset += fcb->fcb$l_reserve1;
	  break;
	}
#else
	struct _rabdef * rab = file;
	switch (origin) {
	case 2:
	  /*SEEK_END*/ 
#if 0
	  offset += fcb->fcb$l_filesize;
#else
	  {}
	  struct _fabdef * fab = rab->rab$l_fab;
	  struct _xabdatdef * dat = fab->fab$l_xab;
	  struct _xabfhcdef * fhc = dat->xab$l_nxt;
	  offset = ((fhc->xab$l_ebk-1) << 9) + fhc->xab$w_ffb; // check size --
#endif
	  break;
	case 1:
	  /*SEEK_CUR*/
	  offset += 512*(rab->rab$w_rfa[2] + (rab->rab$w_rfa[1] << 16)) + rab->rab$w_rfa[0];
	  break;
	}
	int block = offset >> 9;
#ifdef __x86_64__
	block++;
#endif
	rab->rab$w_rfa[0] = block & 0xffff;
	rab->rab$w_rfa[1] = block >> 16;
	rab->rab$w_rfa[2] = offset;
#endif
	return offset;
#endif
}

#if !defined(__alpha__)
asmlinkage long sys_llseek(unsigned int fd, unsigned long offset_high,
			   unsigned long offset_low, loff_t * result,
			   unsigned int origin)
{
	int retval;
	struct file * file;
	loff_t offset;

	retval = -EBADF;
	file = fget(fd);
	if (!file)
		goto bad;
	retval = -EINVAL;
	if (origin > 2)
		goto out_putf;

	offset = llseek(file, ((loff_t) offset_high << 32) | offset_low,
			origin);

	retval = (int)offset;
	if (offset >= 0) {
		retval = -EFAULT;
		if (!copy_to_user(result, &offset, sizeof(offset)))
			retval = 0;
	}
out_putf:
	fput(file);
bad:
	return retval;
}
#endif

asmlinkage ssize_t sys_read(unsigned int fd, char * buf, size_t count)
{
	ssize_t ret;
	struct file * file;

	ret = -EBADF;
	file = fget(fd);
#ifdef CONFIG_VMS
#if 0
	if (file && ((struct _fcb *)(file))->fcb$b_type==DYN$C_FCB)
	  goto do_fcb;
#else
#if 0
	printk("sys_read %x %x %x %x\n",ctl$gl_pcb,fd,buf,count);
#endif
	int sts;
	int curcount = count;
	int retcount = 0;
	struct _rabdef * rab = file;
	struct _fabdef * fab = rab->rab$l_fab;
	char * kbuf = kmalloc(fab->fab$w_mrs, GFP_KERNEL); // check size
	while (curcount>0) {
	  rab->rab$l_ubf = kbuf;
	  rab->rab$w_usz = curcount;
#if 0
	  printk("mrs %x\n",fab->fab$w_mrs);
#endif
#if 0
	  if (curcount < fab->fab$w_mrs)
	    rab->rab$w_usz = fab->fab$w_mrs;
#else
	  if (fab->fab$w_mrs)
	    rab->rab$w_usz = fab->fab$w_mrs;
#endif
	  sts = exe$get(rab);
	  if ((sts&1)==0)
	    break;
	  int thiscount = rab->rab$w_rsz;
	  if (curcount < rab->rab$w_rsz)
	    thiscount = curcount;
	  memcpy(buf + retcount, kbuf, thiscount);
#if 0
	  printk("memcpy %x %x %x\n",buf + retcount, kbuf, thiscount);
#endif
	  curcount -= thiscount;
	  retcount += thiscount;
	  if (fab->fab$w_mrs == 0) {
#if 0
	    printk("memcpy2 %x %x %x\n", retcount, curcount, thiscount);
#endif
	    curcount = 0;
	  }
	}
	kfree(kbuf);
#if 0
	printk("sys_read end %x %x %x %x\n",ctl$gl_pcb,fd,sts,retcount);
#endif
	if (sts & 1) {
	  return retcount;
	} else
	  return 0;
#endif
#endif
	if (file) {
		if (file->f_mode & FMODE_READ) {
#ifdef CONFIG_VMS
		  if (fd<3) goto skip;
#endif
#ifndef CONFIG_VMS
			ret = locks_verify_area(FLOCK_VERIFY_READ, file->f_dentry->d_inode,
						file, file->f_pos, count);
#else
			ret = 0;
#endif
			if (!ret) {
			skip: {}
				ssize_t (*read)(struct file *, char *, size_t, loff_t *);
				ret = -EINVAL;
				if (file->f_op && (read = file->f_op->read) != NULL)
					ret = read(file, buf, count, &file->f_pos);
			}
		}
#ifdef CONFIG_VMS
		  if (fd<3) goto skip2;
#endif
#ifndef CONFIG_VMS
		if (ret > 0)
			dnotify_parent(file->f_dentry, DN_ACCESS);
#endif
		fput(file);
	skip2: {}
	}
	return ret;
#ifdef CONFIG_VMS
#if 0
 do_fcb:
	{}

	struct _fcb * fcb;
	unsigned long offset;

	ret = -EBADF;
	file = fget(fd);
	fcb = file;
	offset = fcb->fcb$l_reserve1;
	ret = rms_kernel_read(file, offset, buf, count);
	fcb->fcb$l_reserve1 += ret;
	return ret;
#endif
#endif
}

asmlinkage ssize_t sys_write(unsigned int fd, const char * buf, size_t count)
{
	ssize_t ret;
	struct file * file;

	ret = -EBADF;
	file = fget(fd);
#ifdef CONFIG_VMS
	if (file == 0)
	  return ret;
	struct _rabdef * rab = file;
	struct _fabdef * fab = rab->rab$l_fab;
	rab->rab$l_rbf = buf;
	rab->rab$w_rsz = count;
	if (fab->fab$w_mrs == 0) {
	  rab->rab$v_asy = 1; // workaround to avoid tza hanging
	}
	int sts = exe$put(file);
	if (sts & 1) {
	  return count;
#if 0
	  unsigned rsz = rab.rab$w_rsz;
	  return rsz;
#endif
	} else
	  return 0;
#endif
	if (file) {
		if (file->f_mode & FMODE_WRITE) {
#ifdef CONFIG_VMS
		  if (fd<3) goto skip;
#endif
			struct inode *inode = file->f_dentry->d_inode;
#ifndef CONFIG_VMS
			ret = locks_verify_area(FLOCK_VERIFY_WRITE, inode, file,
				file->f_pos, count);
#else
			ret = 0;
#endif
			if (!ret) {
			skip: {}
				ssize_t (*write)(struct file *, const char *, size_t, loff_t *);
				ret = -EINVAL;
				if (file->f_op && (write = file->f_op->write) != NULL)
					ret = write(file, buf, count, &file->f_pos);
			}
		}
#ifdef CONFIG_VMS
		  if (fd<3) goto skip2;
#endif
#ifndef CONFIG_VMS
		if (ret > 0)
			dnotify_parent(file->f_dentry, DN_MODIFY);
#endif
		fput(file);
	skip2: {}
	}
	return ret;
}


static ssize_t do_readv_writev(int type, struct file *file,
			       const struct iovec * vector,
			       unsigned long count)
{
	typedef ssize_t (*io_fn_t)(struct file *, char *, size_t, loff_t *);
	typedef ssize_t (*iov_fn_t)(struct file *, const struct iovec *, unsigned long, loff_t *);

	size_t tot_len;
	struct iovec iovstack[UIO_FASTIOV];
	struct iovec *iov=iovstack;
	ssize_t ret, i;
	io_fn_t fn;
	iov_fn_t fnv;
	struct inode *inode;

	/*
	 * First get the "struct iovec" from user memory and
	 * verify all the pointers
	 */
	ret = 0;
	if (!count)
		goto out_nofree;
	ret = -EINVAL;
	if (count > UIO_MAXIOV)
		goto out_nofree;
	if (!file->f_op)
		goto out_nofree;
	if (count > UIO_FASTIOV) {
		ret = -ENOMEM;
		iov = kmalloc(count*sizeof(struct iovec), GFP_KERNEL);
		if (!iov)
			goto out_nofree;
	}
	ret = -EFAULT;
	if (copy_from_user(iov, vector, count*sizeof(*vector)))
		goto out;

	/* BSD readv/writev returns EINVAL if one of the iov_len
	   values < 0 or tot_len overflowed a 32-bit integer. -ink */
	tot_len = 0;
	ret = -EINVAL;
	for (i = 0 ; i < count ; i++) {
		size_t tmp = tot_len;
		int len = iov[i].iov_len;
		if (len < 0)
			goto out;
		tot_len += len;
		if (tot_len < tmp || tot_len < (u32)len)
			goto out;
	}

	inode = file->f_dentry->d_inode;
	/* VERIFY_WRITE actually means a read, as we write to user space */
#ifndef CONFIG_VMS
	ret = locks_verify_area((type == VERIFY_WRITE
				 ? FLOCK_VERIFY_READ : FLOCK_VERIFY_WRITE),
				inode, file, file->f_pos, tot_len);
	if (ret) goto out;
#endif

	fnv = (type == VERIFY_WRITE ? file->f_op->readv : file->f_op->writev);
	if (fnv) {
		ret = fnv(file, iov, count, &file->f_pos);
		goto out;
	}

	/* VERIFY_WRITE actually means a read, as we write to user space */
	fn = (type == VERIFY_WRITE ? file->f_op->read :
	      (io_fn_t) file->f_op->write);

	ret = 0;
	vector = iov;
	while (count > 0) {
		void * base;
		size_t len;
		ssize_t nr;

		base = vector->iov_base;
		len = vector->iov_len;
		vector++;
		count--;

		nr = fn(file, base, len, &file->f_pos);

		if (nr < 0) {
			if (!ret) ret = nr;
			break;
		}
		ret += nr;
		if (nr != len)
			break;
	}

out:
	if (iov != iovstack)
		kfree(iov);
out_nofree:
	/* VERIFY_WRITE actually means a read, as we write to user space */
#ifndef CONFIG_VMS
	if ((ret + (type == VERIFY_WRITE)) > 0)
		dnotify_parent(file->f_dentry,
			(type == VERIFY_WRITE) ? DN_MODIFY : DN_ACCESS);
#endif
	return ret;
}

asmlinkage ssize_t sys_readv(unsigned long fd, const struct iovec * vector,
			     unsigned long count)
{
	struct file * file;
	ssize_t ret;


	ret = -EBADF;
	file = fget(fd);
	if (!file)
		goto bad_file;
	if (file->f_op && (file->f_mode & FMODE_READ) &&
	    (file->f_op->readv || file->f_op->read))
		ret = do_readv_writev(VERIFY_WRITE, file, vector, count);
	fput(file);

bad_file:
	return ret;
}

asmlinkage ssize_t sys_writev(unsigned long fd, const struct iovec * vector,
			      unsigned long count)
{
	struct file * file;
	ssize_t ret;


	ret = -EBADF;
	file = fget(fd);
	if (!file)
		goto bad_file;
	if (file->f_op && (file->f_mode & FMODE_WRITE) &&
	    (file->f_op->writev || file->f_op->write))
		ret = do_readv_writev(VERIFY_READ, file, vector, count);
	fput(file);

bad_file:
	return ret;
}

/* From the Single Unix Spec: pread & pwrite act like lseek to pos + op +
   lseek back to original location.  They fail just like lseek does on
   non-seekable files.  */

asmlinkage ssize_t sys_pread(unsigned int fd, char * buf,
			     size_t count, loff_t pos)
{
	ssize_t ret;
	struct file * file;
	ssize_t (*read)(struct file *, char *, size_t, loff_t *);

	ret = -EBADF;
	file = fget(fd);
#ifdef CONFIG_VMS
#if 0
	if (file && ((struct _fcb *)(file))->fcb$b_type==DYN$C_FCB)
	  goto do_fcb;
#else
	struct _rabdef * rab = fget(fd);
	loff_t curpos =  512*(rab->rab$w_rfa[2] + (rab->rab$w_rfa[1] << 16)) + rab->rab$w_rfa[0];
#define SEEK_SET 0
	sys_lseek(fd, pos, SEEK_SET);
	ret = sys_read(fd, buf, count);
	sys_lseek(fd, curpos, SEEK_SET);
	return ret;
#endif
#endif
	if (!file)
		goto bad_file;
	if (!(file->f_mode & FMODE_READ))
		goto out;
#ifndef CONFIG_VMS
	ret = locks_verify_area(FLOCK_VERIFY_READ, file->f_dentry->d_inode,
				file, pos, count);
	if (ret)
		goto out;
#endif
	ret = -EINVAL;
	if (!file->f_op || !(read = file->f_op->read))
		goto out;
	if (pos < 0)
		goto out;
	ret = read(file, buf, count, &pos);
#ifndef CONFIG_VMS
	if (ret > 0)
		dnotify_parent(file->f_dentry, DN_ACCESS);
#endif
out:
#ifndef CONFIG_VMS
	fput(file);
#endif
bad_file:
	return ret;
#ifdef CONFIG_VMS
#if 0
 do_fcb:
	{}

	struct _fcb * fcb;
	unsigned long offset;

	ret = -EBADF;
	file = fget(fd);
	fcb = file;
	offset = pos;
	ret = rms_kernel_read(file, offset, buf, count);
#if 0
	fcb->fcb$l_reserve1 += ret;
#endif
	return ret;
#endif
#endif
}

asmlinkage ssize_t sys_pwrite(unsigned int fd, const char * buf,
			      size_t count, loff_t pos)
{
	ssize_t ret;
	struct file * file;
	ssize_t (*write)(struct file *, const char *, size_t, loff_t *);

	ret = -EBADF;
	file = fget(fd);
	if (!file)
		goto bad_file;
	if (!(file->f_mode & FMODE_WRITE))
		goto out;
#ifndef CONFIG_VMS
	ret = locks_verify_area(FLOCK_VERIFY_WRITE, file->f_dentry->d_inode,
				file, pos, count);
	if (ret)
		goto out;
#endif
	ret = -EINVAL;
	if (!file->f_op || !(write = file->f_op->write))
		goto out;
	if (pos < 0)
		goto out;

	ret = write(file, buf, count, &pos);
#ifndef CONFIG_VMS
	if (ret > 0)
		dnotify_parent(file->f_dentry, DN_MODIFY);
#endif
out:
	fput(file);
bad_file:
	return ret;
}
