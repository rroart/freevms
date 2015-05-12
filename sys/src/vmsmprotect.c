// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004. Based on mprotect.c.

/*
 *  linux/mm/mprotect.c
 *
 *  (C) Copyright 1994 Linus Torvalds
 */
#include <linux/config.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/mman.h>

#include <asm/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>

#include <ipldef.h>
#include <phddef.h>
#include <rdedef.h>
#include <secdef.h>
#include <misc_routines.h>

static inline void change_pte_range(pmd_t * pmd, unsigned long address,
                                    unsigned long size, pgprot_t newprot)
{
    pte_t * pte;
    unsigned long end;

    if (pmd_none(*pmd))
        return;
    if (pmd_bad(*pmd))
    {
        pmd_ERROR(*pmd);
        pmd_clear(pmd);
        return;
    }
    pte = pte_offset(pmd, address);
    address &= ~PMD_MASK;
    end = address + size;
    if (end > PMD_SIZE)
        end = PMD_SIZE;
    do
    {
        if (pte_present(*pte))
        {
            pte_t entry;

            /* Avoid an SMP race with hardware updated dirty/clean
             * bits by wiping the pte and then setting the new pte
             * into place.
             */
            entry = ptep_get_and_clear(42, 42, pte); // clear
            set_pte(pte, pte_modify(entry, newprot));
        }
        address += PAGE_SIZE;
        pte++;
    }
    while (address && (address < end));
}

static inline void change_pmd_range(pud_t * pud, unsigned long address,
                                    unsigned long size, pgprot_t newprot)
{
    pmd_t * pmd;
    unsigned long end;

    if (pud_none(*pud))
        return;
    if (pud_bad(*pud))
    {
        pud_ERROR(*pud);
        pud_clear(pud);
        return;
    }
    pmd = pmd_offset(pud, address);
    address &= ~PGDIR_MASK;
    end = address + size;
    if (end > PGDIR_SIZE)
        end = PGDIR_SIZE;
    do
    {
        change_pte_range(pmd, address, end - address, newprot);
        address = (address + PMD_SIZE) & PMD_MASK;
        pmd++;
    }
    while (address && (address < end));
}

