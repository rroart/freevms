#ifndef acp_routines_h
#define acp_routines_h

#include "../../../freevms/lib/src/ccbdef.h"
#include "../../../freevms/lib/src/irpdef.h"
#include "../../../freevms/lib/src/pcbdef.h"
#include "../../../freevms/lib/src/ucbdef.h"

int   acp_std$access (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$accessnet (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$deaccess (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$fastio_block (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$modify (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$mount (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$readblk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   acp_std$writeblk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);

#endif
