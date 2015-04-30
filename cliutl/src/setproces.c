// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <starlet.h>
#include <descrip.h>
#include <stdio.h>
#include <stdlib.h>
#include <ssdef.h>
#include <cli$routines.h>

/* Author: Roar Thron�s */

/* Don't know anywhere else to put this */

int set_process(int argc, char**argv)
{
    $DESCRIPTOR(p, "p1");
    $DESCRIPTOR(id, "identification");
    $DESCRIPTOR(na, "name");
    $DESCRIPTOR(pr, "priority");

    char idstr[80];
    struct dsc$descriptor idval;
    idval.dsc$a_pointer = idstr;
    idval.dsc$w_length = 80;

    char nastr[80];
    struct dsc$descriptor naval;
    naval.dsc$a_pointer = nastr;
    naval.dsc$w_length = 80;

    char prstr[80];
    struct dsc$descriptor prval;
    prval.dsc$a_pointer = prstr;
    prval.dsc$w_length = 80;

    int sts, idsts, nasts, prsts;
    int retlen;

    sts = cli$present(&p);
#if 0
    if (0)
        sts = cli$get_value(&p, &o, &retlen);
#endif

    idsts = cli$present(&id);

    int pid = 0;

    if (idsts & 1)
    {
        idsts = cli$get_value(&id, &idval, &retlen);
        pid = strtol(id.dsc$a_pointer, 0, 16);
    }

    nasts = cli$present(&na);

    if (nasts & 1)
    {
        nasts = cli$get_value(&na, &naval, &retlen);
        struct dsc$descriptor d;
        d.dsc$a_pointer = naval.dsc$a_pointer;
        d.dsc$w_length = retlen;
        sys$setprn(&d);
    }

    prsts = cli$present(&pr);

    if (prsts & 1)
    {
        prsts = cli$get_value(&pr, &prval, &retlen);
        int pri = atoi(prval.dsc$a_pointer);
        int pidaddr = 0;
        if (pid)
            pidaddr = &pid;
        sys$setpri(pid, 0, pri, 0, 0, 0);
    }

    return SS$_NORMAL;
}

