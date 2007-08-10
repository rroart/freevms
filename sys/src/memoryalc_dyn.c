// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/config.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include<irpdef.h>
#include<npool_data.h>
#include<ssdef.h>
#include<system_data_cells.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <mmg_routines.h>
#include <internals.h>
#include <ipldef.h>

// move this later
//unsigned long long ioc$gq_listheads[128];

struct _myhead {
  struct _myhead * hd$l_flink;
  long hd$l_seq;
};

struct mymap {
  unsigned long flink;
  unsigned long size;
};

struct _npool_data npp_pool;
struct _npool_data bap_pool;
struct _lsthds npp_listheads;
struct _lsthds bap_listheads;

void __init init_nonpaged_pool(void * pgdat, int size, struct _npool_data * pool, void ** start , void ** free) {
  struct mymap * mytmp;

  *start=alloc_bootmem_node(pgdat,size);
  *start= (struct page *)(PAGE_OFFSET + 
				    MAP_ALIGN((unsigned long)(*start) - PAGE_OFFSET));
  *free= *start;
  struct _lsthds * l = pool->npool$ar_lsthds;
  l->lsthds$l_variablelist_unused=*free;
  mytmp=*start;
  mytmp->flink=0; /* ? */
  mytmp->size=size;
  poison_packet(mytmp,size,1);
}

void __init init_nonpaged(void *pgdat, unsigned long totalpages) {
  memset(&npp_pool, 0, sizeof(npp_pool));
  memset(&bap_pool, 0, sizeof(bap_pool));
  memset(&npp_listheads, 0, sizeof(npp_listheads));
  memset(&bap_listheads, 0, sizeof(npp_listheads));
  npp_pool.npool$ar_lsthds=&npp_listheads;
  bap_pool.npool$ar_lsthds=&bap_listheads;
  exe$gs_npp_npool=&npp_pool;
  exe$gs_bap_npool=&bap_pool;
  exe$gs_npp_base_lsthds=&npp_listheads;
  exe$gs_bap_base_lsthds=&bap_listheads;

  //memset(&ioc$gq_listheads[0], 0, sizeof(ioc$gq_listheads));
  // ordinary nonpaged pool
  //override for now; half space is for nonpaged pool
  sgn$gl_npagedyn=totalpages<<(PAGE_SHIFT-2); // half again/quarter

  init_nonpaged_pool(pgdat, 2*1024*1024, exe$gs_bap_npool, &mmg$gq_bap, &exe$gq_bap_variable);
  init_nonpaged_pool(pgdat, totalpages<<(PAGE_SHIFT-2), exe$gs_npp_npool, &mmg$gl_npagedyn, &exe$gl_nonpaged);

#if 0
  void exe$reclaim_pool_gentle(void * pool);
  signed long long time=-10000000*60;
  exe$setimr(0, &time, exe$reclaim_pool_gentle, exe$gs_npp_npool, 0);
#endif
}

// this has a parallell in exe$allocate_pool
int exe$alononpagvar (int reqsize, int *alosize_p, void **pool_p) {
  // round up to nearest 16 should be moved here, and the statics fixed
  // pool spinlock etc
  int sts=SS$_NORMAL;

  int ipl = vmslock(&SPIN_POOL, IPL$_POOL);
  sts=exe$allocate(reqsize , &exe$gl_nonpaged, 0 , alosize_p, pool_p);
  vmsunlock(&SPIN_POOL, ipl);

  // unlock pool

  if (sts==SS$_NORMAL)
    return sts;

  struct _npool_data * pooldata = exe$gs_npp_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  void * array = &lsthd->lsthds$q_listheads;

  exe$reclaim_pool_aggressive(exe$gs_npp_npool);
  ipl = vmslock(&SPIN_POOL, IPL$_POOL);
  sts=exe$allocate(reqsize , &exe$gl_nonpaged, 0 , alosize_p, pool_p);
  vmsunlock(&SPIN_POOL, ipl);

  if (sts==SS$_NORMAL)
    return sts;

  sts=exe$extendpool(exe$gs_npp_npool);
  if (sts==SS$_NORMAL) {
    int ipl = vmslock(&SPIN_POOL, IPL$_POOL);
    sts=exe$allocate(reqsize , &exe$gl_nonpaged, 0 , alosize_p, pool_p);
    vmsunlock(&SPIN_POOL, ipl);
  }

  if (sts==SS$_NORMAL)
    return sts;

  sts=exe$flushlists(exe$gs_npp_npool, reqsize);

  if (sts==SS$_NORMAL) {
    int ipl = vmslock(&SPIN_POOL, IPL$_POOL);
    sts=exe$allocate(reqsize , &exe$gl_nonpaged, 0 , alosize_p, pool_p);
    vmsunlock(&SPIN_POOL, ipl);
  }

  return sts;
}

