#ifndef mmg_routines_h
#define mmg_routines_h

typedef unsigned long long uint64;

#include <acbdef.h>
#if 0
// not yet
#include <boddef.h>
#endif
#include <ccbdef.h>
#include <far_pointers.h>
#include <gsddef.h>
#if 0
#include <hwrpbdef.h>
#include <ints.h>
#endif
#include <pcbdef.h>
#include <pfndef.h>
#include <phddef.h>
#include <ptedef.h>
#include <rdedef.h>
#if 0
#include <rmddef.h>
#endif
#include <secdef.h>
#include <wsldef.h>

void  mmg_std$add_pt_win_cnt (VOID_PQ va, PTE_PQ va_pte, int win_count); 
int   mmg_std$alloc_pfn_64 (PFN_PPQ pfndbe_p);
int       mmg$alloc_pfn_algnd_64 (uint64 vpn, PFN_PPQ pfndbe_p);
int   mmg_std$alloc_zero_pfn_64 (PFN_PPQ pfndbe_p);
int       mmg$alloc_zero_algnd_64 (uint64 vpn, PFN_PPQ pfndbe_p);
int   mmg_std$allo_contig (uint32 pfn_count);
int   mmg_std$allo_contig_pfn (uint32 pfn_count, uint32 max_pfn);

int   mmg_std$allo_contig_a (uint32 pfn_count, uint32 byte_align);
int   mmg_std$allo_contig_pfn_a (uint32 pfn_count, uint32 byte_align, uint32 max_pfn);
#ifndef	MMG_STD$ALLOC_CTG_PFN_MAP_SVA_SRC
int   mmg_std$alloc_ctg_pfn_map_sva (
      const int proto_pte,
      const int page_count,
      const uint32 refcnt,
      VOID_PQ sva,
      const uint32 max_pfn,
      ...

    );
#endif
void  mmg_std$dalloc_contig_pfn (uint32 pfn, uint32 pfn_count);
void  mmg_std$dalloc_pfn_64 (int pfn, PFN_PQ pfndbe);
void  mmg_std$dalloc_zero_pfn_64 (int pfn, PFN_PQ pfndbe);
void  mmg_std$del_contents_pfn_64 (int pfn, PFN_PQ pfndbe);
int   mmg_std$alloc_gpt (const int req_pages, PTE_PPQ gpte_p);
int   mmg_std$dealloc_gpt (const int page_count, const PTE_PQ gpte);
void  mmg_std$decptref_pfndb (PFN_PQ const pfndbe);
void  mmg_std$deref_bufobj (int32 object_handle_1, int32 object_handle_2);
int   mmg_std$iolock_buf (VOID_PQ const buf, const int bufsiz, 
                          const int is_read, struct _pcb *const pcb,
                          PTE_PPQ va_pte_p, VOID_PPQ fault_va_p);
void  mmg_std$iounlock_buf (const int npages, PTE_PQ const va_pte);
void  mmg_std$lockpgtb_64 (PTE_PQ const va_pte, struct _phd *const phd);
void  mmg_std$svaptechk (VOID_PQ va, struct _pcb *pcb, struct _phd *phd, struct _pte **svapte_p);
void      mmg$tbi_all (void);
int   mmg_std$ref_bufobj (int32 object_handle_1, int32 object_handle_2, ...);
void      mmg$tbi_single (VOID_PQ va);
void      mmg$tbi_single_threads (VOID_PQ va);
void  mmg_std$tbi_data_64 (VOID_PQ va);
void  mmg_std$tbi_data_64_threads (VOID_PQ va);

int   mmg_std$delpag_64 (struct _rde *rde, VOID_PPQ start_va_p, uint64 *pages_1, 
	  int acmode, int *mmg_flags);
void  mmg_std$delete_pts (VOID_PQ start_va, VOID_PQ end_va);
void  mmg_std$delete_private_l3pt (PTE_PQ l3pt_va, struct _pcb *pcb, struct _phd *phd);
void  mmg_std$check_contract_64_1 (struct _rde *rde, struct _phd *phd, VOID_PQ start_va);
void  mmg_std$check_contract_64 (struct _rde *rde, struct _phd *phd, VOID_PQ start_va, 
	  VOID_PQ end_va);
