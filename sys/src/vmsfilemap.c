// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on filemap.c.

/*
 *	linux/mm/filemap.c
 *
 * Copyright (C) 1994-1999  Linus Torvalds
 */

/*
 * This file handles the generic file mmap semantics used by
 * most "normal" filesystems (but you don't /have/ to use this:
 * the NFS filesystem used to do this differently, for example)
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/locks.h>
#include <linux/pagemap.h>
#include <linux/smp_lock.h>
#include <linux/blkdev.h>
#include <linux/file.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/iobuf.h>
#include <linux/compiler.h>

#include <asm/pgalloc.h>
#include <asm/uaccess.h>
#include <asm/mman.h>

#include <linux/highmem.h>

#include <fcbdef.h>
#include <ipldef.h>
#include <phddef.h>
#include <rdedef.h>
#include <misc_routines.h>

/*
 * Shared mappings implemented 30.11.1994. It's not fully working yet,
 * though.
 *
 * Shared mappings now work. 15.8.1995  Bruno.
 *
 * finished 'unifying' the page and buffer cache and SMP-threaded the
 * page-cache, 21.05.1999, Ingo Molnar <mingo@redhat.com>
 *
 * SMP-threaded pagemap-LRU 1999, Andrea Arcangeli <andrea@suse.de>
 */

/*
 * Add a page to the dirty page list.
 */
void fastcall set_page_dirty(struct page *page)
{
#if 0
  test_and_set_bit(PG_dirty, &page->pfn$l_page_state);
#endif
}

/**
 * invalidate_inode_pages - Invalidate all the unlocked pages of one inode
 * @inode: the inode which pages we want to invalidate
 *
 * This function only removes the unlocked pages, if you want to
 * remove all the pages of one inode, you must call truncate_inode_pages.
 */

void invalidate_inode_pages(struct inode * inode)
{
}

/**
 * truncate_inode_pages - truncate *all* the pages from an offset
 * @mapping: mapping to truncate
 * @lstart: offset from with to truncate
 *
 * Truncate the page cache at a set offset, removing the pages
 * that are beyond that offset (and zeroing out partial pages).
 * If any page is locked we wait for it to become unlocked.
 */
void truncate_inode_pages(struct address_space * mapping, loff_t lstart) 
{
}

/**
 * invalidate_inode_pages2 - Clear all the dirty bits around if it can't
 * free the pages because they're mapped.
 * @mapping: the address_space which pages we want to invalidate
 */
void invalidate_inode_pages2(struct address_space * mapping)
{
}

/*
 * Two-stage data sync: first start the IO, then go back and
 * collect the information..
 */
int generic_buffer_fdatasync(struct _fcb *inode, unsigned long start_idx, unsigned long end_idx)
{
	int retval=0;

	return retval;
}

/**
 *      filemap_fdatasync - walk the list of dirty pages of the given address space
 *     	and writepage() all of them.
 * 
 *      @mapping: address space structure to write
 *
 */
extern int block_write_full_page2();

int filemap_fdatasync(struct address_space * mapping)
{
	int ret = 0;

	return ret;
}

/**
 *      filemap_fdatawait - walk the list of locked pages of the given address space
 *     	and wait for all of them.
 * 
 *      @mapping: address space structure to wait for
 *
 */
int filemap_fdatawait(struct address_space * mapping)
{
	int ret = 0;

	return ret;
}

#if 0
/*
 * This is a generic file read routine, and uses the
 * inode->i_op->readpage() function for the actual low-level
 * stuff.
 *
 * This is really ugly. But the goto's actually try to clarify some
 * of the logic when it comes to error handling etc.
 */
void do_generic_file_read(struct file * filp, loff_t *ppos, read_descriptor_t * desc, read_actor_t actor)
{
	struct _fcb *inode = filp->f_dentry->d_inode;
	unsigned long index, offset;
	struct page *cached_page;
	int error;

	cached_page = NULL;
	index = *ppos >> PAGE_CACHE_SHIFT;
	offset = *ppos & ~PAGE_CACHE_MASK;

	for (;;) {
		struct page *page;
		unsigned long end_index, nr, ret;
		struct _fcb * fcb;

		end_index = inode->fcb$l_filesize >> PAGE_CACHE_SHIFT;
			
		if (index > end_index)
			break;
		nr = PAGE_CACHE_SIZE;
		if (index == end_index) {
			nr = inode->fcb$l_filesize & ~PAGE_CACHE_MASK;
			if (nr <= offset)
				break;
		}

		nr = nr - offset;

		/*
		 * Try to find the data in the page cache..
		 */

		page = 0;

		/*
		 * Ok, it wasn't cached, so we need to create a new
		 * page..
		 *
		 * We get here with the page cache lock held.
		 */

		cached_page = alloc_pages(0, 0);
		
		/*
		 * Somebody may have added the page while we
		 * dropped the page cache lock. Check for that.
		 */

		/*
		 * Ok, add the new page to the hash-queues...
		 */
		page = cached_page;
		cached_page = NULL;

		/* ... and start the actual read. The read will unlock the page. */
		fcb = e2_search_fcb(filp->f_dentry->d_inode);

		error = block_read_full_page2(filp->f_dentry->d_inode, page, index);

		/*
		 * Mark the page accessed if we read the
		 * beginning or we just did an lseek.
		 */
#if 0
		if (!offset || !filp->f_reada)
			mark_page_accessed(page);
#endif

		/*
		 * Ok, we have the page, and it's up-to-date, so
		 * now we can copy it to user space...
		 *
		 * The actor routine returns how many bytes were actually used..
		 * NOTE! This may not be the same as how much of a user buffer
		 * we filled up (we may be padding etc), so we can only update
		 * "pos" here (the actor routine has to update the user buffer
		 * pointers and the remaining count).
		 */
		ret = actor(desc, page, offset, nr);
		offset += ret;
		index += offset >> PAGE_CACHE_SHIFT;
		offset &= ~PAGE_CACHE_MASK;

		page_cache_release(page);
		if (ret == nr && desc->count)
			continue;
		break;

	}

	*ppos = ((loff_t) index << PAGE_CACHE_SHIFT) + offset;
	filp->f_reada = 1;
	if (cached_page)
		page_cache_release(cached_page);
#if 0
	UPDATE_ATIME(inode);
#endif
}

