// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <linux/mm.h>
#include <pfndef.h>
#include <vmspte.h>
#include <misc_routines.h>
#include <mmg_routines.h>

int lasteech=0;

#undef OLDINT
#define OLDINT

#ifdef __x86_64__
#undef OLDINT
#endif

struct _mypfn {
#ifdef OLDINT
  struct _mypfn * pfn$l_flink;
  struct _mypfn * pfn$l_blink;
#else
  int pfn$l_flink;
  int pfn$l_blink;
#endif
};

signed long mmg$rempfnh(unsigned long type);
#ifdef OLDINT
signed long mmg$rempfn(unsigned long type, struct _pfn * pfn);
#else
signed long mmg$rempfn(unsigned long type, int pfn);
#endif

signed long mmg$allocpfn(void) {
  signed long pfn=mmg$rempfnh(PFN$C_FREPAGLST);
  if (pfn>=0) {
    //    mem_map[pfn].pfn$l_refcnt=0;
    // not yet? buggy gcc?
  }
  if (pfn>=0) {
    mmg$delconpfn(ctl$gl_pcb,pfn);
  }
  return pfn;
}

signed long mmg$rempfnh(unsigned long type) {
  return mmg$rempfn(type,pfn$al_head[type]);
}

#ifdef OLDINT
signed long mmg$rempfn(unsigned long type, struct _pfn * pfn) {
  struct _mypfn * h;
#else
signed long mmg$rempfn(unsigned long type, int pfn) {
  int h;
#endif
 if (0) {
  int k,l,m[24];
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",l);
    m[k]=mem_map[l].pfn$l_blink;
    l=mem_map[l].pfn$l_flink;
  }
  printk("\n");
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",m[k]);
  }
  printk("\n");
 }
  if (type==PFN$C_FREPAGLST)
    if (sch$gl_freecnt<(1+4)) return -1;
  if (type==PFN$C_MFYPAGLST)
    if (sch$gl_mfycnt<1) return -1;
  if (pfn$al_head[type]==0)
    return -1;
  if (pfn && pfn$al_head[type]==pfn$al_tail[type]) {
    pfn$al_head[type]=pfn$al_tail[type]=0;
    goto out;
  }

  // no tail check yet
  if (pfn==pfn$al_head[type]) {
#ifdef OLDINT
    h=pfn$al_head[type]=((struct _pfn *) pfn$al_head[type])->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
#else
    h=pfn$al_head[type]=mem_map[pfn$al_head[type]].pfn$l_flink;
    mem_map[h].pfn$l_blink=0; // gcc bug?
#endif
    pfn$al_head[type]=h;
    goto out;
  }

  if (pfn==pfn$al_tail[type]) {
#ifdef OLDINT
    h=pfn$al_tail[type]=((struct _pfn *) pfn$al_tail[type])->pfn$l_blink;
    h->pfn$l_flink=0; // gcc bug?
#else
    h=pfn$al_tail[type]=mem_map[pfn$al_tail[type]].pfn$l_blink;
    mem_map[h].pfn$l_flink=0; // gcc bug?
#endif
    pfn$al_tail[type]=h;
    goto out;
  }

  // ordinary case
#ifdef OLDINT
  struct _mypfn * cur=pfn;
  struct _mypfn * prev=cur->pfn$l_blink;
  struct _mypfn * next=cur->pfn$l_flink;
  prev->pfn$l_flink=next;
  next->pfn$l_blink=prev;
#else
  struct _mypfn * cur=&mem_map[pfn];
  int prev=cur->pfn$l_blink;
  int next=cur->pfn$l_flink;
  mem_map[prev].pfn$l_flink=next;
  mem_map[next].pfn$l_blink=prev;
#endif

  //  pfn->pfn$l_refcnt=0;
#if 0
  if (h->pfn$l_flink==0) panic("eech %x\n",lasteech);
  // not yet, struct bugs
  if (pfn->pfn$l_refcnt)
    panic("refcnt\n");