int   mmg_std$try_all_64(struct _rde *rde, VOID_PQ start_va, VOID_PQ end_va,
	  uint64 expbytes, uint64 pages, int *pagefile_cache_p, 
	  int *mmg_flags_p);
void  mmg_std$fast_create_64(struct _rde *rde, VOID_PQ start_va, VOID_PQ end_va,
	  uint64 pages, struct _pte prot_pte);
int   mmg_std$crepag_64(struct _rde *rde, VOID_PPQ va_p, uint64 *pages_1, int max_mode,
	  struct _pte proto_pte, int *pagefile_cache_p, int *mmg_flags_p);
int   mmg_std$create_shpt_mapping_64 (struct _rde *rde, VOID_PPQ va_p, uint64 *pages_1, int max_mode,
	  struct _pte proto_pte, int *mmg_flags_p);
int   mmg_std$check_window_64(int channel, struct _ccb **ccb_p, int *efblk_p);

#ifdef __NEW_STARLET
int   mmg_std$init_pste_64(struct _ccb *ccb, uint64 *pagelets_p, int section_flags,
	  uint64 start_vbn, int pfc_pagelets, int *efblk_p, struct _secdef **pste_p, 
	  int *pstx_p);
int   mmg_std$gfile_gste_64(struct _gsd *gsd, struct _ccb *ccb, uint64 *pagelets_p, 
	  int section_flags, uint64 start_vbn, int pfc_pagelets, int *efblk_p, 
	  struct _secdef **gste_p, unsigned int *gstx_p);
int   mmg_std$gdzro_gste_64(struct _gsd *gsd, uint64 *pagelets_p, int section_flags, 
	  struct _secdef **gste_p, unsigned int *gstx_p);
int   mmg_std$setsecprotown_64(struct _pte *proto_pte_p, int section_flags, int acmode,
	  struct _secdef *section_table_entry_p, struct _gsd *gsd);			  
int   mmg_std$chkpro_audit(struct _gsd *gsd, struct _secdef *gste, int section_flags, 
	  int status, int callers_mode);
int  mmg_std$global_fast_map_64(VOID_PQ start_va, VOID_PQ end_va, uint64 pages,
	  struct _pte proto_pte, struct _secdef *gste);
#endif

int   mmg_std$sec_fast_create_64 (struct _rde *rde, VOID_PQ start_va, VOID_PQ end_va, 
	  uint64 pages, struct _pte proto_pte, int mmg_flags);	  
void  mmg_std$set_gh_and_fastmap_64 (VOID_PQ start_va, VOID_PQ end_va, 
	  uint64 pages, struct _pte proto_pte);
int   mmg_std$init_orb(struct _gsd *gsd, struct _ccb *ccb, int prot);
void  mmg_std$dalcstxscn(struct _phd *phd);
void  mmg_std$subsecrefl(int refcnt, int index, struct _phd *phd);
int   mmg_std$derive_granhint_64(struct _rde *rde, VOID_PPQ start_va_p, 
	  VOID_PPQ end_va_p, uint64 page_count, struct _pte proto_pte,
	  int *mmg_flags_p);
void  mmg_std$fastmap_with_gh_64(VOID_PQ start_va, VOID_PQ end_va,
	  uint64 page_count, struct _pte proto_pte);
void  mmg_std$set_gh_and_fastmap_64(VOID_PQ start_va, VOID_PQ end_va,
	  uint64 page_count, struct _pte proto_pte);
void  mmg_std$perform_audit(struct _gsd *gsd, int status);
void  mmg_std$delgblwcb(struct _pcb *pcb);
void  mmg_std$movgsdnam(char *dest_p, char *source_p, struct _gsd *gsd); 
int   mmg_std$sec_privchk(char *gs_string_p, int flags);
int   mmg_std$lckulkpag(VOID_PQ start_va, int acmode, int flags, struct _pcb *pcb,
			struct _phd *phd, struct _wsl * wsl); // was: WSL_PQ
void  mmg_std$purgwspag_64(VOID_PQ start_va, int acmode, uint64 pages_1,
	  int page_inc);
int   mmg_std$lckbufobjpag(VOID_PQ start_va, int acmode, int page_inc, 
	  struct _bod *bod, PTE_PPQ va_pte_p);
