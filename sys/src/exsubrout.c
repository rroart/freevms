#include "../../freevms/sys/src/system_data_cells.h"

void exe$instimq(struct _tqe * t) {
  struct _tqe *tmp=exe$gl_tqfl;
  while (t->tqe$q_time>tmp->tqe$q_time)
    tmp=tmp->tqe$l_tqfl;
  insque(t,tmp);
}

void exe$rmvtimq(struct _tqe * t) {

}
