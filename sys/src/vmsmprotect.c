/*
 *	linux/mm/mprotect.c
 *
 *  (C) Copyright 1994 Linus Torvalds
 */
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/shm.h>
#include <linux/mman.h>

#include <asm/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>

#include <ipldef.h>
#include <phddef.h>
#include <rdedef.h>

static inline void change_pte_range(pmd_t * pmd, unsigned long address,
	unsigned long size, pgprot_t newprot)
{
	pte_t * pte;
	unsigned long end;

	if (pmd_none(*pmd))
		return;
	if (pmd_bad(*pmd)) {
		pmd_ERROR(*pmd);
		pmd_clear(pmd);
		return;
	}
	pte = pte_offset(pmd, address);
	address &= ~PMD_MASK;
	end = address + size;
	if (end > PMD_SIZE)
		end = PMD_SIZE;
	do {
		if (pte_present(*pte)) {
			pte_t entry;

			/* Avoid an SMP race with hardware updated dirty/clean
			 * bits by wiping the pte and then setting the new pte
			 * into place.
			 */
			entry = ptep_get_and_clear(pte);
			set_pte(pte, pte_modify(entry, newprot));
		}
		address += PAGE_SIZE;
		pte++;
	} while (address && (address < end));
}

static inline void change_pmd_range(pgd_t * pgd, unsigned long address,
	unsigned long size, pgprot_t newprot)
{
	pmd_t * pmd;
	unsigned long end;

	if (pgd_none(*pgd))
		return;
	if (pgd_bad(*pgd)) {
		pgd_ERROR(*pgd);
		pgd_clear(pgd);
		return;
	}
	pmd = pmd_offset(pgd, address);
	address &= ~PGDIR_MASK;
	end = address + size;
	if (end > PGDIR_SIZE)
		end = PGDIR_SIZE;
	do {
		change_pte_range(pmd, address, end - address, newprot);
		address = (address + PMD_SIZE) & PMD_MASK;
		pmd++;
	} while (address && (address < end));
}

static void change_protection(unsigned long start, unsigned long end, pgprot_t newprot)
{
	pgd_t *dir;
	unsigned long beg = start;

	dir = pgd_offset(current->mm, start);
	flush_cache_range(current->mm, beg, end);
	if (start >= end)
		BUG();
	spin_lock(&current->mm->page_table_lock);
	do {
		change_pmd_range(dir, start, end - start, newprot);
		start = (start + PGDIR_SIZE) & PGDIR_MASK;
		dir++;
	} while (start && (start < end));
	spin_unlock(&current->mm->page_table_lock);
	flush_tlb_range(current->mm, beg, end);
	return;
}

static inline int mprotect_fixup_all(struct _rde * vma, struct _rde ** pprev,
	int newflags, pgprot_t prot)
{
	struct _rde * prev = *pprev;
	struct mm_struct * mm = current->mm;//vma->vm_mm;

	if (prev && (prev->rde$pq_start_va + prev->rde$q_region_size) == vma->rde$pq_start_va && can_vma_merge(prev, newflags) &&
	    /*!vma->vm_file && */ !(vma->rde$l_flags & VM_SHARED)) {
		spin_lock(&mm->page_table_lock);
		prev->rde$q_region_size = vma->rde$q_region_size;
		__vma_unlink(mm, vma, prev);
		spin_unlock(&mm->page_table_lock);

		kmem_cache_free(vm_area_cachep, vma);
		mm->map_count--;

		return 0;
	}

	spin_lock(&mm->page_table_lock);
	vma->rde$l_flags = newflags;
	vma->rde$r_regprot.regprt$l_region_prot = *(unsigned long *)&prot;
	spin_unlock(&mm->page_table_lock);

	*pprev = vma;

	return 0;
}

static inline int mprotect_fixup_start(struct _rde * vma, struct _rde ** pprev,
	unsigned long end,
	int newflags, pgprot_t prot)
{
	struct _rde * n, * prev = *pprev;

	*pprev = vma;

	if (prev && (prev->rde$pq_start_va + prev->rde$q_region_size) == vma->rde$pq_start_va && can_vma_merge(prev, newflags) &&
	    /* !vma->vm_file && */ !(vma->rde$l_flags & VM_SHARED)) {
	  //spin_lock(&vma->vm_mm->page_table_lock);
		prev->rde$q_region_size = end - (unsigned long)prev->rde$pq_start_va;
		vma->rde$pq_start_va = end;
		//spin_unlock(&vma->vm_mm->page_table_lock);

		return 0;
	}
	n = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!n)
		return -ENOMEM;
	*n = *vma;
	n->rde$q_region_size = end - (unsigned long)n->rde$pq_start_va;
	n->rde$l_flags = newflags;
	//n->vm_raend = 0;
	n->rde$r_regprot.regprt$l_region_prot = *(unsigned long *)&prot;
#if 0
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
	vma->vm_pgoff += (end - (unsigned long)vma->rde$pq_start_va) >> PAGE_SHIFT;
#endif
	lock_vma_mappings(vma);
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$pq_start_va = end;
	//__insert_vm_struct(current->mm, n);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);

	return 0;
}

static inline int mprotect_fixup_end(struct _rde * vma, struct _rde ** pprev,
	unsigned long start,
	int newflags, pgprot_t prot)
{
	struct _rde * n;

	n = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);
	if (!n)
		return -ENOMEM;
	*n = *vma;
	n->rde$pq_start_va = start;
	//n->vm_pgoff += (n->rde$pq_start_va - vma->rde$pq_start_va) >> PAGE_SHIFT;
	n->rde$l_flags = newflags;
	//n->vm_raend = 0;
	n->rde$r_regprot.regprt$l_region_prot = *(unsigned long *)&prot;