void  mmg_std$delbufobjpages(struct _bod *bod, struct _pcb *pcb);
int   mmg_std$setprtpag_64(struct _rde *rde, VOID_PQ start_va, int acmode, 
	  int alpha_prot, int *prev_prot_p);
int   mmg_std$setfltpag_64(struct _rde *rde, VOID_PQ start_va, int acmode,
	  int fault_mask);
int   mmg_std$has_priv_pswapm(struct _pcb *pcb);
void  mmg_std$addprcpgfl(int pages, struct _phd *phd);         	
void  mmg_std$lock_gpt_64(PTE_PQ gpte, struct _phd *sys_phd,...);
int   mmg_std$gsdscan(VOID_PQ input_gsnam, 
	  UINT64_PQ input_ident, int section_flags, char *return_gsnam, 
	  uint64 *return_ident, struct _gsd **gsd);
int mmg$alloc_sva_map (int proto_pte, int page_count, int refcnt, 
	struct _pte ** ret_svapte, void ** ret_sva, int *ret_pfn);
int mmg$alloc_pfn_map_sva (int proto_pte, int page_count, int refcnt,
	struct _pte * svapte, void ** ret_sva, int *ret_pfn);

#ifndef	MMG_STD$ALLOC_SYSTEM_VA_MAP_SRC
int mmg_std$alloc_system_va_map (const int proto_pte, const int page_count,
	const uint32 refcnt, const uint32 system_region, VOID_PPQ sva, ...);
#endif

#ifndef MMG_STD$ALLOC_PFN_MAP_SYSTEM_VA_SRC
int mmg_std$alloc_pfn_map_system_va (const int proto_pte, const int page_count,
	const uint32 refcnt, VOID_PQ sva, ...);
#endif

int mmg_std$dealloc_sva (int page_count, VOID_PQ sva);
int mmg$pteref_64 (VOID_PQ va, struct _rde *rde, PTE_PPQ va_pte);
int mmg_std$get_pte_for_va (VOID_PQ const va, struct _phd *const phd, PTE_PQ pte_p);
void mmg_std$rem_pfn_64 (int pfn, int list, PFN_PQ pfndbe_p);
void mmg_std$ins_pfnh_64 (int pfn, int list, PFN_PQ pfndbe_p);

int mmg_std$use_res_mem (uint32 page_count, uint32 char_count, char *name, uint32 flags, uint32 group, 
			uint32 *return_pfn, uint32 *reserved_pages);
int mmg_std$free_res_mem (uint32 char_count, char *name, uint32 flags, uint32 group, uint32 *freed_pages);
int mmg_std$return_res_mem (uint32 page_count, uint32 char_count, char *name, uint32 flags, uint32 group, ...);
int mmg_std$copy_rmd (uint32 rmd_count, uint32 char_count, char *name, uint32 flags, uint32 group, struct _rmd *copy_address);
int mmg$test_page(uint32 *pfn_count_p);
int mmg_std$rem_pfnh_64(int list, PFN_PPQ pfndbe_p);
void mmg$compute_mem_checksum(struct _pmreg *pmr, int32 bitmap_size, uint32 flag);

int mmg_std$map_pfns_gh (uint32 first_pfn, uint32 pfn_count, uint32 protection, VOID_PQ sva);
int mmg_std$alloc_map_pfns (uint32 pfn_count, uint32 protection, VOID_PQ sva);

int mmg_std$establish_freepte_list (PTELIST_PQ pte_list, VOID_PQ sva, uint64 pte_count);
int mmg_std$insque_free_pte (PTELIST_PQ pte_list, VOID_PQ sva, uint64 pte_count);
int mmg_std$remque_free_pte (PTELIST_PQ pte_list, uint64 pte_count);
int mmg_std$establish_freepfn_list (uint32 count, void (*callback_routine)(PRVPFN_PQ pfn_list), uint32 priority, PRVPFN_PPQ prvpfn, UINT32_PQ return_count);
int mmg_std$insque_free_pfn (PRVPFN_PQ pfn_list, uint32 count, UINT32_PQ return_count);
int mmg_std$remque_free_pfn (PRVPFN_PQ pfn_list, uint32 count);
int mmg_std$map_from_lists (PTELIST_PQ pte_list, PRVPFN_PQ prvpfn, uint32 protection, VOID_PPQ sva);
int mmg_std$unmap_to_lists (VOID_PQ sva, PTELIST_PQ pte_list, PRVPFN_PQ prvpfn);

