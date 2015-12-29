#ifndef _LINUX_VT_H
#define _LINUX_VT_H

/* 0x56 is 'V', to avoid collision with termios and kd */

#define VT_OPENQRY  0x5600  /* find available vt */

struct vt_mode
{
    char mode;      /* vt mode */
    char waitv;     /* if set, hang on writes if not active */
    short relsig;       /* signal to raise on release req */
    short acqsig;       /* signal to raise on acquisition */
    short frsig;        /* unused (set to 0) */
};
#define VT_GETMODE  0x5601  /* get mode of active vt */
#define VT_SETMODE  0x5602  /* set mode of active vt */
#define     VT_AUTO     0x00    /* auto vt switching */
#define     VT_PROCESS  0x01    /* process controls switching */
#define     VT_ACKACQ   0x02    /* acknowledge switch */

#endif /* _LINUX_VT_H */