int exe_std$alononpaged (int reqsize, int *alosize_p, void **pool_p) {
  int sts=SS$_NORMAL;
#if 0
  if (reqsize<=srpsize) {
    if (rqempty(&ioc$gq_srpiq))
      goto var;
    int addr=remqti(&ioc$gq_srpiq,addr);
    *alosize_p=reqsize;
    *pool_p=addr;
    return sts;
  }
  if (reqsize<=irpsize) {
    if (rqempty(&ioc$gq_irpiq))
      goto var;
    int addr=remqti(&ioc$gq_irpiq,addr);
    *alosize_p=reqsize;
    *pool_p=addr;
    return sts;
  }
  if (reqsize >= lrpmin && reqsize<=srpsize) {
    if (rqempty(&ioc$gq_lrpiq))
      goto var;
    int addr=remqti(&ioc$gq_srpiq,addr);
    *alosize_p=reqsize;
    *pool_p=addr;
    return sts;
  }
#endif
  if (reqsize&63)
    reqsize=((reqsize>>6)+1)<<6; // mm book said something about align 64

  *alosize_p=reqsize;

  struct _npool_data * pooldata = exe$gs_npp_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  struct _myhead * array = &lsthd->lsthds$q_listheads;

  if (reqsize<=8192) {
    *pool_p = exe$lal_remove_first(&array[reqsize>>6]);
    if (*pool_p) {
      check_packet(*pool_p,reqsize,0);
#if 0
      poison_packet(*pool_p,reqsize,0);
#endif
    }
  } 

  if (0==*pool_p) 
    sts = exe$alononpagvar(reqsize, alosize_p, pool_p);

  return sts;
}

int exe_std$deanonpgdsiz(void *pool, int size) {

#if 0
  // now taking some chances with addresses

  if (mmg$gl_npagedyn<=pool && pool<=ioc$gl_lrpsplit) {
    struct _irp * irp = pool;
    if (irp->irp$w_size)
      panic("irp size\n");
    int sts=exe$deallocate(pool, exe$gl_nonpaged, irp->irp$w_size);
    return sts;
  }

  if (ioc$gl_lrpsplit<=pool && pool<=ioc$gl_splitadr) {
    insqti(pool,&ioc$gl_lrpsplit);
    return SS$_NORMAL;
  }

  if (ioc$gl_splitadr<=pool && pool<=ioc$gl_srpsplit) {
    insqti(pool,&ioc$gl_splitadr);
    return SS$_NORMAL;
  }

  if (ioc$gl_srpsplit<=pool && pool<=(ioc$gl_srpsplit+4*512*srpsize)) {
    insqti(pool,&ioc$gl_srpsplit);
    return SS$_NORMAL;
  }  
  
  return 0;
#endif

  if (size&63)
    size=((size>>6)+1)<<6; // mm book said something about align 64

  if (size==0 || (size&63))
    panic("size size %x\n",size);

  struct _npool_data * pooldata = exe$gs_npp_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  struct _myhead * array = &lsthd->lsthds$q_listheads;

  if (size<=8192) {
    poison_packet(pool,size,1);
    exe$lal_insert_first(pool, &array[size>>6]);
  } else {
    int ipl = vmslock(&SPIN_POOL, IPL$_POOL);
    int sts=exe$deallocate(pool, exe$gl_nonpaged, size);
    vmsunlock(&SPIN_POOL, ipl);
  }
  return SS$_NORMAL;
}

int exe_std$deanonpaged (void *pool) {
  struct _irp * irp = pool;
  return exe_std$deanonpgdsiz(pool, irp->irp$w_size);
}

int exe$flushlists(void * pool, int size) {

  struct _npool_data * pooldata = pool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  void * array = &lsthd->lsthds$q_listheads;
  void * listhead=array;
  void * basepool=lsthd->lsthds$l_variablelist_unused;

  struct _myhead * hd = array;

  int i=size>>6;

  for(;i<129;i++) {
    void * ret = exe$lal_remove_first(&hd[i]);
    if (ret==0)
      break;
    int ipl = vmslock(&SPIN_POOL, IPL$_POOL);
    exe$deallocate(ret, basepool, 64*i);
    vmsunlock(&SPIN_POOL, ipl);
  }
}