static inline void change_pud_range(pgd_t * pgd, unsigned long address,
                                    unsigned long size, pgprot_t newprot)
{
    pud_t * pud;
    unsigned long end;

    if (pgd_none(*pgd))
        return;
    if (pgd_bad(*pgd))
    {
        pgd_ERROR(*pgd);
        pgd_clear(pgd);
        return;
    }
    pud = pud_offset(pgd, address);
    address &= ~PGDIR_MASK;
    end = address + size;
    if (end > PGDIR_SIZE)
        end = PGDIR_SIZE;
    do
    {
        change_pmd_range(pud, address, end - address, newprot);
        address = (address + PUD_SIZE) & PUD_MASK;
        pud++;
    }
    while (address && (address < end));
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
    do
    {
        change_pud_range(dir, start, end - start, newprot);
        start = (start + PGDIR_SIZE) & PGDIR_MASK;
        dir++;
    }
    while (start && (start < end));
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
            /*!vma->vm_file && */ !(vma->rde$l_flags & VM_SHARED))
    {
        spin_lock(&mm->page_table_lock);
        prev->rde$q_region_size = vma->rde$q_region_size;
#if 0
        __vma_unlink(mm, vma, prev);
#endif
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

#if 0
    panic("fixup_start not implemented yet\n");
#endif

    *pprev = vma;

    if (prev && (prev->rde$pq_start_va + prev->rde$q_region_size) == vma->rde$pq_start_va && can_vma_merge(prev, newflags) &&
            /* !vma->vm_file && */ !(vma->rde$l_flags & VM_SHARED))
    {
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
    vma->rde$q_region_size -= n->rde$q_region_size;
    //__insert_vm_struct(current->mm, n);
    insrde(n,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
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
    n->rde$q_region_size -= vma->rde$q_region_size;
    //__insert_vm_struct(current->mm, n);
    insrde(n,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
    //spin_unlock(&vma->vm_mm->page_table_lock);

    {
        struct _secdef * sec, * pstl;
        pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
        unsigned long page=start;
        unsigned long secno;
        unsigned long count;
        pgd = pgd_offset(current->mm, page);
        pud = pud_offset(pgd, page);
        pmd = pmd_offset(pud, page);
        pte = pte_offset(pmd, page);
        secno = ((*(unsigned long*)pte)&0xfffff000)>>PAGE_SHIFT;
        pstl=current->pcb$l_phd->phd$l_pst_base_offset;
        sec=&pstl[current->pcb$l_phd->phd$l_pst_free++];
        *sec=pstl[secno];
        sec->sec$l_vbn+=(vma->rde$q_region_size>>PAGE_SHIFT);
        for (count=start; count<(start+n->rde$q_region_size); count+=PAGE_SIZE)
        {
            pgd_t *pgd;
            pud_t *pud;
            pmd_t *pmd;
            pte_t *pte;
            pgd = pgd_offset(current->mm, count);
            pud = pud_offset(pgd, count);
            pmd = pmd_offset(pud, count);
            pte = pte_offset(pmd,count);
            if (((*(unsigned long*)pte)&0xfff)==0xc00)
                *(unsigned long*)pte = ((*(unsigned long*)pte)&0xfff)|((current->pcb$l_phd->phd$l_pst_free-1)<<PAGE_SHIFT);
        }
    }

    unlock_vma_mappings(vma);

    *pprev = n;

    return 0;
}

static inline int mprotect_fixup_middle(struct _rde * vma, struct _rde ** pprev,
                                        unsigned long start, unsigned long end,
                                        int newflags, pgprot_t prot)
{
    struct _rde * left, * right;

    panic("fixup_middle not implemented yet\n");

    left = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
    if (!left)
        return -ENOMEM;
    right = kmem_cache_alloc(vm_area_cachep, SLAB_KERNEL);
    if (!right)
    {
        kmem_cache_free(vm_area_cachep, left);
        return -ENOMEM;
    }
    *left = *vma;
    *right = *vma;
    left->rde$q_region_size = start - (unsigned long)left->rde$pq_start_va;
    right->rde$pq_start_va = end;
    right->rde$q_region_size = end - (unsigned long)vma->rde$pq_start_va;
#if 0
    right->vm_pgoff += (right->rde$pq_start_va - left->rde$pq_start_va) >> PAGE_SHIFT;
    left->vm_raend = 0;
    right->vm_raend = 0;
    if (vma->vm_file)
        atomic_add(2,&vma->vm_file->f_count);
    if (vma->vm_ops && vma->vm_ops->open)
    {
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
    insrde(left,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
    //__insert_vm_struct(current->mm, right);
    insrde(right,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
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

    if ((newflags >> 8) == (vma->rde$l_flags >> 8))
    {
        *pprev = vma;
        return 0;
    }
    newprot = protection_map[(newflags>>8) & 0xf];
    if (start == vma->rde$pq_start_va)
    {
        if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
            error = mprotect_fixup_all(vma, pprev, newflags, newprot);
        else
            error = mprotect_fixup_start(vma, pprev, end, newflags, newprot);
    }
    else if (end == (vma->rde$pq_start_va + vma->rde$q_region_size))
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

    for (nstart = start ; ; )
    {
        unsigned int newflags;
        int last = 0;

        /* Here we know that  vma->rde$pq_start_va <= nstart < (vma->rde$pq_start_va + vma->rde$q_region_size). */

        newflags = (prot<<8) | (vma->rde$l_flags & ~((PROT_READ | PROT_WRITE | PROT_EXEC)<<8));
        if ((newflags & ~(newflags >> 4)) & 0xf)
        {
            error = -EACCES;
            goto out;
        }

        if ((vma->rde$pq_start_va + vma->rde$q_region_size) > end)
        {
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
        if (!vma || vma->rde$pq_start_va != nstart)
        {
            error = -EFAULT;
            goto out;
        }
    }
    if (next && (prev->rde$pq_start_va + prev->rde$q_region_size) == next->rde$pq_start_va && can_vma_merge(next, prev->rde$l_flags) &&
            /*!prev->vm_file && */ !(prev->rde$l_flags & VM_SHARED))
    {
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
