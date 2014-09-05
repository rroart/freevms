// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on mmap.c.

/*
 *	linux/mm/mmap.c
 *
 * Written by obz.
 */
#include <linux/config.h>
#include <linux/slab.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/personality.h>

#include <asm/uaccess.h>
#include <asm/pgalloc.h>

#include <ipldef.h>
#include <phddef.h>
#include <rdedef.h>
#include <va_rangedef.h>
#include <dyndef.h>
#include <queue.h>
#include <misc_routines.h>
#include <exe_routines.h>
#include <rabdef.h>
#include <fabdef.h>

struct vm_area_struct * find_vma_intersection2(struct mm_struct * mm, unsigned long start_addr, unsigned long end_addr);

/*
 * WARNING: the debugging will use recursive algorithms so never enable this
 * unless you know what you are doing.
 */
#undef DEBUG_MM_RB

/* description of effects of mapping type and prot in current implementation.
 * this is due to the limited x86 page protection hardware.  The expected
 * behavior is in parens:
 *
 * map_type	prot
 *		PROT_NONE	PROT_READ	PROT_WRITE	PROT_EXEC
 * MAP_SHARED	r: (no) no	r: (yes) yes	r: (no) yes	r: (no) yes
 *		w: (no) no	w: (no) no	w: (yes) yes	w: (no) no
 *		x: (no) no	x: (no) yes	x: (no) yes	x: (yes) yes
 *		
 * MAP_PRIVATE	r: (no) no	r: (yes) yes	r: (no) yes	r: (no) yes
 *		w: (no) no	w: (no) no	w: (copy) copy	w: (no) no
 *		x: (no) no	x: (no) yes	x: (no) yes	x: (yes) yes
 *
 */
pgprot_t protection_map[16] = {
	__P000, __P001, __P010, __P011, __P100, __P101, __P110, __P111,
	__S000, __S001, __S010, __S011, __S100, __S101, __S110, __S111
};

int sysctl_overcommit_memory;

/* Check that a process has enough memory to allocate a
 * new virtual mapping.
 */
int vm_enough_memory(long pages)
{
	/* Stupid algorithm to decide if we have enough memory: while
	 * simple, it hopefully works in most obvious cases.. Easy to
	 * fool it, but this should catch most mistakes.
	 */
	/* 23/11/98 NJC: Somewhat less stupid version of algorithm,
	 * which tries to do "TheRightThing".  Instead of using half of
	 * (buffers+cache), use the minimum values.  Allow an extra 2%
	 * of num_physpages for safety margin.
	 */

	unsigned long free;
	
        /* Sometimes we want to use more memory than we have. */
	if (sysctl_overcommit_memory)
	    return 1;

	/* The page cache contains buffer pages these days.. */
	extern unsigned int nr_free_pages(void);
	extern int nr_swap_pages;
	free = 0;//atomic_read(&page_cache_size);
	free += nr_free_pages();
	free += nr_swap_pages;

	/*
	 * This double-counts: the nrpages are both in the page-cache
	 * and in the swapper space. At the same time, this compensates
	 * for the swap-space over-allocation (ie "nr_swap_pages" being
	 * too small.
	 */
	free += swapper_space.nrpages;

	/*
	 * The code below doesn't account for free space in the inode
	 * and dentry slab cache, slab cache fragmentation, inodes and
	 * dentries which will become freeable under VM load, etc.
	 * Lets just hope all these (complex) factors balance out...
	 */
#if 0
	free += (dentry_stat.nr_unused * sizeof(struct dentry)) >> PAGE_SHIFT;
	free += (inodes_stat.nr_unused * sizeof(struct inode)) >> PAGE_SHIFT;
#endif

	return free > pages;
}

void lock_vma_mappings(struct vm_area_struct *vma)
{
	struct address_space *mapping;

	mapping = NULL;
#if 0
	if (vma->vm_file)
		mapping = vma->vm_file->f_dentry->d_inode->i_mapping;
#endif
	if (mapping)
		spin_lock(&mapping->i_shared_lock);
}

void unlock_vma_mappings(struct vm_area_struct *vma)
{
	struct address_space *mapping;

	mapping = NULL;
#if 0
	if (vma->vm_file)
		mapping = vma->vm_file->f_dentry->d_inode->i_mapping;
#endif
	if (mapping)
		spin_unlock(&mapping->i_shared_lock);
}

/*
 *  sys_brk() for the most part doesn't need the global kernel
 *  lock, except when an application is doing something nasty
 *  like trying to un-brk an area that has already been mapped
 *  to a regular file.  in this case, the unmapping will need
 *  to invoke file system routines that need the global lock.
 */
