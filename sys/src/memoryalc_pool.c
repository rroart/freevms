// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/config.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/mm.h>

#include<irpdef.h>
#include<npool_data.h>
#include<ssdef.h>
#include<system_data_cells.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <mmg_routines.h>

struct _myhead {
  struct _myhead * hd$l_flink;
  long hd$l_seq;
};

void exe$reclaim_pool_aggressive(void * pool);

// this has its parallell in exe$alonpagvar
int exe$allocate_pool(int requestsize, int pooltype, int alignment, unsigned int * allocatedsize, void ** returnblock) {
  int reqsize=requestsize;
  unsigned int * alosize_p=allocatedsize;
  void ** pool_p=returnblock;
  int sts=SS$_NORMAL;

  // extra from alononpaged
  if (reqsize&63)
    reqsize=((reqsize>>6)+1)<<6; // mm book said something about align 64

  *alosize_p=reqsize;

  struct _npool_data * pooldata = exe$gs_bap_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  struct _myhead * array = &lsthd->lsthds$q_listheads;

  if (reqsize<=8192) {
    *pool_p = exe$lal_remove_first(&array[reqsize>>6]);
    if (*pool_p) {
      check_packet(*pool_p,reqsize,0);
      poison_packet(*pool_p,reqsize,0);
    }
  } 
  // extra end

  if (*pool_p) 
    return sts;

  sts=exe$allocate(reqsize , &exe$gq_bap_variable, 0 , alosize_p, pool_p);

  // unlock pool

  if (sts==SS$_NORMAL)
    return sts;

#if 0
  struct _npool_data * pooldata = exe$gs_bap_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  void * array = &lsthd->lsthds$q_listheads;
#endif

  exe$reclaim_pool_aggressive(exe$gs_bap_npool);
  sts=exe$allocate(reqsize , &exe$gq_bap_variable, 0 , alosize_p, pool_p);

  if (sts==SS$_NORMAL)
    return sts;

  sts=exe$extendpool(exe$gs_bap_npool);
  if (sts==SS$_NORMAL)
    sts=exe$allocate(reqsize , &exe$gq_bap_variable, 0 , alosize_p, pool_p);

  if (sts==SS$_NORMAL)
    return sts;

  sts=exe$flushlists(exe$gs_bap_npool, reqsize);

  if (sts==SS$_NORMAL)
    sts=exe$allocate(reqsize , &exe$gq_bap_variable, 0 , alosize_p, pool_p);

  return sts;
}

void exe$deallocate_pool(void * returnblock, int pooltype, int size) {
  void * pool=returnblock;
  if (size&63)
    size=((size>>6)+1)<<6; // mm book said something about align 64

  if (size==0 || (size&63))
    panic("size size %x\n",size);

  struct _npool_data * pooldata = exe$gs_bap_npool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  struct _myhead * array = &lsthd->lsthds$q_listheads;

  if (size<=8192) {
    exe$lal_insert_first(pool, &array[size>>6]);
    poison_packet(pool,size,1);
  } else {
    int sts=exe$deallocate(pool, exe$gq_bap_variable, size);
  }
  return SS$_NORMAL;
}

exe$extend_npp(void * pool) {
  // 4 axppages is 8 386pages
  printk("exe$extendpool/npp not yet implemented (not so difficult?\n");
  return SS$_INSFMEM;
}

exe$extendpool(void * pool) {
  return exe$extend_npp(pool);
}

static int trim=1;

exe$trim_pool_list(int percentage, void * listhead, void * basepool) {
  
  struct _myhead * hd = listhead;
  int cut;
  cut = hd[trim].hd$l_seq * percentage / 100;
  for (; cut; cut--) {
    void * ret = exe$lal_remove_first(&hd[trim]);
    if (ret)
      exe$deallocate(ret, basepool, 64*trim);
  }
  cut = hd[64+trim].hd$l_seq * (100 - percentage) / 100;
  for (; cut; cut--) {
    void * ret = exe$lal_remove_first(&hd[64+trim]);
    if (ret)
      exe$deallocate(ret, basepool, 64*(64+trim));
  }
  trim++;
  if (trim>64)
    trim=1;
}

void exe$reclaim_pool_aggressive(void * pool) {
  struct _npool_data * pooldata = pool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  void * array = &lsthd->lsthds$q_listheads;
  void * listhead=array;
  void * basepool=lsthd->lsthds$l_variablelist_unused;
  exe$trim_pool_list(50,listhead, basepool);
}

void exe$reclaim_pool_gentle(void * pool) {
  printk("timer exe$reclaim_pool_gentle %x\n",trim);
  struct _npool_data * pooldata = pool;
  struct _lsthds * lsthd = pooldata->npool$ar_lsthds;
  void * array = &lsthd->lsthds$q_listheads;
  void * listhead=array;
  void * basepool=lsthd->lsthds$l_variablelist_unused;
  exe$trim_pool_list(85,listhead, basepool);
  signed long long time=-10000000*60;
  exe$setimr(0, &time, exe$reclaim_pool_gentle, exe$gs_npp_npool, 0);
}

