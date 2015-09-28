#ifndef LDR_ROUTINES_H
#define LDR_ROUTINES_H

#include <far_pointers.h>
#include <ptedef.h>

int   ldr_std$alloc_pt (int npte, void **svapte_p);
int   ldr_std$dealloc_pt (void *svapte, int npte);
int   ldr_std$alloc_s0s1_va (const int req_pages, VOID_PPQ sva_p, PTE_PPQ va_pte_p);
int   ldr_std$dealloc_s0s1_va (const int page_count, const VOID_PQ sva);
int   ldr_std$alloc_s2_va (const int req_pages, VOID_PPQ sva_p, PTE_PPQ va_pte_p);
int   ldr_std$dealloc_s2_va (const int page_count, const VOID_PQ sva);
int   ldr_std$count_s2_pages (void);
int   ldr_std$alloc_s2_va_aligned (const int req_pages, VOID_PPQ sva);

#endif