void do_rms_generic_file_read(struct _fcb * filp, loff_t *ppos, read_descriptor_t * desc, read_actor_t actor)
{
	struct _fcb * fcb = filp;
	struct _fcb * inode=0;
	unsigned long index, offset;
	struct page *cached_page;
	int error;

	cached_page = NULL;
	index = *ppos >> PAGE_CACHE_SHIFT;
	offset = *ppos & ~PAGE_CACHE_MASK;

	for (;;) {
		struct page *page;
		unsigned long end_index, nr, ret;

#if 0
		// not yet
		if (fcb->fcb$l_fill_5)
		end_index = inode->fcb$l_filesize >> PAGE_CACHE_SHIFT;
		else
#endif
		  end_index = 10000; //not yet: fcb->fcb$l_efblk + 1;  

		if (index > end_index)
			break;
		nr = PAGE_CACHE_SIZE;
		if (index == end_index) {
#if 0
		  // not yet
			if (fcb->fcb$l_fill_5)
			nr = inode->fcb$l_filesize & ~PAGE_CACHE_MASK;
			else
#endif
			nr = ((fcb->fcb$l_efblk << 9) + 0 ) & ~PAGE_CACHE_MASK;
			if (nr <= offset)
				break;
		}

		nr = nr - offset;

		/*
		 * Try to find the data in the page cache..
		 */

		page = 0;

		/*
		 * Ok, it wasn't cached, so we need to create a new
		 * page..
		 *
		 * We get here with the page cache lock held.
		 */

		cached_page = alloc_pages(0, 0);
		
		/*
		 * Somebody may have added the page while we
		 * dropped the page cache lock. Check for that.
		 */

		/*
		 * Ok, add the new page to the hash-queues...
		 */
		page = cached_page;
		cached_page = NULL;

		/* ... and start the actual read. The read will unlock the page. */
		//printk("R %x %x R",page,index);
		if (fcb->fcb$l_fill_5)
		error = block_read_full_page3(filp, page, index);
		else
		error = ods2_block_read_full_page3(filp->fcb$l_wlfl, page, index);
		  
		/*
		 * Mark the page accessed if we read the
		 * beginning or we just did an lseek.
		 */
#if 0
		if (!offset)
			mark_page_accessed(page);
#endif

		/*
		 * Ok, we have the page, and it's up-to-date, so
		 * now we can copy it to user space...
		 *
		 * The actor routine returns how many bytes were actually used..
		 * NOTE! This may not be the same as how much of a user buffer
		 * we filled up (we may be padding etc), so we can only update
		 * "pos" here (the actor routine has to update the user buffer
		 * pointers and the remaining count).
		 */
		ret = actor(desc, page, offset, nr);
		offset += ret;
		index += offset >> PAGE_CACHE_SHIFT;
		offset &= ~PAGE_CACHE_MASK;

		page_cache_release(page);
		if (ret == nr && desc->count)
			continue;
		break;

	}

	*ppos = ((loff_t) index << PAGE_CACHE_SHIFT) + offset;
	if (cached_page)
		page_cache_release(cached_page);
#if 0
	if (inode)
	UPDATE_ATIME(inode);
#endif
}
#endif

static ssize_t generic_file_direct_IO(int rw, struct file * filp, char * buf, size_t count, loff_t offset)
{
	ssize_t retval;
	int new_iobuf, chunk_size, blocksize_mask, blocksize, blocksize_bits, iosize, progress;
	struct kiobuf * iobuf;
	struct _fcb * inode = filp->f_dentry->d_inode;

	new_iobuf = 0;
	iobuf = filp->f_iobuf;
	if (test_and_set_bit(0, &filp->f_iobuf_lock)) {
		/*
		 * A parallel read/write is using the preallocated iobuf
		 * so just run slow and allocate a new one.
		 */
		retval = alloc_kiovec(1, &iobuf);
		if (retval)
			goto out;
		new_iobuf = 1;
	}

#if 0
	// not yet
	blocksize = 1 << inode->i_blkbits;
	blocksize_bits = inode->i_blkbits;
#endif
	blocksize_mask = blocksize - 1;
	chunk_size = KIO_MAX_ATOMIC_IO << 10;

	retval = -EINVAL;
	if ((offset & blocksize_mask) || (count & blocksize_mask))
		goto out_free;

	/*
	 * Flush to disk exclusively the _data_, metadata must remain
	 * completly asynchronous or performance will go to /dev/null.
	 */
	//retval = filemap_fdatasync();
#if 0
	if (retval == 0)
		retval = fsync_inode_data_buffers(inode);
#endif
	//if (retval == 0)
	//	retval = filemap_fdatawait();
	if (retval < 0)
		goto out_free;

	progress = retval = 0;
	while (count > 0) {
		iosize = count;
		if (iosize > chunk_size)
			iosize = chunk_size;

		retval = map_user_kiobuf(rw, iobuf, (unsigned long) buf, iosize);
		if (retval)
			break;

		retval = ext2_direct_IO(rw, inode, iobuf, (offset+progress) >> blocksize_bits, blocksize);

		if (rw == READ && retval > 0)
			mark_dirty_kiobuf(iobuf, retval);
		
		if (retval >= 0) {
			count -= retval;
			buf += retval;
			progress += retval;
		}

		unmap_kiobuf(iobuf);

		if (retval != iosize)
			break;
	}

	if (progress)
		retval = progress;

 out_free:
	if (!new_iobuf)
		clear_bit(0, &filp->f_iobuf_lock);
	else
		free_kiovec(1, &iobuf);
 out:	
	return retval;
}

int file_read_actor(read_descriptor_t * desc, struct page *page, unsigned long offset, unsigned long size)
{
	char *kaddr;
	unsigned long left, count = desc->count;

	if (size > count)
		size = count;

	kaddr = kmap(page);
	left = __copy_to_user(desc->buf, kaddr + offset, size);
	kunmap(page);
	
	if (left) {
		size -= left;
		desc->error = -EFAULT;
	}
	desc->count = count - size;
	desc->written += size;
	desc->buf += size;
	return size;
}

#if 0
/*
 * This is the "read()" routine for all filesystems
 * that can use the page cache directly.
 */
