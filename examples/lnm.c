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

  sys$crelnt(0,0,0,0,0,&mytabnam,&mypartab,0);
  sys$crelnt(0,0,0,0,0,&mytabnam2,&mypartab,0);
  sys$crelnt(0,0,0,0,0,&mytabnam3,&mytabnam2,0);

  i[0].item_code=1;
  i[0].buflen=5;
  i[0].bufaddr="mylog";
  bzero(&i[1],sizeof(struct item_list_3));
  status=sys$crelnm(0,&mytabnam2,&mynam,0,i);

  i[0].item_code=1;
  i[0].buflen=6;
  i[0].bufaddr="mylog3";
  bzero(&i[1],sizeof(struct item_list_3));
  status=sys$crelnm(0,&mytabnam2,&mynam2,0,i);

  status=sys$trnlnm(0,&mytabnam2,&mynam,0,i);

  printf("log %x\n",i[0].bufaddr);
  printf("log %s\n",i[0].bufaddr);
}
