/*
 *	linux/mm/mlock.c
 *
 *  (C) Copyright 1995 Linus Torvalds
 */
#include <linux/slab.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/smp_lock.h>
#include <linux/pagemap.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>

#include <ipldef.h>
#include <rdedef.h>

static inline int mlock_fixup_all(struct _rde * vma, int newflags)
{
  //spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$l_flags = newflags;
	//spin_unlock(&vma->vm_mm->page_table_lock);
	return 0;
}

static inline int mlock_fixup_start(struct _rde * vma,
	unsigned long end, int newflags)
{
	struct _rde * n;

	n = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!n)
		return -EAGAIN;
	*n = *vma;
	n->rde$q_region_size = end - (unsigned long)n->rde$pq_start_va;
	n->rde$l_flags = newflags;
	//n->vm_raend = 0;
#if 0
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
	vma->vm_pgoff += (end - vma->rde$pq_start_va) >> PAGE_SHIFT;
#endif
	lock_vma_mappings(vma);
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$pq_start_va = end;
	//__insert_vm_struct(current->mm, n);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

static inline int mlock_fixup_end(struct _rde * vma,
	unsigned long start, int newflags)
{
	struct _rde * n;

	n = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!n)
		return -EAGAIN;
	*n = *vma;
	n->rde$pq_start_va = start;
	//n->vm_pgoff += (n->rde$pq_start_va - vma->rde$pq_start_va) >> PAGE_SHIFT;
	n->rde$l_flags = newflags;
#if 0
	n->vm_raend = 0;
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
	lock_vma_mappings(vma);
#endif
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$q_region_size = start - (unsigned long)vma->rde$pq_start_va;
	//__insert_vm_struct(current->mm, n);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

static inline int mlock_fixup_middle(struct _rde * vma,
	unsigned long start, unsigned long end, int newflags)
{
	struct _rde * left, * right;

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
#if 0
	right->vm_pgoff += (right->rde$pq_start_va - left->rde$pq_start_va) >> PAGE_SHIFT;
#endif
	vma->rde$l_flags = newflags;
#if 0
	left->vm_raend = 0;
	right->vm_raend = 0;
	if (vma->vm_file)
		atomic_add(2, &vma->vm_file->f_count);

	if (vma->vm_ops && vma->vm_ops->open) {
		vma->vm_ops->open(left);
		vma->vm_ops->open(right);
	}
	vma->vm_raend = 0;
	vma->vm_pgoff += (start - vma->rde$pq_start_va) >> PAGE_SHIFT;
#endif
	lock_vma_mappings(vma);
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$pq_start_va = start;
	vma->rde$q_region_size = end - (unsigned long)vma->rde$pq_start_va;
	vma->rde$l_flags = newflags;
	//__insert_vm_struct(current->mm, left);
	//__insert_vm_struct(current->mm, right);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);
	return 0;
}

static int mlock_fixup(struct _rde * vma, 
	unsigned long start, unsigned long end, unsigned int newflags)
{
	int pages, retval;

	if (newflags == vma->rde$l_flags)
		return 0;

	if (start == vma->rde$pq_start_va) {
		if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
			retval = mlock_fixup_all(vma, newflags);
		else
			retval = mlock_fixup_start(vma, end, newflags);
	} else {
		if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
			retval = mlock_fixup_end(vma, start, newflags);
		else
			retval = mlock_fixup_middle(vma, start, end, newflags);
	}
	if (!retval) {
		/* keep track of amount of locked VM */
		pages = (end - start) >> PAGE_SHIFT;
		if (newflags & VM_LOCKED) {
			pages = -pages;
			make_pages_present(start, end);
		}
		//vma->vm_mm->locked_vm -= pages;
	}
	return retval;
}

