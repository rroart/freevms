#include <stdio.h> 
#include <ssdef.h> 
#include <iodef.h>
#include <misc.h>
#include <descrip.h> 

int done=0;

int myast(int some) {
printf("myast\n");
done=1;
}

main(){
struct _iosb myiosb;
int status;
unsigned short int chan;
char mybuf[512];
int efn=0;
$DESCRIPTOR(null,"NLA0");

status=sys$assign(&null,&chan,0,0,0);

printf("status %x chan %x \n", status,chan);

status=sys$qio(0,chan,IO$_READVBLK,0,myast,0,mybuf,512,0,0,0,0);

sleep(2);

printf("status %x done %x \n", status,done);

status=sys$qiow(efn,chan,IO$_READVBLK,0,myast,0,mybuf,512,0,0,0,0);
status=sys$qiow(efn,chan,IO$_READVBLK,0,myast,0,mybuf,512,0,0,0,0);

printf("status %x done %x \n", status,done);

}

