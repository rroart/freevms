#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <lnmdef.h>
#include <starlet.h>

// this will be a bit limited because we can not use the kernel routines for
// now, but instead the system services

show_logical(int argc, char**argv){
  int sts;
  struct item_list_3 itm[2];
  struct dsc$descriptor mytabnam, mynam;
  char resstring[LNM$C_NAMLENGTH];

  if (argc>1 && 0==strncmp(argv[0],"/table",strlen(argv[0]))) {
  } else {
    return 0;
  }

  mynam.dsc$w_length=strlen(argv[2]);
  mynam.dsc$a_pointer=argv[2];
  mytabnam.dsc$w_length=strlen(argv[1]);
  mytabnam.dsc$a_pointer=argv[1];

  itm[0].item_code=LNM$C_STRING;
  itm[0].buflen=LNM$C_NAMLENGTH;
  itm[0].bufaddr=resstring;
  bzero(&itm[1],sizeof(struct item_list_3));

  sts = sys$trnlnm(0,&mytabnam,&mynam,0,itm);

  printf("   \"%s\" = \"%s\"\n",argv[2],resstring);

  return sts;
}
