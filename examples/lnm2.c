#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <misc.h>

main(){
  struct item_list_3 i[2];
  int c;
  int status;

  $DESCRIPTOR(mynam,"BIBI");
  $DESCRIPTOR(mynam2,"BOBO");
  $DESCRIPTOR(mytabnam3,"MYTEST3");
  $DESCRIPTOR(mytabnam2,"MYTEST2");
  $DESCRIPTOR(mytabnam,"MYTEST");
  $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");

  i[0].item_code=1;
  bzero(&i[1],sizeof(struct item_list_3));
  status=sys$trnlnm(0,&mytabnam2,&mynam,0,i);

  printf("log %x %x\n",i[0].bufaddr,i[0].buflen);
  printf("log %s\n",i[0].bufaddr);
}
