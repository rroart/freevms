// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <iodef.h>
#include <starlet.h>
#include <smg$routines.h>
#include "smg.h"

extern int smgunix;

#undef smg$set_term_characteristics
int smg$set_term_characteristics (long * pasteboard_id, int * on_characteristics1, int * on_characteristics2, int * off_characteristics1, int * off_characteristics2, int * old_characteristics1, int * old_characteristics2, int * on_characteristics3, int * off_characteristics3, int * old_characteristics3)
{
    short chan;
    smg$$get_pasteboard_chan (*pasteboard_id, &chan);
    int bigger = on_characteristics3 || off_characteristics3 || old_characteristics3;
    int write = on_characteristics1 || off_characteristics1 || on_characteristics2 || off_characteristics2 || on_characteristics3 || off_characteristics2;
    int buf[3];
    int buflen = 8;
    if (bigger)
        buflen = 12;
    long long io_stats;
    int status;
    if (smgunix)
        return SS$_NORMAL;
    status = sys$qiow (0, chan, IO$_SENSEMODE, &io_stats, 0, 0, buf, buflen, 0, 0, 0, 0);
    if (old_characteristics1)
        *old_characteristics1 = buf[0];
    if (old_characteristics2)
        *old_characteristics2 = buf[1];
    if (old_characteristics3)
        *old_characteristics2 = buf[2];
    if (!write)
        return SS$_NORMAL;
    if (on_characteristics1)
        buf[0] |= *on_characteristics1;
    if (on_characteristics2)
        buf[1] |= *on_characteristics2;
    if (on_characteristics3)
        buf[2] |= *on_characteristics3;
    if (off_characteristics1)
        buf[0] &= ~(*off_characteristics1);
    if (off_characteristics2)
        buf[1] &= ~(*off_characteristics2);
    if (off_characteristics3)
        buf[2] &= ~(*off_characteristics3);
    status = sys$qiow (0, chan, IO$_SETMODE, &io_stats, 0, 0, buf, buflen, 0, 0, 0, 0);
    return SS$_NORMAL;
}

