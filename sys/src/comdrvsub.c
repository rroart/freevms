#include<system_data_cells.h>
#include<linux/kernel.h>
#include <asm/hw_irq.h>

void com$post(struct _irp * i, struct _ucb * u) {
  insqti(i,&ioc$gq_postiq);
  SOFTINT_IOPOST_VECTOR;
}
