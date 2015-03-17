// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h>
#include <string.h>

#include <ssdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include <starlet.h>
#include <misc.h>
#include <cli$routines.h>

// this will be a bit limited because we can not use the kernel routines for
// now, but instead the system services

show_logical(int argc, char**argv)
{
    int sts;
    struct item_list_3 itm[2];
    struct dsc$descriptor mytabnam, mynam;
    char resstring[LNM$C_NAMLENGTH];
    memset(resstring, 0, LNM$C_NAMLENGTH);

    char * default_table = "LNM$PROCESS_TABLE";
    char * table;

    $DESCRIPTOR(p, "p1");
    $DESCRIPTOR(d, "table");

    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset (c, 0, 80);

    char e[80];
    struct dsc$descriptor o2;
    o2.dsc$a_pointer=e;
    o2.dsc$w_length=80;
    memset (e, 0, 80);

    int retlen;

    sts = cli$present(&p);
    if ((sts&1)==0)
        return sts;

    sts = cli$present(&d);

    if (sts&1)
    {
        sts = cli$get_value(&d, &o, &retlen);
        table=c;
    }
    else
    {
        table=default_table;
    }

    sts = cli$get_value(&p, &o2, &retlen);

    mynam.dsc$w_length=strlen(e);
    mynam.dsc$a_pointer=e;
    mytabnam.dsc$w_length=strlen(table);
    mytabnam.dsc$a_pointer=table;

    memset(&itm[0],0,sizeof(struct item_list_3));
    itm[0].item_code=LNM$_STRING;
    itm[0].buflen=LNM$C_NAMLENGTH;
    itm[0].bufaddr=resstring;
    memset(&itm[1],0,sizeof(struct item_list_3));

    sts = sys$trnlnm(0,&mytabnam,&mynam,0,itm);

    printf("   \"%s\" = \"%s\"\n",e,resstring);

    return sts;
}
