// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <dvidef.h>
#include <starlet.h>
#include <misc.h>

show_device(){
int i;
struct item_list_3 lst[4];
char devname[15];
char devlen;
int dvistatus;

lst[0].buflen=15;
lst[0].item_code=DVI$_DEVNAM;
lst[0].bufaddr=devname;
lst[0].retlenaddr=&devlen;
lst[1].buflen=0;
lst[1].item_code=0;
do {
dvistatus=sys$getdvi(0,0,0,lst,0,0,0,0);
printf("%s\n",devname);
} while (dvistatus == SS$_NORMAL);
//} while (jpistatus != SS$_NOMOREPROC);
//printf("dvistatus %x\n",dvistatus);
}
