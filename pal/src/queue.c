// $Id$
// $Locker$

// Author. Roar Thronæs.

/* Not implemented: atomicity */
/* Not implemented: long and quadword alignments */

/* Hmm C is too strongly typed */

/* These are VARM based. 32 bits */

/* Author: Roar Thronæs */

#include<queue.h>

void insque(void * entry, void * pred) {
  if (entry==pred) panic("same\n");
  if (entry==*(long *)pred) panic("same\n");
  if (entry==*(long *)(((long)pred)+4)) panic("same\n");
  int flag=mycli();
  //mycheckaddr();
  *(void **)entry=*(void **)pred;
  *(void **)(entry+4)=pred;
  *(void **)((*(void **)pred)+4)=entry;
  *(void **)pred=entry;
  //mycheckaddr();
  mysti(flag);
}

unsigned long remque(void * entry, void * addr) {
  int flag=mycli();
  //mycheckaddr();
  *(void **)(*(void **)(entry+4))=*(void **)entry;
  *(void **)((*(void **)entry)+4)=*(void **)(entry+4);
  addr=entry;
  //mycheckaddr();
  mysti(flag);
  return (unsigned long) addr;
}

/* These are AARM based. Still 32 bits */

void insqhi(void * entry, void * header) {
  int flag=mycli();
  signed long tmp1=*(signed long *)header;
  signed long tmp2=(signed long)(header-entry);
  /* cli() or a semaphore/mutex? */
  *(signed long *)entry=tmp1+tmp2;
  *(signed long *)(entry+4)=tmp2;
  *(signed long *)(header+(signed long)(tmp1)+4)=-tmp1-tmp2;
  *(signed long *)header=-tmp2;
  /* sti() */
  mysti(flag);
}

void insqti (void * entry, void * header) {
  int flag=mycli();
  signed long tmp1=*(signed long *)header;
  signed long tmp2=*(signed long *)(header+4);
  signed long tmp3=(signed long)(header-entry);
  /* cli() or a semaphore/mutex? */
  *(signed long *)entry=tmp3;
  *(signed long *)(entry+4)=tmp2+tmp3;
  if (tmp2)
    *(signed long *)(header+(signed long)(tmp2))=-tmp2-tmp3;
  else
    tmp1=-tmp3-tmp2;
  *(signed long *)(header+4)=-tmp3;
  *(signed long *)header=tmp1;
  /* sti() */
  mysti(flag);
}

unsigned long remqhi(void * header, void * addr) {
  int flag=mycli();
  signed long tmp1=*(signed long *)header;
  signed long tmp2=(signed long)header+tmp1;
  signed long tmp3;
  /* cli() or a semaphore/mutex? */
  if (!tmp1)
    tmp3=(signed long) header;
  else
    tmp3=tmp2+*(signed long *)tmp2;
  *((signed long *)(tmp3+4))=(signed long)header-tmp3;
  *(signed long *)header=tmp3-(signed long)header;
  addr=(signed long *)tmp2;
  mysti(flag);
  return (unsigned long) addr;
  /* sti() */
}

unsigned long remqti(void * header, void * myaddr) {
  int flag=mycli();
  /*signed long tmp1=*header;*/
  signed long tmp5=*(signed long *)(header+4);
  signed long addr=(signed long)header+tmp5;
  signed long tmp2=addr+*((signed long *)(addr+4));
  /* cli() or a semaphore/mutex? */
  *(signed long *)(header+4)=tmp2-(signed long)header;
  if (tmp2 == (signed long) header)
    *(signed long *)header=0;
  else
    *(signed long *)tmp2=(signed long)header-tmp2;
  myaddr=(signed long *) addr;
  mysti(flag);
  return (unsigned long) myaddr;
  /* sti() */
}

struct myq {
  unsigned long flink;
  unsigned long blink;
};

inline int aqempty(void * q) {
  struct myq *m=(struct myq *) q;
  return (q==m->flink);
}

inline int rqempty(void * q) {
  struct myq *m=(struct myq *) q;
  return (m->flink==0 && m->blink==0);
}

void boot_insqti (void * entry, void * header) {
  signed long tmp1=*(signed long *)header;
  signed long tmp2=*(signed long *)(header+4);
  signed long tmp3=(signed long)(header-entry);
  /* cli() or a semaphore/mutex? */
  *(signed long *)entry=tmp3;
  *(signed long *)(entry+4)=tmp2+tmp3;
  if (tmp2)
    *(signed long *)(header+(signed long)(tmp2))=-tmp2-tmp3;
  else
    tmp1=-tmp3-tmp2;
  *(signed long *)(header+4)=-tmp3;
  *(signed long *)header=tmp1;
}

//__PAL_INSQUEL_D

//__PAL_REMQUEL_D