static int do_mlock(unsigned long start, size_t len, int on)
{
	unsigned long nstart, end, tmp;
	struct _rde * vma, * next;
	int error;

	if (on && !capable(CAP_IPC_LOCK))
		return -EPERM;
	len = PAGE_ALIGN(len);
	end = start + len;
	if (end < start)
		return -EINVAL;
	if (end == start)
		return 0;
	//vma = find_vma(current->mm, start);
	vma = mmg$lookup_rde_va(start,current->pcb$l_phd,LOOKUP_RDE_EXACT,IPL$_ASTDEL);
	if (!vma || vma->rde$pq_start_va > start)
		return -ENOMEM;

	for (nstart = start ; ; ) {
		unsigned int newflags;

		/* Here we know that  vma->rde$pq_start_va <= nstart < (vma->rde$pq_start_va + vma->rde$q_region_size). */

		newflags = vma->rde$l_flags | VM_LOCKED;
		if (!on)
			newflags &= ~VM_LOCKED;

		if ((vma->rde$pq_start_va + vma->rde$q_region_size) >= end) {
			error = mlock_fixup(vma, nstart, end, newflags);
			break;
		}

		tmp = (vma->rde$pq_start_va + vma->rde$q_region_size);
		next = vma->rde$ps_va_list_flink;
		error = mlock_fixup(vma, nstart, tmp, newflags);
		if (error)
			break;
		nstart = tmp;
		vma = next;
		if (!vma || vma->rde$pq_start_va != nstart) {
			error = -ENOMEM;
			break;
		}
	}
	return error;
}

asmlinkage long sys_mlock(unsigned long start, size_t len)
{
	unsigned long locked;
	unsigned long lock_limit;
	int error = -ENOMEM;

	down_write(&current->mm->mmap_sem);
	len = PAGE_ALIGN(len + (start & ~PAGE_MASK));
	start &= PAGE_MASK;

	locked = len >> PAGE_SHIFT;
	locked += current->mm->locked_vm;

	lock_limit = current->rlim[RLIMIT_MEMLOCK].rlim_cur;
	lock_limit >>= PAGE_SHIFT;

	/* check against resource limits */
	if (locked > lock_limit)
		goto out;

	/* we may lock at most half of physical memory... */
	/* (this check is pretty bogus, but doesn't hurt) */
	if (locked > num_physpages/2)
		goto out;

	error = do_mlock(start, len, 1);
out:
	up_write(&current->mm->mmap_sem);
	return error;
}

asmlinkage long sys_munlock(unsigned long start, size_t len)
{
	int ret;

	down_write(&current->mm->mmap_sem);
	len = PAGE_ALIGN(len + (start & ~PAGE_MASK));
	start &= PAGE_MASK;
	ret = do_mlock(start, len, 0);
	up_write(&current->mm->mmap_sem);
	return ret;
}

static int do_mlockall(int flags)
{
	int error;
	unsigned int def_flags;
	struct _rde * vma;

	if (!capable(CAP_IPC_LOCK))
		return -EPERM;

	def_flags = 0;
	if (flags & MCL_FUTURE)
		def_flags = VM_LOCKED;
	current->mm->def_flags = def_flags;

	error = 0;
	for (vma = current->mm->mmap; vma ; vma = vma->rde$ps_va_list_flink) {
		unsigned int newflags;

		newflags = vma->rde$l_flags | VM_LOCKED;
		if (!(flags & MCL_CURRENT))
			newflags &= ~VM_LOCKED;
		error = mlock_fixup(vma, vma->rde$pq_start_va, (vma->rde$pq_start_va + vma->rde$q_region_size), newflags);
		if (error)
			break;
	}
	return error;
}

asmlinkage long sys_mlockall(int flags)
{
	unsigned long lock_limit;
	int ret = -EINVAL;

	down_write(&current->mm->mmap_sem);
	if (!flags || (flags & ~(MCL_CURRENT | MCL_FUTURE)))
		goto out;

	lock_limit = current->rlim[RLIMIT_MEMLOCK].rlim_cur;
	lock_limit >>= PAGE_SHIFT;

	ret = -ENOMEM;
	if (current->mm->total_vm > lock_limit)
		goto out;

	/* we may lock at most half of physical memory... */
	/* (this check is pretty bogus, but doesn't hurt) */
	if (current->mm->total_vm > num_physpages/2)
		goto out;

	ret = do_mlockall(flags);
out:
	up_write(&current->mm->mmap_sem);
	return ret;
}

asmlinkage long sys_munlockall(void)
{
	int ret;

	down_write(&current->mm->mmap_sem);
	ret = do_mlockall(0);
	up_write(&current->mm->mmap_sem);
	return ret;
}
