#include "../../freevms/sys/src/system_data_cells.h"
#include <linux/mm.h>
#include "../../freevms/lib/src/pfndef.h"

signed long mmg$allocpfn(void) {
  struct _pfn * p;
  if (!pfn$al_head)
    return -1;
  p=pfn$al_head;
  pfn$al_head=((struct _pfn *) pfn$al_head)->pfn$l_flink;
  ((struct _pfn *) pfn$al_head + 4)->pfn$l_flink=0; // gcc bug?
  //  p->pfn$l_refcnt=0;
  if (p->pfn$l_refcnt)
    panic("refcnt\n");
  return (((unsigned long)p-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$allocontig(unsigned long num) {
  struct _pfn * p, * first=pfn$al_head, * next;
  unsigned long done=0, c;
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
    pfn$al_head=p->pfn$l_flink;
    ((struct _pfn *) pfn$al_head+4)->pfn$l_flink=0; // gcc bug?
  } else {
    ((struct _pfn *)((first->pfn$l_flink)+4))->pfn$l_flink=p->pfn$l_flink;// gcc bug?
    (((struct _pfn *)((struct _pfn *)((first->pfn$l_flink)+4))->pfn$l_flink)->pfn$l_blink)=first->pfn$l_blink;
  }

  if (p->pfn$l_refcnt) // do more 
    panic("refcnt\n");
  return (((unsigned long)first-(unsigned long)mem_map)/sizeof(struct _pfn));
}

signed long mmg$dallocpfn(unsigned long pfn) {
  mem_map[pfn].pfn$l_flink=0;
  mem_map[pfn].pfn$l_blink=pfn$al_tail;
  ((struct _pfn *)pfn$al_tail)->pfn$l_flink=&mem_map[pfn];
  pfn$al_tail=&mem_map[pfn];
}
