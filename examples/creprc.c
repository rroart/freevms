#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <syidef.h>
#include <misc.h>

main(int argc,void ** argv){
  $DESCRIPTOR(imgname,"/bin/ls");
  $DESCRIPTOR(prcname,"ls");
  $DESCRIPTOR(imgname2,"/vms$common/syshlp/examples/hi.exe");
  $DESCRIPTOR(imgname3,"[vms$common.syshlp.examples]hi.exe");
  $DESCRIPTOR(prcname3,"hi");
  $DESCRIPTOR(imgname4,"/vms$common/sysexe/dcl");
  $DESCRIPTOR(prcname4,"dcl2");
  int pid;
  struct item_list_3 lst[14], syilst[2];

  if (argc>1)
    goto dcl;

  int RC = sys$creprc(&pid, &imgname,0,0,0,
		      0, 0, &prcname, 24, 0,
		      0, 0, 0, 0, 0);

  printf("creprc pid %x returned %x\n",pid,RC);

  int retlenaddr;
  int mem=0;

  syilst[0].buflen=4;
  syilst[0].item_code=SYI$_LASTFLD;
  syilst[0].bufaddr=&mem;
  syilst[0].retlenaddr=&retlenaddr;
  syilst[1].buflen=0;
  syilst[1].item_code=0;

  int sts=sys$getsyi(0,0,0,syilst,0,0,0);

  if (!mem) {
    printf("No VMS MM\n");
    goto out;
  }

  int file = fopen(imgname2.dsc$a_pointer,"r");
  if (file==0) {
    printf("%s not found\n",imgname2.dsc$a_pointer);
    goto out;
  }

  RC = sys$creprc(&pid, &imgname3,0,0,0,
		      0, 0, &prcname3, 24, 0,
		      0, 0, 0, 0, 0);

  printf("creprc pid %x returned %x\n",pid,RC);

 out:
  sleep(60);

  return SS$_NORMAL;

 dcl:

  {
    $DESCRIPTOR(opa,"opa0");
    int RC = sys$creprc(&pid, &imgname4,&opa,&opa,&opa,
			0, 0, &prcname4, 24, 0,
			0, 0, 0, 0, 0);

  sleep(60);

  return SS$_NORMAL;
  }

}
