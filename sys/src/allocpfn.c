// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <linux/mm.h>
#include <pfndef.h>

struct _mypfn {
  struct myq * pfn$l_flink;
  struct myq * pfn$l_blink;
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
  if (p->pfn$l_refcnt)
    panic("refcnt\n");
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

  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
  sch$gl_freecnt-=num;
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$dallocpfn(unsigned long pfn) {
  struct _mypfn * m=&mem_map[pfn];
  //  mem_map[pfn].pfn$l_flink=0;
  //mem_map[pfn].pfn$l_blink=pfn$al_tail;
  //return;
  m->pfn$l_flink=0;
  m->pfn$l_blink=pfn$al_tail;
  if (mem_map[pfn].pfn$l_flink) panic("dalloc\n");
  ((struct _pfn *)pfn$al_tail)->pfn$l_flink=&mem_map[pfn];
  pfn$al_tail=&mem_map[pfn];
  sch$gl_freecnt++;
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

  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
  sch$gl_freecnt-=num;
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
}

