// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <linux/mm.h>
#include <pfndef.h>

struct _mypfn {
  struct _mypfn * pfn$l_flink;
  struct _mypfn * pfn$l_blink;
};

signed long mmg$allocpfn(void) {
  struct _pfn * p;
  struct _mypfn * m, * h;
  if (sch$gl_freecnt<(1+4)) return -1;
  if (!pfn$al_head)
    return -1;
  if (pfn$al_head==pfn$al_tail) panic("eech2\n");
  m=p=pfn$al_head;
  h=pfn$al_head=((struct _pfn *) pfn$al_head)->pfn$l_flink;
  h->pfn$l_blink=0; // gcc bug?
  //  p->pfn$l_refcnt=0;
  if (h->pfn$l_flink==0) panic("eech\n");
#if 0
  // not yet, struct bugs
  if (p->pfn$l_refcnt)
    panic("refcnt\n");
#endif
  sch$gl_freecnt--;
  return (((unsigned long)p-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$allocontig(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head, * next;
  struct _mypfn * h, * m;
  unsigned long done=0, c;
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
    h=pfn$al_head=p->pfn$l_flink;
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

signed long mmg$dallocpfn(unsigned long pfn) {
  struct _mypfn * m=&mem_map[pfn];
  //  mem_map[pfn].pfn$l_flink=0;
  //mem_map[pfn].pfn$l_blink=pfn$al_tail;
  //return;

#if 0
  // do not do original algorithm for now
  m->pfn$l_flink=0;
  m->pfn$l_blink=pfn$al_tail;
  if (mem_map[pfn].pfn$l_flink) panic("dalloc\n");
  ((struct _pfn *)pfn$al_tail)->pfn$l_flink=&mem_map[pfn];
  pfn$al_tail=&mem_map[pfn];
  sch$gl_freecnt++;
#else
  // do one that "sorts"; we do not want totally fragmentation and panic
  struct _mypfn * tmp = pfn$al_head;

#if 0
  // do this check for a while
  // not yet, struct bugs
  if (mem_map[pfn].pfn$l_refcnt) // do more 
    panic("refcnt\n");
#endif

  mypfncheckaddr();
  if (m<pfn$al_head) {
    m->pfn$l_flink=pfn$al_head;
    m->pfn$l_blink=0;
    tmp->pfn$l_blink=m;
    pfn$al_head=m;
    goto out;
  }
  while (tmp!=pfn$al_tail) {
    if (m>tmp && m<tmp->pfn$l_flink) { 
      m->pfn$l_flink=tmp->pfn$l_flink;
      m->pfn$l_blink=tmp;
      ((struct _mypfn *)tmp->pfn$l_flink)->pfn$l_blink=m;
      tmp->pfn$l_flink=m;
      goto out;
    }
    if (m>tmp && tmp->pfn$l_flink==0) {
      tmp->pfn$l_flink=m;
      m->pfn$l_flink=0;
      m->pfn$l_blink=pfn$al_tail;
      pfn$al_tail=m;
      goto out;
    }
    tmp=tmp->pfn$l_flink;
  }
 out:
  //  if (mem_map[pfn].pfn$l_flink) panic("dalloc\n");
  //((struct _pfn *)pfn$al_tail)->pfn$l_flink=&mem_map[pfn];
  mypfncheckaddr();
  sch$gl_freecnt++;
#endif
}

/* at least task_struct need to be on a 8k/16k aligned va */
signed long mmg$allocontig_align(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head, * next;
  struct _mypfn * h, * m;
  unsigned long done=0, c;
  unsigned long count=0;
  if (sch$gl_freecnt<(num+4)) return -1;
  if (!pfn$al_head)
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
    if ((first->pfn$l_flink==0) || ((unsigned long)first->pfn$l_flink>(unsigned long)&mem_map[8193])) panic("eech3\n");
    if (!done) first=first->pfn$l_flink;
  }

  if (first==pfn$al_head) {
    h=pfn$al_head=p->pfn$l_flink;
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

mypfncheckaddr(){
#if 0
  // for debugging "only"
  int i,n=0,m=0;
  struct _mypfn  *tmp2;
  unsigned long tmp;
  tmp=pfn$al_head;
  tmp2=tmp;
  tmp2=tmp2->pfn$l_flink;
  do {
    n++;
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_flink;
  } while (tmp2!=pfn$al_tail);
  n--;
  tmp=pfn$al_tail;
  tmp2=tmp;
  tmp2=tmp2->pfn$l_blink;
  do {
    m++;
    if (tmp2!=(tmp2->pfn$l_flink->pfn$l_blink)) goto mypanic;
    if (tmp2!=(tmp2->pfn$l_blink->pfn$l_flink)) goto mypanic;
    tmp2=tmp2->pfn$l_blink;
  } while (tmp2!=pfn$al_head);
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