ssize_t generic_file_read(struct file * filp, char * buf, size_t count, loff_t *ppos)
{
	ssize_t retval;

	if ((ssize_t) count < 0)
		return -EINVAL;

	if (filp->f_flags & O_DIRECT)
		goto o_direct;

	retval = -EFAULT;
	if (access_ok(VERIFY_WRITE, buf, count)) {
		retval = 0;

		if (count) {
			read_descriptor_t desc;

			desc.written = 0;
			desc.count = count;
			desc.buf = buf;
			desc.error = 0;
			do_generic_file_read(filp, ppos, &desc, file_read_actor);

			retval = desc.written;
			if (!retval)
				retval = desc.error;
		}
	}
 out:
	return retval;

 o_direct:
	{
		loff_t pos = *ppos, size;
		struct _fcb *inode = filp->f_dentry->d_inode;

		retval = 0;
		if (!count)
			goto out; /* skip atime */
		size = inode->fcb$l_filesize;
		if (pos < size) {
			if (pos + count > size)
				count = size - pos;
			retval = generic_file_direct_IO(READ, filp, buf, count, pos);
			if (retval > 0)
				*ppos = pos + retval;
		}
#if 0
		UPDATE_ATIME(filp->f_dentry->d_inode);
#endif
		goto out;
	}
}

ssize_t rms_generic_file_read(struct file * filp, char * buf, size_t count, loff_t *ppos)
{
	ssize_t retval;

	if ((ssize_t) count < 0)
		return -EINVAL;

	retval = -EFAULT;
	if (access_ok(VERIFY_WRITE, buf, count)) {
		retval = 0;

		if (count) {
			read_descriptor_t desc;

			desc.written = 0;
			desc.count = count;
			desc.buf = buf;
			desc.error = 0;
			do_rms_generic_file_read(filp, ppos, &desc, file_read_actor);

			retval = desc.written;
			if (!retval)
				retval = desc.error;
		}
	}
 out:
	return retval;
}
#endif

static int file_send_actor(read_descriptor_t * desc, struct page *page, unsigned long offset , unsigned long size)
{
	ssize_t written;
	unsigned long count = desc->count;
	struct file *file = (struct file *) desc->buf;

	if (size > count)
		size = count;

 	if (file->f_op->sendpage) {
 		written = file->f_op->sendpage(file, page, offset,
					       size, &file->f_pos, size<count);
	} else {
		char *kaddr;
		mm_segment_t old_fs;

		old_fs = get_fs();
		set_fs(KERNEL_DS);

		kaddr = kmap(page);
		written = file->f_op->write(file, kaddr + offset, size, &file->f_pos);
		kunmap(page);

		set_fs(old_fs);
	}
	if (written < 0) {
		desc->error = written;
		written = 0;
	}
	desc->count = count - written;
	desc->written += written;
	return written;
}

asmlinkage ssize_t sys_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
#if 0
	ssize_t retval;
	struct file * in_file, * out_file;
	struct _fcb * in_inode, * out_inode;

	/*
	 * Get input file, and verify that it is ok..
	 */
	retval = -EBADF;
	in_file = fget(in_fd);
	if (!in_file)
		goto out;
	if (!(in_file->f_mode & FMODE_READ))
		goto fput_in;
	retval = -EINVAL;
	in_inode = in_file->f_dentry->d_inode;
	if (!in_inode)
		goto fput_in;	
	retval = locks_verify_area(FLOCK_VERIFY_READ, in_inode, in_file, in_file->f_pos, count);
	if (retval)
		goto fput_in;

	/*
	 * Get output file, and verify that it is ok..
	 */
	retval = -EBADF;
	out_file = fget(out_fd);
	if (!out_file)
		goto fput_in;
	if (!(out_file->f_mode & FMODE_WRITE))
		goto fput_out;
	retval = -EINVAL;
	if (!out_file->f_op || !out_file->f_op->write)
		goto fput_out;
	out_inode = out_file->f_dentry->d_inode;
	retval = locks_verify_area(FLOCK_VERIFY_WRITE, out_inode, out_file, out_file->f_pos, count);
	if (retval)
		goto fput_out;

	retval = 0;
	if (count) {
		read_descriptor_t desc;
		loff_t pos = 0, *ppos;

		retval = -EFAULT;
		ppos = &in_file->f_pos;
		if (offset) {
			if (get_user(pos, offset))
				goto fput_out;
			ppos = &pos;
		}

		desc.written = 0;
		desc.count = count;
		desc.buf = (char *) out_file;
		desc.error = 0;
		do_generic_file_read(in_file, ppos, &desc, file_send_actor);

		retval = desc.written;
		if (!retval)
			retval = desc.error;
		if (offset)
			put_user(pos, offset);
	}

fput_out:
	fput(out_file);
fput_in:
	fput(in_file);
out:
	return retval;
#else
	return -EBADF;
#endif
}

#ifdef __x86_64__
asmlinkage ssize_t sys_sendfile64(int out_fd, int in_fd, loff_t *offset, size_t count)
{
  printk("sendfile64 not imp\n");
  return -1;
}
#endif

static ssize_t do_readahead(struct file *file, unsigned long index, unsigned long nr)
{
	unsigned long max;

	/* Limit it to the size of the file.. */
	max = 0;
	if (index > max)
		return 0;
	max -= index;
	if (nr > max)
		nr = max;

	/* And limit it to a sane percentage of the inactive list.. */
	extern int nr_inactive_pages;
	max = nr_inactive_pages / 2;
	if (nr > max)
		nr = max;

	while (nr) {
	  //page_cache_read(file, index);
		index++;
		nr--;
	}
	return 0;
}

asmlinkage ssize_t sys_readahead(int fd, loff_t offset, size_t count)
{
	ssize_t ret;
	struct file *file;

	ret = -EBADF;
	file = fget(fd);
	if (file) {
		if (file->f_mode & FMODE_READ) {
			unsigned long start = offset >> PAGE_CACHE_SHIFT;
			unsigned long len = (count + ((long)offset & ~PAGE_CACHE_MASK)) >> PAGE_CACHE_SHIFT;
			ret = do_readahead(file, start, len);
		}
		fput(file);
	}
	return ret;
}

/* Called with mm->page_table_lock held to protect against other
 * threads/the swapper from ripping pte's out from under us.
 */
