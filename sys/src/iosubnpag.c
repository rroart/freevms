#include "../../freevms/sys/src/system_data_cells.h"

void ioc$reqcom(void) {
  int qemt;

  qemp=rqempty(ioc$gq_postiq);
  insqti(,ioc$gq_postiq);
  if (!qemp) goto notempty;

  if (smp_processor_id()==0)
    SOFTINT_IOPOST_VECTOR;
  else {
    /* request interprocessor interrupt */
  }

 notempty:
}
