// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <linux/mm.h>
#include <pfndef.h>

int lasteech=0;

struct _mypfn {
  struct _mypfn * pfn$l_flink;
  struct _mypfn * pfn$l_blink;
};

signed long mmg$rempfnh(unsigned long type);
signed long mmg$rempfn(unsigned long type, struct _pfn * pfn);

signed long mmg$allocpfn(void) {
  signed long pfn=mmg$rempfnh(PFN$C_FREPAGLST);
  if (pfn>=0) {
    //    mem_map[pfn].pfn$l_refcnt=0;
    // not yet? buggy gcc?
  }
  return pfn;
}

signed long mmg$rempfnh(unsigned long type) {
  return mmg$rempfn(type,pfn$al_head[type]);
}

signed long mmg$rempfn(unsigned long type, struct _pfn * pfn) {
  struct _mypfn * h;
  if (sch$gl_freecnt<(1+4)) return -1;
  if (!pfn$al_head[type])
    return -1;
  if (pfn$al_head[type]==pfn$al_tail[type]) panic("eech2\n");
  // no tail check yet
  if (pfn==pfn$al_head[type]) {
    h=pfn$al_head[type]=((struct _pfn *) pfn$al_head[type])->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
  }
  //  pfn->pfn$l_refcnt=0;
  if (h->pfn$l_flink==0) panic("eech %x\n",lasteech);
#if 0
  // not yet, struct bugs
  if (pfn->pfn$l_refcnt)
    panic("refcnt\n");
#endif
  if (type==PFN$C_FREPAGLST) sch$gl_freecnt--;
  if (type==PFN$C_MFYPAGLST) sch$gl_mfycnt--;
  if (((struct _pfn *) pfn$al_head[type])->pfn$l_flink==0) {
    panic("eech4\n");
  }
  lasteech=1;
  return (((unsigned long)pfn-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$allocontig(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head[PFN$C_FREPAGLST], * next;
  struct _mypfn * h, * m;
  unsigned long done=0, c;
  lasteech=2;
  if (sch$gl_freecnt<(num+4)) return -1;
  if (!pfn$al_head)
    return -1;
  while(!done) {
    for(p=first,c=1;c<num;c++) {
      next=p++;
      if (next->pfn$l_flink!=p) goto out;
    }
    done=1;
  out:
    if (!done) first=first->pfn$l_flink;
  }

  if (first==pfn$al_head) {
    h=pfn$al_head[PFN$C_FREPAGLST]=p->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
  } else {
    h=first;
    ((struct _mypfn *)h->pfn$l_blink)->pfn$l_flink=p->pfn$l_flink;// gcc bug?
    ((struct _mypfn *)((struct _mypfn *)h->pfn$l_blink)->pfn$l_flink)->pfn$l_blink=h->pfn$l_blink; // gcc bug?
  }

#if 0
  // not yet, struct bugs
  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif
  sch$gl_freecnt-=num;
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$inspfn(unsigned long type, struct _pfn * pfn, struct _pfn * list) {
  struct _mypfn * m=pfn, * tmp;

  lasteech=3;

  if (list) {
    tmp=list; 
  } else {
    tmp=pfn$al_head[type];
    while (tmp!=pfn$al_tail[type] && pfn>tmp->pfn$l_flink) {
      tmp=tmp->pfn$l_flink;
    }
  }

  mypfncheckaddr();

  //special for new everything? try to not need this eventually?
  if (m && pfn$al_head[type]==0 && pfn$al_tail[type]==0) {
    pfn$al_head[type]=m;
    pfn$al_tail[type]=m;
    m->pfn$l_blink=0;
    m->pfn$l_flink=0;
    goto out;
  }

  // new head?
  if (m<pfn$al_head[type]) {
    m->pfn$l_flink=pfn$al_head[type];
    m->pfn$l_blink=0;
    tmp->pfn$l_blink=m;
    pfn$al_head[type]=m;
    goto out;
  }

  // ordinary case?
  if (m>tmp && m<tmp->pfn$l_flink) { 
    m->pfn$l_flink=tmp->pfn$l_flink;
    m->pfn$l_blink=tmp;
    ((struct _mypfn *)tmp->pfn$l_flink)->pfn$l_blink=m;
    tmp->pfn$l_flink=m;
    goto out;
  }

  // new tail?
  if (m>tmp && tmp->pfn$l_flink==0) {
    tmp->pfn$l_flink=m;
    m->pfn$l_flink=0;
    m->pfn$l_blink=pfn$al_tail[type];
    pfn$al_tail[type]=m;
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
signed long mmg$allocontig_align(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head[PFN$C_FREPAGLST], * next;
  struct _mypfn * h, * m;
  unsigned long done=0, c;
  unsigned long count=0;
  lasteech=4;
  if (sch$gl_freecnt<(num+4)) return -1;
  if (!pfn$al_head[PFN$C_FREPAGLST])
    return -1;
  while(!done) {
    if (!(((unsigned long)first->virtual) & ( (num<<PAGE_SHIFT)-1))) {
      for(p=first,c=1;c<num;c++) {
	next=p++;
	if (next->pfn$l_flink!=p) goto out;
      }
      done=1;
    }
  out:
    count++;
    if ((first->pfn$l_flink==0) /*|| ((unsigned long)first->pfn$l_flink>(unsigned long)&mem_map[8193])*/) panic("eech3\n");
    if (!done) first=first->pfn$l_flink;
  }

  if (first==pfn$al_head[PFN$C_FREPAGLST]) {
    h=pfn$al_head[PFN$C_FREPAGLST]=p->pfn$l_flink;
    h->pfn$l_blink=0; // gcc bug?
  } else {
    h=first;
    h->pfn$l_blink->pfn$l_flink=p->pfn$l_flink;// gcc bug?
    h->pfn$l_blink->pfn$l_flink->pfn$l_blink=h->pfn$l_blink; // gcc bug?
  }

#if 0
  // not yet, struct bugs
  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif
  sch$gl_freecnt-=num;
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
}

mypfncheckaddr(){
#if 0
  // for debugging "only"
  int i,n=0,m=0;
  struct _mypfn  *tmp2;
  unsigned long tmp;
  tmp=pfn$al_head[PFN$C_FREPAGLST];
  tmp2=tmp;
  tmp2=tmp2->pfn$l_flink;
  do {
    n++;
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_flink;
  } while (tmp2!=pfn$al_tail[PFN$C_FREPAGLST]);
  n--;
  tmp=pfn$al_tail[PFN$C_FREPAGLST];
  tmp2=tmp;
  tmp2=tmp2->pfn$l_blink;
  do {
    m++;
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_blink;
  } while (tmp2!=pfn$al_head[PFN$C_FREPAGLST]);
  m--;
  if (n!=m) goto mypanic;
  return;
 mypanic:
  printk("mypanic %x %x %x %x %x\n",i,n,m,tmp,tmp2);
  printk("mypanic %x %x %x %x %x\n",tmp2->pfn$l_flink,tmp2->pfn$l_flink->pfn$l_blink,tmp2->pfn$l_blink,tmp2->pfn$l_blink->pfn$l_flink,42);
  cli();
  while(1) {; };
  sickinsque(0x11111111,0x22222222);
#endif 
}

#ifdef CONFIG_VMS

int mmg$relpfn(signed int pfn) {
  if (mem_map[pfn].pfn$l_page_state&PFN$M_MODIFY) {
    // do more dealloc
    // maybe backingstore related?
    mmg$inspfnt(PFN$C_MFYPAGLST,&mem_map[pfn]);

  } else {
    mmg$inspfn(PFN$C_FREPAGLST,&mem_map[pfn],0); // really inspfnh
  }
}

#endif
