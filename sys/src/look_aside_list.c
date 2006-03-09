// $Id$
// $Locker$

// Author. Roar Thronæs.

struct _myhead {
  struct _myhead * hd$l_flink;
  long hd$l_seq;
};
#include<linux/kernel.h>
void *exe$lal_remove_first(void *listhead) {
  struct _myhead * h=listhead;
  struct _myhead * ret = h->hd$l_flink;
  h->hd$l_seq--;
  if (ret) h->hd$l_flink=ret->hd$l_flink;
  return ret;
}

void exe$lal_insert_first(void *packet,void *listhead) {
  struct _myhead * h=listhead;
  struct _myhead * p=packet;
  h->hd$l_seq++;
  p->hd$l_flink=h->hd$l_flink;
  h->hd$l_flink=p;
}

