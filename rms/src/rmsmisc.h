// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thron�s.

#ifndef RMSMISC_H
#define RMSMISC_H

#define IFI_MAX 64
extern struct _wccfile *ifi_table[];
extern struct _ifbdef *ifb_table[];

// RMS reserves event flags 27-30, and maybe 31
// have seen something about flag 29 being used
// pick 30 for xqp; set in access.c
#define RMS_EF 29

#endif /* RMSMISC_H */