static inline int filemap_sync_pte(pte_t * ptep, struct _rde *vma,
	unsigned long address, unsigned int flags)
{
	pte_t pte = *ptep;

	if (pte_present(pte)) {
		struct page *page = pte_page(pte);
		if (VALID_PAGE(page) && !PageReserved(page) && ptep_test_and_clear_dirty(ptep)) {
			flush_tlb_page2(current->mm, address);
			set_page_dirty(page);
		}
	}
	return 0;
}

static inline int filemap_sync_pte_range(pmd_t * pmd,
	unsigned long address, unsigned long size, 
	struct _rde *vma, unsigned long offset, unsigned int flags)
{
	pte_t * pte;
	unsigned long end;
	int error;

	if (pmd_none(*pmd))
		return 0;
	if (pmd_bad(*pmd)) {
		pmd_ERROR(*pmd);
		pmd_clear(pmd);
		return 0;
	}
	pte = pte_offset(pmd, address);
	offset += address & PMD_MASK;
	address &= ~PMD_MASK;
	end = address + size;
	if (end > PMD_SIZE)
		end = PMD_SIZE;
	error = 0;
	do {
		error |= filemap_sync_pte(pte, vma, address + offset, flags);
		address += PAGE_SIZE;
		pte++;
	} while (address && (address < end));
	return error;
}

static inline int filemap_sync_pmd_range(pud_t * pud,
	unsigned long address, unsigned long size, 
	struct _rde *vma, unsigned long offset, unsigned int flags)
{
	pmd_t * pmd;
	unsigned long offset_not, end;
	int error;

	if (pud_none(*pud))
		return 0;
	if (pud_bad(*pud)) {
		pud_ERROR(*pud);
		pud_clear(pud);
		return 0;
	}
	pmd = pmd_offset(pud, address);
	offset = address & PGDIR_MASK;
	address &= ~PGDIR_MASK;
	end = address + size;
	if (end > PGDIR_SIZE)
		end = PGDIR_SIZE;
	error = 0;
	do {
		error |= filemap_sync_pte_range(pmd, address, end - address, vma, offset, flags);
		address = (address + PMD_SIZE) & PMD_MASK;
		pmd++;
	} while (address && (address < end));
	return error;
}

static inline int filemap_sync_pud_range(pgd_t * pgd,
	unsigned long address, unsigned long size, 
	struct _rde *vma, unsigned int flags)
{
	pud_t * pud;
	unsigned long offset, end;
	int error;

	if (pgd_none(*pgd))
		return 0;
	if (pgd_bad(*pgd)) {
		pgd_ERROR(*pgd);
		pgd_clear(pgd);
		return 0;
	}
	pud = pud_offset(pgd, address);
	offset = address & PGDIR_MASK;
	address &= ~PGDIR_MASK;
	end = address + size;
	if (end > PGDIR_SIZE)
		end = PGDIR_SIZE;
	error = 0;
	do {
		error |= filemap_sync_pmd_range(pud, address, end - address, vma, offset, flags);
		address = (address + PUD_SIZE) & PUD_MASK;
		pud++;
	} while (address && (address < end));
	return error;
}

int filemap_sync(struct vm_area_struct * vma, unsigned long address,
	size_t size, unsigned int flags)
{
	pgd_t * dir;
	unsigned long end = address + size;
	int error = 0;
	struct mm_struct *mm =current->mm;

	/* Aquire the lock early; it may be possible to avoid dropping
	 * and reaquiring it repeatedly.
	 */
	//spin_lock(&vma->vm_mm->page_table_lock);

	dir = pgd_offset(mm, address);
	flush_cache_range(mm, end - size, end);
	if (address >= end)
		BUG();
	do {
		error |= filemap_sync_pud_range(dir, address, end - address, vma, flags);
		address = (address + PGDIR_SIZE) & PGDIR_MASK;
		dir++;
	} while (address && (address < end));
	flush_tlb_range(mm, end - size, end);

	//spin_unlock(&vma->vm_mm->page_table_lock);

	return error;
}

/* This is used for a general mmap of a disk file */

#undef vm_area_struct
int generic_file_mmap(struct file * file, struct vm_area_struct * vma) {
}
#define vm_area_struct _rde

/*
 * The msync() system call.
 */

/*
 * MS_SYNC syncs the entire file - including mappings.
 *
 * MS_ASYNC initiates writeout of just the dirty mapped data.
 * This provides no guarantee of file integrity - things like indirect
 * blocks may not have started writeout.  MS_ASYNC is primarily useful
 * where the application knows that it has finished with the data and
 * wishes to intelligently schedule its own I/O traffic.
 */
static int msync_interval(struct _rde * vma,
	unsigned long start, unsigned long end, int flags)
{
	int ret = 0;
	struct file * file = 0;//vma->vm_file;

	if (file && (vma->rde$l_flags & VM_SHARED)) {
		ret = filemap_sync(vma, start, end-start, flags);

		if (!ret && (flags & (MS_SYNC|MS_ASYNC))) {
			struct _fcb * inode = file->f_dentry->d_inode;

#if 0
			down(&inode->i_sem);
#endif
			//ret = filemap_fdatasync();
			if (flags & MS_SYNC) {
				int err;

				if (file->f_op && file->f_op->fsync) {
					err = file->f_op->fsync(file, file->f_dentry, 1);
					if (err && !ret)
						ret = err;
				}
				//err = filemap_fdatawait();
				if (err && !ret)
					ret = err;
			}
#if 0
			up(&inode->i_sem);
#endif
		}
	}
	return ret;
}

asmlinkage long sys_msync(unsigned long start, size_t len, int flags)
{
	unsigned long end;
	struct _rde * vma;
	int unmapped_error, error = -EINVAL;

	down_read(&current->mm->mmap_sem);
	if (start & ~PAGE_MASK)
		goto out;
	len = (len + ~PAGE_MASK) & PAGE_MASK;
	end = start + len;
	if (end < start)
		goto out;
	if (flags & ~(MS_ASYNC | MS_INVALIDATE | MS_SYNC))
		goto out;
	error = 0;
	if (end == start)
		goto out;
	/*
	 * If the interval [start,end) covers some unmapped address ranges,
	 * just ignore them, but return -EFAULT at the end.
	 */
	//vma = find_vma(current->mm, start);
	vma = find_vma(current->pcb$l_phd,start);
	unmapped_error = 0;
	for (;;) {
		/* Still start < end. */
		error = -EFAULT;
		if (!vma)
			goto out;
		/* Here start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (start < vma->rde$pq_start_va) {
			unmapped_error = -EFAULT;
			start = vma->rde$pq_start_va;
		}
		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (end <= (vma->rde$pq_start_va + vma->rde$q_region_size)) {
			if (start < end) {
				error = msync_interval(vma, start, end, flags);
				if (error)
					goto out;
			}
			error = unmapped_error;
			goto out;
		}
		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size) < end. */
		error = msync_interval(vma, start, (vma->rde$pq_start_va + vma->rde$q_region_size), flags);
		if (error)
			goto out;
		start = (vma->rde$pq_start_va + vma->rde$q_region_size);
		vma = 0; //vma->vm_next;
	}
out:
	up_read(&current->mm->mmap_sem);
	return error;
}