int mmg$pfncheck (uint32 pfn, UINT32_PQ which_list);
int mmg$pfncheck_range (uint32 pfn, uint32 page_count);
int mmg$ptecheckw_process (void);
int mmg$ptecheckw_system (void);
void mmg$ptecheckw_process_ast(struct _pcb *pcb, struct _acb *acb);

int mmg$mem_config_info (UINT32_PQ max_mem_desc_nodes_p, UINT32_PQ max_frag_per_node_p);

int mmg$mem_config_pfn (uint32 pfn, INT_PQ page_type_p, UINT64_PQ part_id_p, UINT64_PQ comm_id_p);

int mmg$alloc_pfn_color_64 (uint32 flags, uint64 vpn, uint32 rad, PFN_PPQ pfndbe_p);
int mmg$alloc_zero_color_64 (uint32 flags, uint64 vpn, uint32 rad, PFN_PPQ pfndbe_p);		
int mmg$alloc_contig_color_a (uint32 pfn_count, uint32 flags, uint32 byte_align, uint32 rad, UINT32_PQ largest_chunk_p);	

int mmg$va_to_rad (VOID_PQ va, UINT32_PQ rad_p);

int mmg$relpfn(signed int pfn);
int mmg$incptref(struct _phd * phd, struct _mypte * pte);
int mmg$decptref(struct _phd * phd, struct _mypte * pte);
int mmg$makewsle(struct _pcb * p, struct _phd * phd, void * va, void * pte, signed int pfn);
int mmg$frewsle(struct _pcb * p, void * va);
int mmg$frewslx(struct _pcb * p, void * va,unsigned long * pte, unsigned long index);
int mmg$allocpagfil1(int size, int * page);
int mmg$dallocpagfil1(int page);
int mmg$delpag(int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);
int mmg$crepag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);
int mmg$credel(int acmode, void * first, void * last, void (* pagesubr)(), struct _va_range *inadr, struct _va_range *retadr, unsigned int acmodeagain, struct _pcb * p, int numpages);
int mmg$crepag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);
int mmg$fast_create(struct _pcb * p, struct _rde *rde, void * start_va, void * end_va, unsigned long pages, unsigned long prot_pte);
int mmg$fast_create_gptx(struct _pcb * p, struct _rde *rde, void * start_va, void * end_va, unsigned long pages, unsigned long prot_pte);
int mmg$vfysecflg(void);
int mmg$purgwsscn(int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);
int mmg$purgwspag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);
signed long mmg$allocpfn(void);
signed long mmg$rempfn(unsigned long type, int pfn);
mmg$delwslx(struct _pcb * pcb, struct _phd * phd, int index,int pte);
mmg$delpfnlst(int type, int pfn);
mmg$dalcstxscn(void);
mmg$gsdscn(void * input_gsnam, unsigned long long * input_ident, int section_flags, char *return_gsnam, unsigned long long  *return_ident, struct _gsd **gsd);
mmg$imgreset();
void mmg$purgempl(unsigned long command);
void mmg$wrtmfypag(void);
signed int mmg$ininewpfn(struct _pcb * p, struct _phd * phd, void * va, struct _mypte * pte);
signed long mmg$dallocpfn(struct _pfn * pageframe);
signed long mmg$allocontig(unsigned long num);
signed long mmg$allocontig_align(unsigned long num);
signed long mmg$allocontig_align(unsigned long num);
signed long mmg$inspfnt(unsigned long type, struct _pfn * pfn);
#ifdef __i386__
signed long mmg$inspfn(unsigned long type, struct _pfn * pfn, struct _pfn * list);
#else
signed long mmg$inspfn(unsigned long type, int pfn, int list);
#endif
signed long mmg$inspfnh(unsigned long type, struct _pfn * pfn);
mmg$delconpfn(struct _pcb * p, int pfn);

#include <linux/linkage.h>

asmlinkage int exe$mgblsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, struct _secid *ident, unsigned int relpag);
#if 0
asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned long long * ident, unsigned int relpag, unsigned /*short*/ int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc);
#endif

#endif