asmlinkage unsigned long sys_brk(unsigned long brk)
{
	unsigned long rlim, retval;
	unsigned long newbrk, oldbrk;
	struct mm_struct *mm = current->mm;

	down_write(&mm->mmap_sem);

	if (brk < mm->end_code)
		goto out;
	newbrk = PAGE_ALIGN(brk);
	oldbrk = PAGE_ALIGN(mm->brk);
	if (oldbrk == newbrk)
		goto set_brk;

	/* Always allow shrinking brk. */
	if (brk <= mm->brk) {
		if (!do_munmap(mm, newbrk, oldbrk-newbrk))
			goto set_brk;
		goto out;
	}

	/* Check against rlimit.. */
	rlim = current->rlim[RLIMIT_DATA].rlim_cur;
	if (rlim < RLIM_INFINITY && brk - mm->start_data > rlim)
		goto out;

	/* Check against existing mmap mappings. */
	if (find_vma_intersection2(current->pcb$l_phd, oldbrk, newbrk+PAGE_SIZE))
		goto out;

	/* Check if we have enough memory.. */
	if (!vm_enough_memory((newbrk-oldbrk) >> PAGE_SHIFT))
		goto out;

	/* Ok, looks good - let it rip. */
	if (do_brk(oldbrk, newbrk-oldbrk) != oldbrk)
		goto out;
set_brk:
	mm->brk = brk;
out:
	retval = mm->brk;
	up_write(&mm->mmap_sem);
	return retval;
}

/* Combine the mmap "prot" and "flags" argument into one "rde$l_flags" used
 * internally. Essentially, translate the "PROT_xxx" and "MAP_xxx" bits
 * into "VM_xxx".
 */
static inline unsigned long calc_rde$l_flags(unsigned long prot, unsigned long flags)
{
#define _trans(x,bit1,bit2) \
((bit1==bit2)?(x&bit1):(x&bit1)?bit2:0)

	unsigned long prot_bits, flag_bits;
	prot_bits =
		_trans(prot, PROT_READ, VM_READ) |
		_trans(prot, PROT_WRITE, VM_WRITE) |
		_trans(prot, PROT_EXEC, VM_EXEC);
	flag_bits =
		_trans(flags, MAP_GROWSDOWN, VM_GROWSDOWN) |
		_trans(flags, MAP_DENYWRITE, VM_DENYWRITE) |
		_trans(flags, MAP_EXECUTABLE, VM_EXECUTABLE);
	return (prot_bits | flag_bits);
#undef _trans
}

#ifdef DEBUG_MM_RB
static int browse_rb(rb_node_t * rb_node) {
	int i = 0;
	if (rb_node) {
		i++;
		i += browse_rb(rb_node->rb_left);
		i += browse_rb(rb_node->rb_right);
	}
	return i;
}

static void validate_mm(struct mm_struct * mm) {
	int bug = 0;
	int i = 0;
	struct _rde * tmp = mm->mmap;
	while (tmp) {
		tmp = tmp->vm_next;
		i++;
	}
	if (i != mm->map_count)
		printk("map_count %d vm_next %d\n", mm->map_count, i), bug = 1;
	i = browse_rb(mm->mm_rb.rb_node);
	if (i != mm->map_count)
		printk("map_count %d rb %d\n", mm->map_count, i), bug = 1;
	if (bug)
		BUG();
}
#else
#define validate_mm(mm) do { } while (0)
#endif

#if 0
static int vma_merge(struct mm_struct * mm, struct vm_area_struct * prev,
		     rb_node_t * rb_parent, unsigned long addr, unsigned long end, unsigned long vm_flags)
{
	spinlock_t * lock = &mm->page_table_lock;
	if (prev->vm_end == addr && can_vma_merge(prev, vm_flags)) {
		struct vm_area_struct * next;

		spin_lock(lock);
		prev->vm_end = end;
		next = prev->vm_next;
		if (next && prev->vm_end == next->vm_start && can_vma_merge(next, vm_flags)) {
			prev->vm_end = next->vm_end;
			__vma_unlink(mm, next, prev);
			spin_unlock(lock);

			mm->map_count--;
			kmem_cache_free(vm_area_cachep, next);
			return 1;
		}
		spin_unlock(lock);
		return 1;
	}

	prev = prev->vm_next;
	if (prev) {
 merge_next:
		if (!can_vma_merge(prev, vm_flags))
			return 0;
		if (end == prev->vm_start) {
			spin_lock(lock);
			prev->vm_start = addr;
			spin_unlock(lock);
			return 1;
		}
	}

	return 0;
}
#endif

unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len,
	unsigned long prot, unsigned long flags, unsigned long pgoff)
{
	struct mm_struct * mm = current->mm;
	struct _rde * vma, * prev;
	unsigned int rde$l_flags;
	int correct_wcount = 0;
	int error;
	rb_node_t ** rb_link, * rb_parent;
	struct _va_range inadr;

#if 0
	if (file && ((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)
	  if (file && (!file->f_op || !file->f_op->mmap))
	    return -ENODEV;
#endif

	if ((len = PAGE_ALIGN(len)) == 0)
		return addr;

	if (len > TASK_SIZE)
		return -EINVAL;

	/* offset overflow? */
	if ((pgoff + (len >> PAGE_SHIFT)) < pgoff)
		return -EINVAL;

	/* Too many mappings? */
	if (mm->map_count > MAX_MAP_COUNT)
		return -ENOMEM;

	/* Obtain the address to map to. we verify (or select) it and ensure
	 * that it represents a valid section of the address space.
	 */
	addr = get_unmapped_area(file, addr, len, pgoff, flags);
	if (addr & ~PAGE_MASK)
		return addr;

#if 0
munmap_back2:
	//vma = find_vma_prepare(mm, addr, &prev, &rb_link, &rb_parent);
	vma = find_vma_prev(current->pcb$l_phd, addr, &prev);
	if (vma && vma->rde$pq_start_va < addr + len) {
		if (do_munmap(mm, addr, len))
			return -ENOMEM;
		goto munmap_back2;
	}
#endif

	/* Do simple checking here so the lower-level routines won't have
	 * to. we assume access permissions have been handled by the open
	 * of the memory object, so we don't do any here.
	 */
	rde$l_flags = calc_rde$l_flags(prot,flags) | mm->def_flags | VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC;

	/* mlock MCL_FUTURE? */
	if (rde$l_flags & VM_LOCKED) {
		unsigned long locked = mm->locked_vm << PAGE_SHIFT;
		locked += len;
		if (locked > current->rlim[RLIMIT_MEMLOCK].rlim_cur)
			return -EAGAIN;
	}

	if (file) {
	  if (0/*((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB*/) {
		switch (flags & MAP_TYPE) {
		case MAP_SHARED:
			if ((prot & PROT_WRITE) && !(file->f_mode & FMODE_WRITE))
				return -EACCES;

			/* Make sure we don't allow writing to an append-only file.. */
			if (IS_APPEND(file->f_dentry->d_inode) && (file->f_mode & FMODE_WRITE))
				return -EACCES;

			/* make sure there are no mandatory locks on the file. */
			if (locks_verify_locked(file->f_dentry->d_inode))
				return -EAGAIN;

			rde$l_flags |= VM_SHARED | VM_MAYSHARE;
			if (!(file->f_mode & FMODE_WRITE))
				rde$l_flags &= ~(VM_MAYWRITE | VM_SHARED);

			/* fall through */
		case MAP_PRIVATE:
			if (!(file->f_mode & FMODE_READ))
				return -EACCES;
			break;

		default:
			return -EINVAL;
		}
	  }
	} else {
		rde$l_flags |= VM_SHARED | VM_MAYSHARE;
		switch (flags & MAP_TYPE) {
		default:
			return -EINVAL;
		case MAP_PRIVATE:
			rde$l_flags &= ~(VM_SHARED | VM_MAYSHARE);
			/* fall through */
		case MAP_SHARED:
			break;
		}
	}

	/* Clear old maps */
	error = -ENOMEM;
munmap_back:
	//vma = find_vma_prepare(mm, addr, &prev, &rb_link, &rb_parent);
	vma = find_vma(current->pcb$l_phd, addr);
	if (vma && vma->rde$pq_start_va < addr + len) {
		if (do_munmap(mm, addr, len))
			return -ENOMEM;
		goto munmap_back;
	}

	/* Check against address space limit. */
	if ((mm->total_vm << PAGE_SHIFT) + len
	    > current->rlim[RLIMIT_AS].rlim_cur)
		return -ENOMEM;

	/* Private writable mapping? Check memory availability.. */
	if ((rde$l_flags & (VM_SHARED | VM_WRITE)) == VM_WRITE &&
	    !(flags & MAP_NORESERVE)				 &&
	    !vm_enough_memory(len >> PAGE_SHIFT))
		return -ENOMEM;

	/* Can we just expand an old anonymous mapping? */
#if 0
	if (!file && !(rde$l_flags & VM_SHARED) && vma /*rb_parent*/)
		if (vma_merge(mm, prev, rb_parent, addr, addr + len, rde$l_flags))
			goto out;
#endif

	inadr.va_range$ps_start_va=addr;
	inadr.va_range$ps_end_va=addr+len;
	prot=prot; //translate later?
	flags=flags; //translate later?

	exe$create_region_32 (len,*(unsigned long*)&protection_map[(rde$l_flags>>8) & 0x0f] ,rde$l_flags   ,0,0,0,addr);
	if (file) {
	  struct _fcb * fcb=file;
#if 0
	  if (((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)	  
	    fcb=e2_search_fcb(file->f_dentry->d_inode);
#endif
	  struct _rabdef * rab = file;
	  struct _fabdef * fab = rab->rab$l_fab;
	  int chan = fab->fab$l_stv;
	  exe$crmpsc(&inadr,0,0,0,0,0,0,/*(unsigned short int)*/chan,0,pgoff<<3,0,0);
	} else {
	  exe$cretva(&inadr,0,0);
	}
	return addr;

	/* Determine the object being mapped and call the appropriate
	 * specific mapper. the address has already been validated, but
	 * not unmapped, but the maps are removed from the list.
	 */
	vma = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!vma)
		return -ENOMEM;

	//vma->vm_mm = mm;
	vma->rde$pq_start_va = addr;
	vma->rde$q_region_size = len;
	vma->rde$l_flags = rde$l_flags;
	vma->rde$r_regprot.regprt$l_region_prot = *(unsigned long*)&protection_map[(rde$l_flags>>8) & 0x0f];
#if 0
	vma->vm_ops = NULL;
	vma->vm_pgoff = pgoff;
	vma->vm_file = NULL;
	vma->vm_private_data = NULL;
	vma->vm_raend = 0;
#endif

	if (file) {
		error = -EINVAL;
		if (rde$l_flags & (VM_GROWSDOWN|VM_GROWSUP))
			goto free_vma;
		if (rde$l_flags & VM_DENYWRITE) {
			error = deny_write_access(file);
			if (error)
				goto free_vma;
			correct_wcount = 1;
		}
		//vma->vm_file = file;
		get_file(file);
#if 0
		error = file->f_op->mmap(file, vma);
#endif
		if (error)
			goto unmap_and_free_vma;
	} else if (flags & MAP_SHARED) {
		error = shmem_zero_setup(vma);
		if (error)
			goto free_vma;
	}

	/* Can addr have changed??
	 *
	 * Answer: Yes, several device drivers can do it in their
	 *         f_op->mmap method. -DaveM
	 */
	addr = vma->rde$pq_start_va;

#if 0
	//vma_link(mm, vma, prev, rb_link, rb_parent);
	if (((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)
	  if (correct_wcount)
	    atomic_inc(&file->f_dentry->d_inode->i_writecount);
#endif

out:	
	mm->total_vm += len >> PAGE_SHIFT;
	if (rde$l_flags & VM_LOCKED) {
		mm->locked_vm += len >> PAGE_SHIFT;
		make_pages_present(addr, addr + len);
	}
	return addr;

unmap_and_free_vma:
#if 0
	if (((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)
	  if (correct_wcount)
	    atomic_inc(&file->f_dentry->d_inode->i_writecount);
	//vma->vm_file = NULL;
	if (((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)
	  fput(file);
#endif

	/* Undo any partial mapping done by a device driver. */
	zap_page_range(mm, vma->rde$pq_start_va, vma->rde$q_region_size);
free_vma:
	kmem_cache_free(vm_area_cachep, vma);
	return error;
}

/* Get an address range which is currently unmapped.
 * For shmat() with addr=0.
 *
 * Ugly calling convention alert:
 * Return value with the low bits set means error value,
 * ie
 *	if (ret & ~PAGE_MASK)
 *		error = ret;
 *
 * This function "knows" that -ENOMEM has the bits set.
 */
#ifndef HAVE_ARCH_UNMAPPED_AREA
static inline unsigned long arch_get_unmapped_area(struct file *filp, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags)
{
	struct _rde *vma;

	if (len > TASK_SIZE)
		return -ENOMEM;

	if (addr) {
		addr = PAGE_ALIGN(addr);
		//vma = find_vma(current->mm, addr);
		vma = find_vma(current->pcb$l_phd,addr);
		if (TASK_SIZE - len >= addr &&
		    (!vma || addr + len <= vma->rde$pq_start_va))
			return addr;
	}
	addr = PAGE_ALIGN(TASK_UNMAPPED_BASE);

	for (vma = find_vma(current->pcb$l_phd,addr); ; vma = vma->rde$ps_va_list_flink) {
		/* At this point:  (!vma || addr < (vma->rde$pq_start_va + vma->rde$q_region_size)). */
		if (TASK_SIZE - len < addr)
			return -ENOMEM;
		if (!vma || addr + len <= vma->rde$pq_start_va)
			return addr;
		addr = (vma->rde$pq_start_va + vma->rde$q_region_size);
	}
}
#else
extern unsigned long arch_get_unmapped_area(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
#endif	

unsigned long get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags)
{
	if (flags & MAP_FIXED) {
		if (addr > TASK_SIZE - len)
			return -ENOMEM;
		if (addr & ~PAGE_MASK)
			return -EINVAL;
		return addr;
	}

#if 0
	if (file && ((struct _fcb *)file)->fcb$b_type!=DYN$C_FCB)
	  if (file && file->f_op && file->f_op->get_unmapped_area)
	    return file->f_op->get_unmapped_area(file, addr, len, pgoff, flags);
#endif

	return arch_get_unmapped_area(file, addr, len, pgoff, flags);
}

struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr) {
  struct _phd * phd=mm;
  struct _rde * head=&phd->phd$ps_p0_va_list_flink;
  struct _rde * tmp=head->rde$ps_va_list_flink;
  struct _rde * prev=head;
  while (tmp!=head) {
    if (addr<(tmp->rde$ps_start_va+(unsigned long)tmp->rde$q_region_size)) 
      return tmp;
    prev=tmp;
    tmp=tmp->rde$ps_va_list_flink;
  }
  return 0;
}

struct vm_area_struct * find_vma_prev(struct mm_struct * mm, unsigned long addr, struct vm_area_struct **prev) {
  struct _phd * phd=mm;
  struct _rde * head=&phd->phd$ps_p0_va_list_flink;
  struct _rde * tmp=head->rde$ps_va_list_flink;
  *prev=0;
  while (tmp!=head) {
    if (addr<(tmp->rde$ps_start_va+(unsigned long)tmp->rde$q_region_size)) 
      return tmp;
    *prev=tmp;
    tmp=tmp->rde$ps_va_list_flink;
  }
  return 0;
}

struct vm_area_struct * find_extend_vma(struct mm_struct * mm, unsigned long addr)
{
	struct _rde * vma;
	unsigned long start;

	addr &= PAGE_MASK;
	vma = find_vma(current->pcb$l_phd,addr);
	if (!vma)
		return NULL;
	if (vma->rde$ps_start_va <= addr)
		return vma;
	if (!(vma->rde$l_flags & VM_GROWSDOWN))
		return NULL;
	start = vma->rde$ps_start_va;
	if (expand_stack(vma, addr))
		return NULL;
	if (vma->rde$l_flags & VM_LOCKED) {
		make_pages_present(addr, start);
	}
	return vma;
}

struct vm_area_struct * find_vma_intersection2(struct mm_struct * mm, unsigned long start_addr, unsigned long end_addr)
{
  struct _rde * vma = find_vma(mm,start_addr);

  if (vma && end_addr <= vma->rde$ps_start_va)
    vma = NULL;
  return vma;
}

/* Normal function to fix up a mapping
 * This function is the default for when an area has no specific
 * function.  This may be used as part of a more specific routine.
 * This function works out what part of an area is affected and
 * adjusts the mapping information.  Since the actual page
 * manipulation is done in do_mmap(), none need be done here,
 * though it would probably be more appropriate.
 *
 * By the time this function is called, the area struct has been
 * removed from the process mapping list, so it needs to be
 * reinserted if necessary.
 *
 * The 4 main cases are:
 *    Unmapping the whole area
 *    Unmapping from the start of the segment to a point in it
 *    Unmapping from an intermediate point to the end
 *    Unmapping between to intermediate points, making a hole.
 *
 * Case 4 involves the creation of 2 new areas, for each side of
 * the hole.  If possible, we reuse the existing area rather than
 * allocate a new one, and the return indicates whether the old
 * area was reused.
 */
static struct _rde * unmap_fixup(struct mm_struct *mm, 
	struct _rde *area, unsigned long addr, size_t len, 
	struct _rde *extra)
{
	struct _rde *mpnt;
	unsigned long end = addr + len;

#if 0
	area->vm_mm->total_vm -= len >> PAGE_SHIFT;
	if (area->rde$l_flags & VM_LOCKED)
		area->vm_mm->locked_vm -= len >> PAGE_SHIFT;
#endif

	/* Unmapping the whole area. */
	if (addr == area->rde$pq_start_va && end == (area->rde$pq_start_va + area->rde$q_region_size)) {
#if 0
		if (area->vm_ops && area->vm_ops->close)
			area->vm_ops->close(area);
		if (area->vm_file)
			fput(area->vm_file);
#endif
		//kmem_cache_free(vm_area_cachep, area); //not yet
		return extra;
	}

	/* Work out to one of the ends. */
	if (end == (area->rde$pq_start_va + area->rde$q_region_size)) {
		/*
		 * here area isn't visible to the semaphore-less readers
		 * so we don't need to update it under the spinlock.
		 */
		area->rde$q_region_size = addr - (unsigned long) area->rde$pq_start_va;
		lock_vma_mappings(area);
		spin_lock(&mm->page_table_lock);
	} else if (addr == area->rde$pq_start_va) {
	  //area->vm_pgoff += (end - area->rde$pq_start_va) >> PAGE_SHIFT;
		/* same locking considerations of the above case */
		area->rde$pq_start_va = end;
		area->rde$q_region_size -= len;
		lock_vma_mappings(area);
		spin_lock(&mm->page_table_lock);
	} else {
	/* Unmapping a hole: area->rde$pq_start_va < addr <= end < (area->rde$pq_start_va + area->rde$q_region_size) */
		/* Add end mapping -- leave beginning for below */
		mpnt = extra;
		extra = NULL;

		//mpnt->vm_mm = area->vm_mm;
		mpnt->rde$pq_start_va = end;
		mpnt->rde$q_region_size = area->rde$q_region_size - len;
		mpnt->rde$r_regprot.regprt$l_region_prot = area->rde$r_regprot.regprt$l_region_prot;
		mpnt->rde$l_flags = area->rde$l_flags;
		//mpnt->vm_raend = 0;
		//mpnt->vm_ops = area->vm_ops;
		//mpnt->vm_pgoff = area->vm_pgoff + ((end - area->rde$pq_start_va) >> PAGE_SHIFT);
		//mpnt->vm_file = area->vm_file;
#if 0
		mpnt->vm_private_data = area->vm_private_data;
		if (mpnt->vm_file)
			get_file(mpnt->vm_file);
		if (mpnt->vm_ops && mpnt->vm_ops->open)
			mpnt->vm_ops->open(mpnt);
#endif
		area->rde$q_region_size = addr - (unsigned long) area->rde$pq_start_va;	/* Truncate area */

		/* Because mpnt->vm_file == area->vm_file this locks
		 * things correctly.
		 */
		lock_vma_mappings(area);
		spin_lock(&mm->page_table_lock);
		//__insert_vm_struct(mm, mpnt);
		insrde(mpnt,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	}

	//__insert_vm_struct(mm, area);
	insrde(area,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
	spin_unlock(&mm->page_table_lock);
	unlock_vma_mappings(area);
	return extra;
}

/*
 * Try to free as many page directory entries as we can,
 * without having to work very hard at actually scanning
 * the page tables themselves.
 *
 * Right now we try to free page tables if we have a nice
 * PGDIR-aligned area that got free'd up. We could be more
 * granular if we want to, but this is fast and simple,
 * and covers the bad cases.
 *
 * "prev", if it exists, points to a vma before the one
 * we just free'd - but there's no telling how much before.
 */
static void free_pgtables(struct mm_struct * mm, struct _rde *prev,
	unsigned long start, unsigned long end)
{
	unsigned long first = start & PGDIR_MASK;
	unsigned long last = end + PGDIR_SIZE - 1;
	unsigned long start_index, end_index;

	if (!prev) {
		prev = mm->mmap;
		if (!prev)
			goto no_mmaps;
		if ((prev->rde$pq_start_va + prev->rde$q_region_size) > start) {
			if (last > prev->rde$pq_start_va)
				last = prev->rde$pq_start_va;
			goto no_mmaps;
		}
	}
	for (;;) {
	  struct _rde *next = 0;//prev->vm_next;
	  next = prev->rde$ps_va_list_flink;

	  if (next && next->rde$b_type==30 /*fix later*/) {
			if (next->rde$pq_start_va < start) {
				prev = next;
				continue;
			}
			if (last > next->rde$pq_start_va)
				last = next->rde$pq_start_va;
		}
		if ((prev->rde$pq_start_va + prev->rde$q_region_size) > first)
			first = (prev->rde$pq_start_va + prev->rde$q_region_size) + PGDIR_SIZE - 1;
		break;
	}
no_mmaps:
	/*
	 * If the PGD bits are not consecutive in the virtual address, the
	 * old method of shifting the VA >> by PGDIR_SHIFT doesn't work.
	 */
	start_index = pgd_index(first);
	end_index = pgd_index(last);
	if (end_index > start_index) {
		clear_page_tables(mm, start_index, end_index - start_index);
		flush_tlb_pgtables(mm, first & PGDIR_MASK, last & PGDIR_MASK);
	}
}

/* Munmap is split into 2 main parts -- this part which finds
 * what needs doing, and the areas themselves, which do the
 * work.  This now handles partial unmappings.
 * Jeremy Fitzhardine <jeremy@sw.oz.au>
 */
int do_munmap(struct mm_struct *mm, unsigned long addr, size_t len)
{
	struct _rde *mpnt, *prev, **npp, *free, *extra;

	if ((addr & ~PAGE_MASK) || addr > TASK_SIZE || len > TASK_SIZE-addr)
		return -EINVAL;

	if ((len = PAGE_ALIGN(len)) == 0)
		return -EINVAL;

	/* Check if this memory area is ok - put it on the temporary
	 * list if so..  The checks here are pretty simple --
	 * every area affected in some way (by any overlap) is put
	 * on the list.  If nothing is put on, nothing is affected.
	 */
	//mpnt = find_vma_prev(mm, addr, &prev);
	mpnt = find_vma_prev(current->pcb$l_phd,addr,&prev);
	if (!mpnt)
		return 0;
	/* we have  addr < (mpnt->rde$pq_start_va + mpnt->rde$q_region_size)  */

	if (mpnt->rde$pq_start_va >= addr+len)
		return 0;

	/* If we'll make "hole", check the vm areas limit */
	if ((mpnt->rde$pq_start_va < addr && (mpnt->rde$pq_start_va + mpnt->rde$q_region_size) > addr+len)
	    && mm->map_count >= MAX_MAP_COUNT)
		return -ENOMEM;

	/*
	 * We may need one additional vma to fix up the mappings ... 
	 * and this is the last chance for an easy error exit.
	 */
	extra = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!extra)
		return -ENOMEM;

	npp = (prev ? &prev->rde$ps_va_list_flink : &current->pcb$l_phd->phd$ps_p0_va_list_flink);
	free = NULL;
	spin_lock(&mm->page_table_lock);
	for ( ; mpnt && mpnt->rde$pq_start_va < addr+len; mpnt = *npp) {
	        //*npp = mpnt->rde$ps_va_list_flink;
		remque(mpnt,0);
		mpnt->rde$ps_va_list_flink = free;
		free = mpnt;
		//rb_erase(&mpnt->vm_rb, &mm->mm_rb);
	}
	mm->mmap_cache = NULL;	/* Kill the cache. */
	spin_unlock(&mm->page_table_lock);

	/* Ok - we have the memory areas we should free on the 'free' list,
	 * so release them, and unmap the page range..
	 * If the one of the segments is only being partially unmapped,
	 * it will put new _rde(s) into the address space.
	 * In that case we have to be careful with VM_DENYWRITE.
	 */
	while ((mpnt = free) != NULL) {
		unsigned long st, end, size;
		struct file *file = NULL;

		free = free->rde$ps_va_list_flink;

		st = addr < mpnt->rde$pq_start_va ? mpnt->rde$pq_start_va : addr;
		end = addr+len;
		end = end > (mpnt->rde$pq_start_va + mpnt->rde$q_region_size) ? (mpnt->rde$pq_start_va + mpnt->rde$q_region_size) : end;
		size = end - st;

		if (mpnt->rde$l_flags & VM_DENYWRITE &&
		    (st != mpnt->rde$pq_start_va || end != (mpnt->rde$pq_start_va + mpnt->rde$q_region_size)) && 1) {
#if 0
		    (file = mpnt->vm_file) != NULL) {
			atomic_dec(&file->f_dentry->d_inode->i_writecount);
#endif
		}
		//remove_shared_vm_struct(mpnt);
		mm->map_count--;

		zap_page_range(mm, st, size);

		/*
		 * Fix the mapping, and free the old area if it wasn't reused.
		 */
		extra = unmap_fixup(mm, mpnt, st, size, extra);
#if 0
		if (file)
			atomic_inc(&file->f_dentry->d_inode->i_writecount);
#endif
	}
	//validate_mm(mm);

	/* Release the extra vma struct if it wasn't used */
#if 0 // not yet
	if (extra)
		kmem_cache_free(vm_area_cachep, extra);
#endif

	free_pgtables(mm, prev, addr, addr+len);

	return 0;
}

asmlinkage long sys_munmap(unsigned long addr, size_t len)
{
	int ret;
	struct mm_struct *mm = current->mm;

	down_write(&mm->mmap_sem);
	ret = do_munmap(mm, addr, len);
	up_write(&mm->mmap_sem);
	return ret;
}

/*
 *  this is really a simplified "do_mmap".  it only handles
 *  anonymous maps.  eventually we may be able to do some
 *  brk-specific accounting here.
 */
unsigned long do_brk(unsigned long addr, unsigned long len)
{
	struct mm_struct * mm = current->mm;
	struct _rde * vma, * prev;
	unsigned long flags;
	rb_node_t ** rb_link, * rb_parent;

	len = PAGE_ALIGN(len);
	if (!len)
		return addr;

	/*
	 * mlock MCL_FUTURE?
	 */
	if (mm->def_flags & VM_LOCKED) {
		unsigned long locked = mm->locked_vm << PAGE_SHIFT;
		locked += len;
		if (locked > current->rlim[RLIMIT_MEMLOCK].rlim_cur)
			return -EAGAIN;
	}

	/*
	 * Clear old maps.  this also does some error checking for us
	 */
 munmap_back:
	//vma = find_vma_prepare(mm, addr, &prev, &rb_link, &rb_parent);
	vma = find_vma(current->pcb$l_phd, addr);
	if (vma && vma->rde$pq_start_va < addr + len) {
		if (do_munmap(mm, addr, len))
			return -ENOMEM;
		goto munmap_back;
	}

	/* Check against address space limits *after* clearing old maps... */
	if ((mm->total_vm << PAGE_SHIFT) + len
	    > current->rlim[RLIMIT_AS].rlim_cur)
		return -ENOMEM;

	if (mm->map_count > MAX_MAP_COUNT)
		return -ENOMEM;

	if (!vm_enough_memory(len >> PAGE_SHIFT))
		return -ENOMEM;

	flags = calc_rde$l_flags(PROT_READ|PROT_WRITE|PROT_EXEC,
				MAP_FIXED|MAP_PRIVATE) | mm->def_flags;

	flags |= VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC;

	/* Can we just expand an old anonymous mapping? */
#if 0
	if (rb_parent && vma_merge(mm, prev, rb_parent, addr, addr + len, flags))
		goto out;
#endif

	/*
	 * create a vma struct for an anonymous mapping
	 */
	vma = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!vma)
		return -ENOMEM;

	//vma->vm_mm = mm;
	vma->rde$pq_start_va = addr;
	vma->rde$q_region_size = len;
	vma->rde$l_flags = flags;
	vma->rde$r_regprot.regprt$l_region_prot = *(unsigned long*)&protection_map[(flags>>8) & 0x0f];
#if 0
	vma->vm_ops = NULL;
	vma->vm_pgoff = 0;
	vma->vm_file = NULL;
	vma->vm_private_data = NULL;
#endif

	//vma_link(mm, vma, prev, rb_link, rb_parent);
	insrde(vma,&current->pcb$l_phd->phd$ps_p0_va_list_flink);

out:
	mm->total_vm += len >> PAGE_SHIFT;
	if (flags & VM_LOCKED) {
		mm->locked_vm += len >> PAGE_SHIFT;
		make_pages_present(addr, addr + len);
	}
	return addr;
}

/* Release all mmaps. */
void exit_mmap(struct mm_struct * mm)
{
	struct _rde * mpnt;

	release_segments(mm);
	spin_lock(&mm->page_table_lock);
	mpnt = current->pcb$l_phd->phd$ps_p0_va_list_flink;
	//	mpnt = 0;
	// find right phd, otherwise we have got some leak
	mm->mmap = mm->mmap_cache = NULL;
	mm->mm_rb = RB_ROOT;
	mm->rss = 0;
	spin_unlock(&mm->page_table_lock);
	mm->total_vm = 0;
	mm->locked_vm = 0;

	flush_cache_mm(mm);
	//while(0) {
	while (mpnt!=&current->pcb$l_phd->phd$ps_p0_va_list_flink) {
		struct _rde * next = mpnt->rde$ps_va_list_flink;
		unsigned long start = mpnt->rde$pq_start_va;
		unsigned long size = mpnt->rde$q_region_size;

#if 0
		if (mpnt->vm_ops) {
			if (mpnt->vm_ops->close)
				mpnt->vm_ops->close(mpnt);
		}
#endif
		mm->map_count--;
		//remove_shared_vm_struct(mpnt);
		zap_page_range(mm, start, size);
#if 0
		if (mpnt->vm_file)
			fput(mpnt->vm_file);
#endif
		//kmem_cache_free(vm_area_cachep, mpnt);// not yet
		mpnt = next;
	}
	flush_tlb_mm(mm);

	/* This is just debugging */
#if 0
	if (mm->map_count)
		BUG();
#endif

	//printk("cpt %x %x %x %x\n",mm, FIRST_USER_PGD_NR, USER_PTRS_PER_PGD -1);
#ifdef __i386__
	clear_page_tables(mm, FIRST_USER_PGD_NR, USER_PTRS_PER_PGD -1 ); // don't clobber P1
#else
	//clear_page_tables(mm, FIRST_USER_PGD_NR, 1 ); // don't clobber P1
	//clear_page_tables(mm, FIRST_USER_PGD_NR, USER_PTRS_PER_PGD -1 ); // don't clobber P1
#if 0
	void free_pgd_range(struct mmu_gather **tlb, unsigned long addr, unsigned long end, unsigned long floor, unsigned long ceiling);
	#include <asm/tlb.h>
	struct mmu_gather *tlb = tlb_gather_mmu(mm);
	free_pgd_range(&tlb,  FIRST_USER_ADDRESS, 0x7f000000, FIRST_USER_ADDRESS, 0);
#endif
#endif
}

/* Insert vm structure into process list sorted by address
 * and into the inode's i_mmap ring.  If vm_file is non-NULL
 * then the i_shared_lock must be held here.
 */
#if 0
void __insert_vm_struct(struct mm_struct * mm, struct vm_area_struct * vma)
{
	struct _rde * __vma, * prev;
	rb_node_t ** rb_link, * rb_parent;

	__vma = find_vma_prepare(mm, vma->rde$pq_start_va, &prev, &rb_link, &rb_parent);
	if (__vma && __vma->rde$pq_start_va < (vma->rde$pq_start_va + vma->rde$q_region_size))
		BUG();
	//__vma_link(mm, vma, prev, rb_link, rb_parent);
	mm->map_count++;
	validate_mm(mm);
}

void insert_vm_struct(struct mm_struct * mm, struct vm_area_struct * vma)
{
	struct _rde * __vma, * prev;
	rb_node_t ** rb_link, * rb_parent;

	__vma = find_vma_prepare(mm, vma->rde$pq_start_va, &prev, &rb_link, &rb_parent);
	if (__vma && __vma->rde$pq_start_va < (vma->rde$pq_start_va + vma->rde$q_region_size))
		BUG();
	//vma_link(mm, vma, prev, rb_link, rb_parent);
	validate_mm(mm);
}
#endif
