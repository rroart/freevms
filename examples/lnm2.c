#include <stdio.h>
#include <lnmdef.h>
#include <ssdef.h>
#include <descrip.h>
#include <misc.h>

main()
{
    struct item_list_3 i[2];
    int c;
    int status;
    char resstring[LNM$C_NAMLENGTH];

    $DESCRIPTOR(mynam,"BIBI");
    $DESCRIPTOR(mynam2,"BOBO");
    $DESCRIPTOR(mytabnam3,"MYTEST3");
    $DESCRIPTOR(mytabnam2,"MYTEST2");
    $DESCRIPTOR(mytabnam,"MYTEST");
    $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");

    i[0].item_code=LNM$_STRING;
    i[0].buflen=LNM$C_NAMLENGTH;
    i[0].bufaddr=resstring;

    bzero(&i[1],sizeof(struct item_list_3));
    status=sys$trnlnm(0,&mytabnam2,&mynam,0,i);

    printf("log %x %x\n",i[0].bufaddr,i[0].buflen);
    printf("log %s\n",i[0].bufaddr);
}