static inline void setup_read_behavior(struct _rde * vma,
	int behavior)
{
  //VM_ClearReadHint(vma);
	switch(behavior) {
		case MADV_SEQUENTIAL:
			vma->rde$l_flags |= VM_SEQ_READ;
			break;
		case MADV_RANDOM:
			vma->rde$l_flags |= VM_RAND_READ;
			break;
		default:
			break;
	}
	return;
}

static long madvise_fixup_start(struct _rde * vma,
	unsigned long end, int behavior)
{
	struct _rde * n;
	struct mm_struct * mm = current->mm;//vma->vm_mm;

	n = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!n)
		return -EAGAIN;
	*n = *vma;
	n->rde$q_region_size = end - (unsigned long)n->rde$pq_start_va;
	setup_read_behavior(n, behavior);
	//n->vm_raend = 0;
#if 0
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
#endif
	//vma->vm_pgoff += (end - vma->rde$pq_start_va) >> PAGE_SHIFT;
	lock_vma_mappings(vma);
	spin_lock(&mm->page_table_lock);
	vma->rde$pq_start_va = end;
	//__insert_vm_struct(mm, n);
	insrde(n,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	spin_unlock(&mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

static long madvise_fixup_end(struct _rde * vma,
	unsigned long start, int behavior)
{
	struct _rde * n;
	struct mm_struct * mm = current->mm;// vma->vm_mm;

	n = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!n)
		return -EAGAIN;
	*n = *vma;
	n->rde$pq_start_va = start;
	//n->vm_pgoff += (n->rde$pq_start_va - vma->rde$pq_start_va) >> PAGE_SHIFT;
	setup_read_behavior(n, behavior);
	//n->vm_raend = 0;
#if 0
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
#endif
	lock_vma_mappings(vma);
	spin_lock(&mm->page_table_lock);
	vma->rde$q_region_size = start - (unsigned long)vma->rde$pq_start_va;
	//__insert_vm_struct(mm, n);
	insrde(n,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	spin_unlock(&mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

static long madvise_fixup_middle(struct _rde * vma,
	unsigned long start, unsigned long end, int behavior)
{
	struct _rde * left, * right;
	struct mm_struct * mm = current->mm;//vma->vm_mm;

	left = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!left)
		return -EAGAIN;
	right = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!right) {
		kmem_cache_free(vm_area_cachep, left);
		return -EAGAIN;
	}
	*left = *vma;
	*right = *vma;
	left->rde$q_region_size = start - (unsigned long)left->rde$pq_start_va;
	right->rde$pq_start_va = end;
	//right->vm_pgoff += (right->rde$pq_start_va - left->rde$pq_start_va) >> PAGE_SHIFT;
	//left->vm_raend = 0;
	//right->vm_raend = 0;
#if 0
	if (vma->vm_file)
		atomic_add(2, &vma->vm_file->f_count);

	if (vma->vm_ops && vma->vm_ops->open) {
		vma->vm_ops->open(left);
		vma->vm_ops->open(right);
	}
#endif
	//vma->vm_pgoff += (start - vma->rde$pq_start_va) >> PAGE_SHIFT;
	//vma->vm_raend = 0;
	lock_vma_mappings(vma);
	spin_lock(&mm->page_table_lock);
	vma->rde$pq_start_va = start;
	vma->rde$q_region_size = end - start;
	setup_read_behavior(vma, behavior);
	//__insert_vm_struct(mm, left);
	insrde(left,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	//__insert_vm_struct(mm, right);
	insrde(right,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	spin_unlock(&mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

/*
 * We can potentially split a vm area into separate
 * areas, each area with its own behavior.
 */
static long madvise_behavior(struct _rde * vma,
	unsigned long start, unsigned long end, int behavior)
{
	int error = 0;

	/* This caps the number of vma's this process can own */
	if (current->mm->map_count > MAX_MAP_COUNT)
		return -ENOMEM;

	if (start == vma->rde$pq_start_va) {
		if (end == (vma->rde$pq_start_va + vma->rde$q_region_size)) {
			setup_read_behavior(vma, behavior);
			//vma->vm_raend = 0;
		} else
			error = madvise_fixup_start(vma, end, behavior);
	} else {
		if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
			error = madvise_fixup_end(vma, start, behavior);
		else
			error = madvise_fixup_middle(vma, start, end, behavior);
	}

	return error;
}

/*
 * Schedule all required I/O operations, then run the disk queue
 * to make sure they are started.  Do not wait for completion.
 */
static long madvise_willneed(struct _rde * vma,
	unsigned long start, unsigned long end)
{
	long error = -EBADF;
	struct file * file;
	unsigned long size, rlim_rss;

	/* Doesn't work if there's no mapped file. */
#if 0
	if (!vma->vm_file)
		return error;
	file = vma->vm_file;
	size = (file->f_dentry->d_inode->fcb$l_filesize + PAGE_CACHE_SIZE - 1) >>
							PAGE_CACHE_SHIFT;
#endif

	start = ((start - (unsigned long)vma->rde$pq_start_va) >> PAGE_SHIFT);// + vma->vm_pgoff;
	if (end > (vma->rde$pq_start_va + vma->rde$q_region_size))
		end = (vma->rde$pq_start_va + vma->rde$q_region_size);
	end = ((end - (unsigned long)vma->rde$pq_start_va) >> PAGE_SHIFT);// + vma->vm_pgoff;

	/* Make sure this doesn't exceed the process's max rss. */
	error = -EIO;
	rlim_rss = current->rlim ?  current->rlim[RLIMIT_RSS].rlim_cur :
				LONG_MAX; /* default: see resource.h */
	if ((current->mm->rss + (end - start)) > rlim_rss)
		return error;

	/* round to cluster boundaries if this isn't a "random" area. */
#if 0
	if (0) {
	  //if (!VM_RandomReadHint(vma)) {
		start = CLUSTER_OFFSET(start);
		end = CLUSTER_OFFSET(end + CLUSTER_PAGES - 1);

		while ((start < end) && (start < size)) {
			error = read_cluster_nonblocking(file, start, size);
			start += CLUSTER_PAGES;
			if (error < 0)
				break;
		}
	} else {
		while ((start < end) && (start < size)) {
		  error = 0;//page_cache_read(file, start);
			start++;
			if (error < 0)
				break;
		}
	}

	/* Don't wait for someone else to push these requests. */
	run_task_queue(&tq_disk);
#endif

	return error;
}

/*
 * Application no longer needs these pages.  If the pages are dirty,
 * it's OK to just throw them away.  The app will be more careful about
 * data it wants to keep.  Be sure to free swap resources too.  The
 * zap_page_range call sets things up for refill_inactive to actually free
 * these pages later if no one else has touched them in the meantime,
 * although we could add these pages to a global reuse list for
 * refill_inactive to pick up before reclaiming other pages.
 *
 * NB: This interface discards data rather than pushes it out to swap,
 * as some implementations do.  This has performance implications for
 * applications like large transactional databases which want to discard
 * pages in anonymous maps after committing to backing store the data
 * that was kept in them.  There is no reason to write this data out to
 * the swap area if the application is discarding it.
 *
 * An interface that causes the system to free clean pages and flush
 * dirty pages is already available as msync(MS_INVALIDATE).
 */
static long madvise_dontneed(struct _rde * vma,
	unsigned long start, unsigned long end)
{
	if (vma->rde$l_flags & VM_LOCKED)
		return -EINVAL;

	zap_page_range(current->mm, start, end - start);
	return 0;
}

static long madvise_vma(struct _rde * vma, unsigned long start,
	unsigned long end, int behavior)
{
	long error = -EBADF;

	switch (behavior) {
	case MADV_NORMAL:
	case MADV_SEQUENTIAL:
	case MADV_RANDOM:
		error = madvise_behavior(vma, start, end, behavior);
		break;

	case MADV_WILLNEED:
		error = madvise_willneed(vma, start, end);
		break;

	case MADV_DONTNEED:
		error = madvise_dontneed(vma, start, end);
		break;

	default:
		error = -EINVAL;
		break;
	}
		
	return error;
}

/*
 * The madvise(2) system call.
 *
 * Applications can use madvise() to advise the kernel how it should
 * handle paging I/O in this VM area.  The idea is to help the kernel
 * use appropriate read-ahead and caching techniques.  The information
 * provided is advisory only, and can be safely disregarded by the
 * kernel without affecting the correct operation of the application.
 *
 * behavior values:
 *  MADV_NORMAL - the default behavior is to read clusters.  This
 *		results in some read-ahead and read-behind.
 *  MADV_RANDOM - the system should read the minimum amount of data
 *		on any access, since it is unlikely that the appli-
 *		cation will need more than what it asks for.
 *  MADV_SEQUENTIAL - pages in the given range will probably be accessed
 *		once, so they can be aggressively read ahead, and
 *		can be freed soon after they are accessed.
 *  MADV_WILLNEED - the application is notifying the system to read
 *		some pages ahead.
 *  MADV_DONTNEED - the application is finished with the given range,
 *		so the kernel can free resources associated with it.
 *
 * return values:
 *  zero    - success
 *  -EINVAL - start + len < 0, start is not page-aligned,
 *		"behavior" is not a valid value, or application
 *		is attempting to release locked or shared pages.
 *  -ENOMEM - addresses in the specified range are not currently
 *		mapped, or are outside the AS of the process.
 *  -EIO    - an I/O error occurred while paging in data.
 *  -EBADF  - map exists, but area maps something that isn't a file.
 *  -EAGAIN - a kernel resource was temporarily unavailable.
 */
asmlinkage long sys_madvise(unsigned long start, size_t len, int behavior)
{
	unsigned long end;
	struct _rde * vma;
	int unmapped_error = 0;
	int error = -EINVAL;

	down_write(&current->mm->mmap_sem);

	if (start & ~PAGE_MASK)
		goto out;
	len = (len + ~PAGE_MASK) & PAGE_MASK;
	end = start + len;
	if (end < start)
		goto out;

	error = 0;
	if (end == start)
		goto out;

	/*
	 * If the interval [start,end) covers some unmapped address
	 * ranges, just ignore them, but return -ENOMEM at the end.
	 */
	//vma = find_vma(current->mm, start);
	vma = find_vma(current->pcb$l_phd,start);
	for (;;) {
		/* Still start < end. */
		error = -ENOMEM;
		if (!vma)
			goto out;

		/* Here start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (start < vma->rde$pq_start_va) {
			unmapped_error = -ENOMEM;
			start = vma->rde$pq_start_va;
		}

		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (end <= (vma->rde$pq_start_va + vma->rde$q_region_size)) {
			if (start < end) {
				error = madvise_vma(vma, start, end,
							behavior);
				if (error)
					goto out;
			}
			error = unmapped_error;
			goto out;
		}

		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size) < end. */
		error = madvise_vma(vma, start, (vma->rde$pq_start_va + vma->rde$q_region_size), behavior);
		if (error)
			goto out;
		start = (vma->rde$pq_start_va + vma->rde$q_region_size);
		vma = 0; //vma->vm_next;
	}

out:
	up_write(&current->mm->mmap_sem);
	return error;
}

/*
 * Later we can get more picky about what "in core" means precisely.
 * For now, simply check to see if the page is in the page cache,
 * and is up to date; i.e. that no page-in operation would be required
 * at this time if an application were to map and access this page.
 */
static unsigned char mincore_page(struct _rde * vma,
	unsigned long pgoff)
{
	unsigned char present = 0;
	struct address_space * as = 0;
	struct page * page;

	page = 0;
#if 0
	if ((page) && (Page_Uptodate(page)))
#endif
		present = 1;

	return present;
}

static long mincore_vma(struct _rde * vma,
	unsigned long start, unsigned long end, unsigned char * vec)
{
	long error, i, remaining;
	unsigned char * tmp;

	error = -ENOMEM;
#if 0
	if (!vma->vm_file)
		return error;
#endif

	start = ((start - (unsigned long)vma->rde$pq_start_va) >> PAGE_SHIFT);// + vma->vm_pgoff;
	if (end > (vma->rde$pq_start_va + vma->rde$q_region_size))
		end = (vma->rde$pq_start_va + vma->rde$q_region_size);
	end = ((end - (unsigned long)vma->rde$pq_start_va) >> PAGE_SHIFT);// + vma->vm_pgoff;

	error = -EAGAIN;
	tmp = (unsigned char *) __get_free_page(GFP_KERNEL);
	if (!tmp)
		return error;

	/* (end - start) is # of pages, and also # of bytes in "vec */
	remaining = (end - start),

	error = 0;
	for (i = 0; remaining > 0; remaining -= PAGE_SIZE, i++) {
		int j = 0;
		long thispiece = (remaining < PAGE_SIZE) ?
						remaining : PAGE_SIZE;

		while (j < thispiece)
			tmp[j++] = mincore_page(vma, start++);

		if (copy_to_user(vec + PAGE_SIZE * i, tmp, thispiece)) {
			error = -EFAULT;
			break;
		}
	}

	free_page((unsigned long) tmp);
	return error;
}

/*
 * The mincore(2) system call.
 *
 * mincore() returns the memory residency status of the pages in the
 * current process's address space specified by [addr, addr + len).
 * The status is returned in a vector of bytes.  The least significant
 * bit of each byte is 1 if the referenced page is in memory, otherwise
 * it is zero.
 *
 * Because the status of a page can change after mincore() checks it
 * but before it returns to the application, the returned vector may
 * contain stale information.  Only locked pages are guaranteed to
 * remain in memory.
 *
 * return values:
 *  zero    - success
 *  -EFAULT - vec points to an illegal address
 *  -EINVAL - addr is not a multiple of PAGE_CACHE_SIZE,
 *		or len has a nonpositive value
 *  -ENOMEM - Addresses in the range [addr, addr + len] are
 *		invalid for the address space of this process, or
 *		specify one or more pages which are not currently
 *		mapped
 *  -EAGAIN - A kernel resource was temporarily unavailable.
 */
asmlinkage long sys_mincore(unsigned long start, size_t len,
	unsigned char * vec)
{
	int index = 0;
	unsigned long end;
	struct _rde * vma;
	int unmapped_error = 0;
	long error = -EINVAL;

	down_read(&current->mm->mmap_sem);

	if (start & ~PAGE_CACHE_MASK)
		goto out;
	len = (len + ~PAGE_CACHE_MASK) & PAGE_CACHE_MASK;
	end = start + len;
	if (end < start)
		goto out;

	error = 0;
	if (end == start)
		goto out;

	/*
	 * If the interval [start,end) covers some unmapped address
	 * ranges, just ignore them, but return -ENOMEM at the end.
	 */
	//vma = find_vma(current->mm, start);
	vma = find_vma(current->pcb$l_phd,start);
	for (;;) {
		/* Still start < end. */
		error = -ENOMEM;
		if (!vma)
			goto out;

		/* Here start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (start < vma->rde$pq_start_va) {
			unmapped_error = -ENOMEM;
			start = vma->rde$pq_start_va;
		}

		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size). */
		if (end <= (vma->rde$pq_start_va + vma->rde$q_region_size)) {
			if (start < end) {
				error = mincore_vma(vma, start, end,
							&vec[index]);
				if (error)
					goto out;
			}
			error = unmapped_error;
			goto out;
		}

		/* Here vma->rde$pq_start_va <= start < (vma->rde$pq_start_va + vma->rde$q_region_size) < end. */
		error = mincore_vma(vma, start, (vma->rde$pq_start_va + vma->rde$q_region_size), &vec[index]);
		if (error)
			goto out;
		index += ((unsigned long)(vma->rde$pq_start_va + vma->rde$q_region_size) - start) >> PAGE_CACHE_SHIFT;
		start = (vma->rde$pq_start_va + vma->rde$q_region_size);
		vma = 0; //vma->vm_next;
	}

out:
	up_read(&current->mm->mmap_sem);
	return error;
}

inline void remove_suid(struct _fcb *inode)
{
#if 0
	unsigned int mode;

	/* set S_IGID if S_IXGRP is set, and always set S_ISUID */
	mode = (inode->i_mode & S_IXGRP)*(S_ISGID/S_IXGRP) | S_ISUID;

	/* was any of the uid bits set? */
	mode &= inode->i_mode;
	if (mode && !capable(CAP_FSETID)) {
		inode->i_mode &= ~mode;
		mark_inode_dirty(inode);
	}
#endif
}

/*
 * Write to a file through the page cache. 
 *
 * We currently put everything into the page cache prior to writing it.
 * This is not a problem when writing full pages. With partial pages,
 * however, we first have to read the data into the cache, then
 * dirty the page, and finally schedule it for writing. Alternatively, we
 * could write-through just the portion of data that would go into that
 * page, but that would kill performance for applications that write data
 * line by line, and it's prone to race conditions.
 *
 * Note that this routine doesn't try to keep track of dirty pages. Each
 * file system has to do this all by itself, unfortunately.
 *							okir@monad.swb.de
 */
ssize_t
generic_file_write(struct file *file,const char *buf,size_t count, loff_t *ppos)
{
	struct _fcb	*inode = file->f_dentry->d_inode;
	unsigned long	limit = current->rlim[RLIMIT_FSIZE].rlim_cur;
	loff_t		pos;
	struct page	*page, *cached_page;
	ssize_t		written;
	long		status = 0;
	int		err;
	unsigned	bytes;

	if ((ssize_t) count < 0)
		return -EINVAL;

	if (!access_ok(VERIFY_READ, buf, count))
		return -EFAULT;

	cached_page = NULL;

#if 0
	down(&inode->i_sem);
#endif

	pos = *ppos;
	err = -EINVAL;
	if (pos < 0)
		goto out;

	err = file->f_error;
	if (err) {
		file->f_error = 0;
		goto out;
	}

	written = 0;

	/* FIXME: this is for backwards compatibility with 2.4 */
	if (/*!S_ISBLK(inode->i_mode) &&*/ file->f_flags & O_APPEND)
		pos = inode->fcb$l_filesize;

	/*
	 * Check whether we've reached the file size limit.
	 */
	err = -EFBIG;
	
	if (limit != RLIM_INFINITY) {
		if (pos >= limit) {
			send_sig(SIGXFSZ, current, 0);
			goto out;
		}
		if (pos > 0xFFFFFFFFULL || count > limit - (u32)pos) {
			/* send_sig(SIGXFSZ, current, 0); */
			count = limit - (u32)pos;
		}
	}

	/*
	 *	LFS rule 
	 */
	if ( pos + count > MAX_NON_LFS && !(file->f_flags&O_LARGEFILE)) {
		if (pos >= MAX_NON_LFS) {
			send_sig(SIGXFSZ, current, 0);
			goto out;
		}
		if (count > MAX_NON_LFS - (u32)pos) {
			/* send_sig(SIGXFSZ, current, 0); */
			count = MAX_NON_LFS - (u32)pos;
		}
	}

	/*
	 *	Are we about to exceed the fs block limit ?
	 *
	 *	If we have written data it becomes a short write
	 *	If we have exceeded without writing data we send
	 *	a signal and give them an EFBIG.
	 *
	 *	Linus frestrict idea will clean these up nicely..
	 */
	 
	if (1/*!S_ISBLK(inode->i_mode)*/) {
#if 0
	  // not yet
		if (pos >= inode->i_sb->s_maxbytes)
		{
			if (count || pos > inode->i_sb->s_maxbytes) {
				send_sig(SIGXFSZ, current, 0);
				err = -EFBIG;
				goto out;
			}
			/* zero-length writes at ->s_maxbytes are OK */
		}

		if (pos + count > inode->i_sb->s_maxbytes)
			count = inode->i_sb->s_maxbytes - pos;
#endif
	} else {
#if 0
	  // not yet
		if (is_read_only(inode->i_rdev)) {
			err = -EPERM;
			goto out;
		}
#endif
		if (pos >= inode->fcb$l_filesize) {
			if (count || pos > inode->fcb$l_filesize) {
				err = -ENOSPC;
				goto out;
			}
		}

		if (pos + count > inode->fcb$l_filesize)
			count = inode->fcb$l_filesize - pos;
	}

	err = 0;
	if (count == 0)
		goto out;

	remove_suid(inode);
#if 0
	inode->i_ctime = inode->i_mtime = CURRENT_TIME;
#endif
	mark_inode_dirty_sync(inode);

	if (file->f_flags & O_DIRECT)
		goto o_direct;

	do {
		unsigned long index, offset;
		long page_fault;
		char *kaddr;

		/*
		 * Try to find the page in the cache. If it isn't there,
		 * allocate a free page.
		 */
		offset = (pos & (PAGE_CACHE_SIZE -1)); /* Within page */
		index = pos >> PAGE_CACHE_SHIFT;
		bytes = PAGE_CACHE_SIZE - offset;
		if (bytes > count)
			bytes = count;

		/*
		 * Bring in the user page that we will copy from _first_.
		 * Otherwise there's a nasty deadlock on copying from the
		 * same page as we're writing to, without it being marked
		 * up-to-date.
		 */
		{ volatile unsigned char dummy;
			__get_user(dummy, buf);
			__get_user(dummy, buf+bytes-1);
		}

		status = -ENOMEM;	/* we'll assign it later anyway */
		page = alloc_page(0);
		if (!page)
			break;

#if 0
		/* We have exclusive IO access to the page.. */
		if (!PageLocked(page)) {
			PAGE_BUG(page);
		}
#endif

		kaddr = kmap(page);
		status = block_prepare_write2(inode, page, offset, offset+bytes, index);
		if (status)
			goto sync_failure;
		page_fault = __copy_from_user(kaddr+offset, buf, bytes);
		flush_dcache_page(page);
		status = generic_commit_write2(inode, page, offset, offset+bytes, index);
		if (page_fault)
			goto fail_write;
		if (!status)
			status = bytes;

		if (status >= 0) {
			written += status;
			count -= status;
			pos += status;
			buf += status;
		}
unlock:
		kunmap(page);
		/* Mark it unlocked again and drop the page.. */
#if 0
		SetPageReferenced(page);
		UnlockPage(page);
#endif
		page_cache_release(page);

		if (status < 0)
			break;
	} while (count);
done:
	*ppos = pos;

	if (cached_page)
		page_cache_release(cached_page);

	/* For now, when the user asks for O_SYNC, we'll actually
	 * provide O_DSYNC. */
#if 0
	if (status >= 0) {
	  if ((file->f_flags & O_SYNC) /*|| IS_SYNC(inode)*/)
			status = generic_osync_inode(inode, OSYNC_METADATA|OSYNC_DATA);
	}
#endif
	
out_status:	
	err = written ? written : status;
out:

#if 0
	up(&inode->i_sem);
#endif
	return err;
fail_write:
	status = -EFAULT;
	goto unlock;

sync_failure:
	/*
	 * If blocksize < pagesize, prepare_write() may have instantiated a
	 * few blocks outside fcb$l_filesize.  Trim these off again.
	 */
	kunmap(page);
#if 0
	UnlockPage(page);
#endif
	page_cache_release(page);
	if (pos + bytes > inode->fcb$l_filesize)
		vmtruncate(inode, inode->fcb$l_filesize);
	goto done;

o_direct:
	written = generic_file_direct_IO(WRITE, file, (char *) buf, count, pos);
	if (written > 0) {
		loff_t end = pos + written;
		if (end > inode->fcb$l_filesize && 1/*!S_ISBLK(inode->i_mode)*/) {
			inode->fcb$l_filesize = end;
			mark_inode_dirty(inode);
		}
		*ppos = end;
		//invalidate_inode_pages2();
	}
	/*
	 * Sync the fs metadata but not the minor inode changes and
	 * of course not the data as we did direct DMA for the IO.
	 */
#if 0
	if (written >= 0 && file->f_flags & O_SYNC)
		status = generic_osync_inode(inode, OSYNC_METADATA);
#endif
	goto out_status;
}

void __init page_cache_init(unsigned long mempages)
{
	printk("%%KERNEL-I-ISNOMORE, Linux Page-cache is no longer used\n");
}