#endif
 out:
  if (type==PFN$C_FREPAGLST) sch$gl_freecnt--;
  if (type==PFN$C_MFYPAGLST) sch$gl_mfycnt--;
#if 0
  if (((struct _pfn *) pfn$al_head[type])->pfn$l_flink==0) {
    panic("eech4\n");
  }
#endif
  lasteech=1;
#ifdef OLDINT
  return (((unsigned long)pfn-(unsigned long)mem_map)/sizeof(struct _pfn));
#else
  return pfn;
#endif
}

signed long mmg$allocontig(unsigned long num) {
#ifdef OLDINT
  struct _pfn * p, * first=pfn$al_head[PFN$C_FREPAGLST], * next;
  struct _mypfn * h, * m;
#else
  int p, first=pfn$al_head[PFN$C_FREPAGLST], next;
  int h, m;
#endif
  unsigned long done=0, c;
  lasteech=2;
  if (sch$gl_freecnt<(num+4)) return -1;
  if (!pfn$al_head)
    return -1;
  while(!done) {
    for(p=first,c=1;c<num;c++) {
      next=p++;
#ifdef OLDINT
      if (next->pfn$l_flink!=p) goto out;
#else
      if (mem_map[next].pfn$l_flink!=p) goto out;
#endif
    }
    done=1;
  out:
#ifdef OLDINT
    if (!done) first=first->pfn$l_flink;
#else
    if (!done) first=mem_map[first].pfn$l_flink;
#endif
  }

  if (first==pfn$al_head) {
#ifdef OLDINT
    h=pfn$al_head[PFN$C_FREPAGLST]=p->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
#else
    h=pfn$al_head[PFN$C_FREPAGLST]=mem_map[p].pfn$l_flink;
    mem_map[h].pfn$l_blink=0; // gcc bug?
#endif
  } else {
    h=first;
#ifdef OLDINT
    ((struct _mypfn *)h->pfn$l_blink)->pfn$l_flink=p->pfn$l_flink;// gcc bug?
    ((struct _mypfn *)((struct _mypfn *)h->pfn$l_blink)->pfn$l_flink)->pfn$l_blink=h->pfn$l_blink; // gcc bug?
#else
    mem_map[mem_map[h].pfn$l_blink].pfn$l_flink=mem_map[p].pfn$l_flink;// gcc bug?
    mem_map[mem_map[mem_map[h].pfn$l_blink].pfn$l_flink].pfn$l_blink=mem_map[h].pfn$l_blink; // gcc bug?
#endif
  }

#if 0
  // not yet, struct bugs
  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif
  sch$gl_freecnt-=num;
#ifdef OLDINT
  for(c=first-&mem_map[0];num;c++,num--) {
#else
  for(c=first;num;c++,num--) {
#endif
    mmg$delconpfn(ctl$gl_pcb,c);
  }
#ifdef OLDINT
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
#else
  return first;
#endif
}

// drop doing this sorted some time later
#ifdef OLDINT
signed long mmg$inspfn(unsigned long type, struct _pfn * pfn, struct _pfn * list) {
  struct _mypfn * m=pfn, * tmp;
#else
signed long mmg$inspfn(unsigned long type, int pfn, int list) {
  int m=pfn, tmp;
#endif

  lasteech=3;

#ifdef OLDINT
  pfn->pfn$v_loc=type;
#else
  mem_map[pfn].pfn$v_loc=type;
#endif

  if (list) {
    tmp=list; 
  } else {
    tmp=pfn$al_head[type];
#ifdef OLDINT
    while (tmp!=pfn$al_tail[type] && pfn>tmp->pfn$l_flink) {
      tmp=tmp->pfn$l_flink;
#else
    while (tmp!=pfn$al_tail[type] && pfn>mem_map[tmp].pfn$l_flink) {
      tmp=mem_map[tmp].pfn$l_flink;
#endif
    }
  }

  mypfncheckaddr();

  //special for new everything? try to not need this eventually?
  if (m && pfn$al_head[type]==0 && pfn$al_tail[type]==0) {
    pfn$al_head[type]=m;
    pfn$al_tail[type]=m;
#ifdef OLDINT
    m->pfn$l_blink=0;
    m->pfn$l_flink=0;
#else
    mem_map[m].pfn$l_blink=0;
    mem_map[m].pfn$l_flink=0;
#endif
    goto out;
  }

  // new head?
  if (m<pfn$al_head[type]) {
#ifdef OLDINT
    m->pfn$l_flink=pfn$al_head[type];
    m->pfn$l_blink=0;
    tmp->pfn$l_blink=m;
#else
    mem_map[m].pfn$l_flink=pfn$al_head[type];
    mem_map[m].pfn$l_blink=0;
    mem_map[tmp].pfn$l_blink=m;
#endif
    pfn$al_head[type]=m;
    goto out;
  }

  // new tail?
#ifdef OLDINT
  if (m>tmp && tmp->pfn$l_flink==0) {
    tmp->pfn$l_flink=m;
    m->pfn$l_flink=0;
    m->pfn$l_blink=pfn$al_tail[type];
#else
  if (m>tmp && mem_map[tmp].pfn$l_flink==0) {
    mem_map[tmp].pfn$l_flink=m;
    mem_map[m].pfn$l_flink=0;
    mem_map[m].pfn$l_blink=pfn$al_tail[type];
#endif
    pfn$al_tail[type]=m;
    goto out;
  }

  // ordinary case?
#ifdef OLDINT
  if (m>tmp && m<tmp->pfn$l_flink) { 
    m->pfn$l_flink=tmp->pfn$l_flink;
    m->pfn$l_blink=tmp;
    ((struct _mypfn *)tmp->pfn$l_flink)->pfn$l_blink=m;
    tmp->pfn$l_flink=m;
#else
  if (m>tmp && m<mem_map[tmp].pfn$l_flink) { 
    mem_map[m].pfn$l_flink=mem_map[tmp].pfn$l_flink;
    mem_map[m].pfn$l_blink=tmp;
    mem_map[mem_map[tmp].pfn$l_flink].pfn$l_blink=m;
    mem_map[tmp].pfn$l_flink=m;
#endif
    goto out;
  }

#if 0
  // do this check for a while
  // not yet, struct bugs
  if (mem_map[pfn].pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif

 out:
  //  if (mem_map[pfn].pfn$l_flink) panic("dalloc\n");
  //((struct _pfn *)pfn$al_tail[type])->pfn$l_flink=&mem_map[pfn];
  mypfncheckaddr();
  if (type==PFN$C_FREPAGLST) sch$gl_freecnt++;
  if (type==PFN$C_MFYPAGLST) sch$gl_mfycnt++;
}

signed long mmg$dallocpfn(struct _pfn * pageframe) {
  signed long pfn=mmg$inspfn(PFN$C_FREPAGLST,pageframe,0);
#if 0
#ifdef __i386__
  // check. debug
  pageframe->pfn$l_pt_pfn = (void*) 0x42424242;
#endif
#endif
  return pfn;
  // should be tail really
}

signed long mmg$inspfnh(unsigned long type, struct _pfn * pfn) {
  return mmg$inspfn(type,pfn,pfn$al_head[type]);
}

signed long mmg$inspfnt(unsigned long type, struct _pfn * pfn) {
  return mmg$inspfn(type,pfn,pfn$al_tail[type]);
}

/* at least task_struct need to be on a 8k/16k aligned va */
#ifdef OLDINT
signed long mmg$allocontig_align(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head[PFN$C_FREPAGLST], * next;
  struct _mypfn * h, * m;
#else
signed long mmg$allocontig_align(unsigned long num) {
  int p, first=pfn$al_head[PFN$C_FREPAGLST], next;
  int h, m;
#endif
  p = 0;
  //  printk("n %lx\n",num);
 if (0) {
  int k,l,m[24];
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",l);
    m[k]=mem_map[l].pfn$l_blink;
    l=mem_map[l].pfn$l_flink;
  }
  printk("\n");
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",m[k]);
  }
  printk("\n");
 }
  unsigned long done=0, c;
  unsigned long count=0;
  lasteech=4;
  if (sch$gl_freecnt<(num+4)) return -1;
  if (!pfn$al_head[PFN$C_FREPAGLST])
    return -1;
  while(!done) {
#ifdef OLDINT
    if (!(((unsigned long)page_address(first)) & ( (num<<PAGE_SHIFT)-1))) {
#else
    if (!(((unsigned long)page_address(&mem_map[first])) & ( (num<<PAGE_SHIFT)-1))) {
#endif
      for(p=first,c=1;c<num;c++) {
	next=p++;
#ifdef OLDINT
	if (next->pfn$l_flink!=p) goto out;
#else
	//printk("t %lx %lx %lx\n",next,p,mem_map[next].pfn$l_flink);
	if (mem_map[next].pfn$l_flink!=p) goto out;
#endif
      }
      done=1;
    }
  out:
    count++;
#ifdef OLDINT
    if ((first->pfn$l_flink==0) /*|| ((unsigned long)first->pfn$l_flink>(unsigned long)&mem_map[8193])*/) panic("eech3\n");
    if (!done) first=first->pfn$l_flink;
#else
    if ((mem_map[first].pfn$l_flink==0) /*|| ((unsigned long)first->pfn$l_flink>(unsigned long)&mem_map[8193])*/) panic("eech3\n");
    if (!done) first=mem_map[first].pfn$l_flink;
#endif
  }

  if (first==pfn$al_head[PFN$C_FREPAGLST]) {
#ifdef OLDINT
    h=pfn$al_head[PFN$C_FREPAGLST]=p->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
#else
    h=pfn$al_head[PFN$C_FREPAGLST]=mem_map[p].pfn$l_flink;
    mem_map[h].pfn$l_blink=0; // gcc bug?
#endif
  } else {
    h=first;
#ifdef OLDINT
    h->pfn$l_blink->pfn$l_flink=p->pfn$l_flink;// gcc bug?
    h->pfn$l_blink->pfn$l_flink->pfn$l_blink=h->pfn$l_blink; // gcc bug?
#else
    mem_map[mem_map[h].pfn$l_blink].pfn$l_flink=mem_map[p].pfn$l_flink;// gcc bug?
    mem_map[mem_map[mem_map[h].pfn$l_blink].pfn$l_flink].pfn$l_blink=mem_map[h].pfn$l_blink; // gcc bug?
#endif
  }

#if 0
  // not yet, struct bugs
  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif
  sch$gl_freecnt-=num;
#ifdef OLDINT
  for(c=first-&mem_map[0];num;c++,num--) {
#else
  for(c=first;num;c++,num--) {
#endif
    mmg$delconpfn(ctl$gl_pcb,c);
  }
  if (0) {
  int k,l,m[24];
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",l);
    m[k]=mem_map[l].pfn$l_blink;
    l=mem_map[l].pfn$l_flink;
  }
  printk("\n");
  l=pfn$al_head[0];
  for(k=0;k<24;k++) {
    printk("%lx ",m[k]);
  }
  printk("\n");
 }
#ifdef OLDINT
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
#else
  return first;
#endif
}

mypfncheckaddr(){
#if 0
  // for debugging "only"
  int i,n=0,m=0;
#ifdef OLDINT
  struct _mypfn  *tmp2;
#else
  int tmp2;
#endif
  unsigned long tmp;
  tmp=pfn$al_head[PFN$C_FREPAGLST];
  tmp2=tmp;
#ifdef OLDINT
  tmp2=tmp2->pfn$l_flink;
#else
  tmp2=mem_map[tmp2].pfn$l_flink;
#endif
  do {
    n++;
#ifdef OLDINT
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_flink;
#else
    if (tmp2!=(mem_map[mem_map[tmp2].pfn$l_flink].pfn$l_blink)) goto mypanic;
    if (tmp2!=(mem_map[mem_map[tmp2].pfn$l_blink].pfn$l_flink)) goto mypanic;
    tmp2=mem_map[tmp2].pfn$l_flink;
#endif
  } while (tmp2!=pfn$al_tail[PFN$C_FREPAGLST]);
  n--;
  tmp=pfn$al_tail[PFN$C_FREPAGLST];
  tmp2=tmp;
#ifdef OLDINT
  tmp2=tmp2->pfn$l_blink;
#else
  tmp2=mem_map[tmp2].pfn$l_blink;
#endif
  do {
    m++;
#ifdef OLDINT
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_blink;
#else
    if (tmp2!=(mem_map[mem_map[tmp2].pfn$l_flink].pfn$l_blink)) goto mypanic;
    if (tmp2!=(mem_map[mem_map[tmp2].pfn$l_blink].pfn$l_flink)) goto mypanic;
    tmp2=mem_map[tmp2].pfn$l_blink;
#endif
  } while (tmp2!=pfn$al_head[PFN$C_FREPAGLST]);
  m--;
  if (n!=m) goto mypanic;
  return;
 mypanic:
  printk("mypanic %x %x %x %x %x\n",i,n,m,tmp,tmp2);
#ifdef OLDINT
  printk("mypanic %x %x %x %x %x\n",tmp2->pfn$l_flink,tmp2->pfn$l_flink->pfn$l_blink,tmp2->pfn$l_blink,tmp2->pfn$l_blink->pfn$l_flink,42);
#else
  printk("mypanic %x %x %x %x %x\n",mem_map[tmp2].pfn$l_flink,mem_map[mem_map[tmp2].pfn$l_flink].pfn$l_blink,mem_map[tmp2].pfn$l_blink,mem_map[mem_map[tmp2].pfn$l_blink].pfn$l_flink,42);
#endif
  cli();
  while(1) {; };
  sickinsque(0x11111111,0x22222222);
#endif 
}

#ifdef CONFIG_VMS

int mmg$relpfn(signed int pfn) {
  pte_t * pte = mem_map[pfn].pfn$q_pte_index;
  *(unsigned long *)pte&=~(_PAGE_PRESENT|_PAGE_TYP1);
#ifndef __arch_um__
  __flush_tlb(); //flush_tlb_range(current->mm, page, page + PAGE_SIZE);
#endif
  if (mem_map[pfn].pfn$l_page_state&PFN$M_MODIFY) {
    // do more dealloc
    // maybe backingstore related?
#ifdef OLDINT
    mmg$inspfn/* really t*/(PFN$C_MFYPAGLST,&mem_map[pfn],0);
#else
    mmg$inspfn/* really t*/(PFN$C_MFYPAGLST,pfn,0);
#endif

  } else {
#ifdef OLDINT
    mmg$inspfn(PFN$C_FREPAGLST,&mem_map[pfn],0); // really inspfnh
#else
    mmg$inspfn(PFN$C_FREPAGLST,pfn,0); // really inspfnh
#endif
  }
}

mmg$delconpfn(struct _pcb * p, int pfn) {
  struct _pfn * pfnp=&mem_map[pfn];
  unsigned long * pte = pfnp->pfn$q_pte_index;
  if (pte) {
    *pte=pfnp->pfn$q_bak;
#ifndef __arch_um__
    __flush_tlb(); //flush_tlb_range(current->mm, page, page + PAGE_SIZE);
#endif
    mmg$decptref(p->pcb$l_phd,pte);
    pfnp->pfn$q_pte_index=0;
  }
}

mmg$delpfnlst(int type, int pfn) {
#ifdef OLDINT
  mmg$rempfn(type,&mem_map[pfn]);
#else
  mmg$rempfn(type,pfn);
#endif
  mmg$relpfn(pfn);
}

#endif
