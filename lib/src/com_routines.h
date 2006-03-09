#ifndef com_routines_h
#define com_routines_h

#include <far_pointers.h>

void  com_std$delattnast (struct _acb **acb_lh, struct _ucb *ucb);
void  com_std$delattnastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid);
void  com_std$delctrlast (struct _acb **acb_lh, struct _ucb *ucb, int matchchar, int32 *inclchar_p);
void  com_std$delctrlastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid, int matchchar, int32 *inclchar_p);
void  com_std$drvdealmem (void *ptr);
int   com_std$flushattns (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh);
int   com_std$flushctrls (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh, int32 *mask_p);
void  com_std$post (struct _irp *irp, struct _ucb *ucb);
void  com_std$post_nocnt (struct _irp *irp);
int   com_std$setattnast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh);
int   com_std$setctrlast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh, int mask, struct _tast **tast_p);

#endif