#if 0
	if (n->vm_file)
		get_file(n->vm_file);
	if (n->vm_ops && n->vm_ops->open)
		n->vm_ops->open(n);
#endif
	lock_vma_mappings(vma);
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$q_region_size = start - (unsigned long)vma->rde$pq_start_va;
	//__insert_vm_struct(current->mm, n);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);

	*pprev = n;

	return 0;
}

static inline int mprotect_fixup_middle(struct _rde * vma, struct _rde ** pprev,
	unsigned long start, unsigned long end,
	int newflags, pgprot_t prot)
{
	struct _rde * left, * right;

	left = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!left)
		return -ENOMEM;
	right = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
	if (!right) {
		kmem_cache_free(vm_area_cachep, left);
		return -ENOMEM;
	}
	*left = *vma;
	*right = *vma;
	left->rde$q_region_size = start - (unsigned long)left->rde$pq_start_va;
	right->rde$pq_start_va = end;
#if 0
	right->vm_pgoff += (right->rde$pq_start_va - left->rde$pq_start_va) >> PAGE_SHIFT;
	left->vm_raend = 0;
	right->vm_raend = 0;
	if (vma->vm_file)
		atomic_add(2,&vma->vm_file->f_count);
	if (vma->vm_ops && vma->vm_ops->open) {
		vma->vm_ops->open(left);
		vma->vm_ops->open(right);
	}
	vma->vm_pgoff += (start - vma->rde$pq_start_va) >> PAGE_SHIFT;
	vma->vm_raend = 0;
#endif
	vma->rde$r_regprot.regprt$l_region_prot = *(unsigned long *)&prot;
	lock_vma_mappings(vma);
	//spin_lock(&vma->vm_mm->page_table_lock);
	vma->rde$pq_start_va = start;
	vma->rde$q_region_size = end - start;
	vma->rde$l_flags = newflags;
	//__insert_vm_struct(current->mm, left);
	//__insert_vm_struct(current->mm, right);
	//spin_unlock(&vma->vm_mm->page_table_lock);
	unlock_vma_mappings(vma);

	*pprev = right;

	return 0;
}

static int mprotect_fixup(struct _rde * vma, struct _rde ** pprev,
	unsigned long start, unsigned long end, unsigned int newflags)
{
	pgprot_t newprot;
	int error;

	if (newflags == vma->rde$l_flags) {
		*pprev = vma;
		return 0;
	}
	newprot = protection_map[newflags & 0xf];
	if (start == vma->rde$pq_start_va) {
		if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
			error = mprotect_fixup_all(vma, pprev, newflags, newprot);
		else
			error = mprotect_fixup_start(vma, pprev, end, newflags, newprot);
	} else if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
		error = mprotect_fixup_end(vma, pprev, start, newflags, newprot);
	else
		error = mprotect_fixup_middle(vma, pprev, start, end, newflags, newprot);

	if (error)
		return error;

	change_protection(start, end, newprot);
	return 0;
}

asmlinkage long sys_mprotect(unsigned long start, size_t len, unsigned long prot)
{
	unsigned long nstart, end, tmp;
	struct _rde * vma, * next, * prev;
	int error = -EINVAL;

	if (start & ~PAGE_MASK)
		return -EINVAL;
	len = PAGE_ALIGN(len);
	end = start + len;
	if (end < start)
		return -EINVAL;
	if (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC))
		return -EINVAL;
	if (end == start)
		return 0;

	down_write(&current->mm->mmap_sem);

	//vma = find_vma_prev(current->mm, start, &prev);
	//vma = mmg$search_rde_va(start, &current->pcb$l_phd->phd$ps_p0_va_list_flink, &prev, &next);
	vma = find_vma_prev(current->pcb$l_phd,start,&prev);
	//prev = vma->rde$ps_va_list_blink;
	error = -EFAULT;
	if (!vma || vma->rde$pq_start_va > start)
		goto out;

	for (nstart = start ; ; ) {
		unsigned int newflags;
		int last = 0;

		/* Here we know that  vma->rde$pq_start_va <= nstart < (vma->rde$pq_start_va + vma->rde$q_region_size). */

		newflags = prot | (vma->rde$l_flags & ~(PROT_READ | PROT_WRITE | PROT_EXEC));
		if ((newflags & ~(newflags >> 4)) & 0xf) {
			error = -EACCES;
			goto out;
		}

		if ((vma->rde$pq_start_va + vma->rde$q_region_size) > end) {
			error = mprotect_fixup(vma, &prev, nstart, end, newflags);
			goto out;
		}
		if ((vma->rde$pq_start_va + vma->rde$q_region_size) == end)
			last = 1;

		tmp = (vma->rde$pq_start_va + vma->rde$q_region_size);
		next = vma->rde$ps_va_list_flink;
		error = mprotect_fixup(vma, &prev, nstart, tmp, newflags);
		if (error)
			goto out;
		if (last)
			break;
		nstart = tmp;
		vma = next;
		if (!vma || vma->rde$pq_start_va != nstart) {
			error = -EFAULT;
			goto out;
		}
	}
	if (next && (prev->rde$pq_start_va + prev->rde$q_region_size) == next->rde$pq_start_va && can_vma_merge(next, prev->rde$l_flags) &&
	    /*!prev->vm_file && */ !(prev->rde$l_flags & VM_SHARED)) {
	  //spin_lock(&prev->vm_mm->page_table_lock);
		prev->rde$q_region_size = next->rde$q_region_size;
		//__vma_unlink(prev->vm_mm, next, prev);
		//spin_unlock(&prev->vm_mm->page_table_lock);

		kmem_cache_free(vm_area_cachep, next);
		//prev->vm_mm->map_count--;
	}
out:
	up_write(&current->mm->mmap_sem);
	return error;
}
