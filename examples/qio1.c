#include <stdio.h> 
#include <ssdef.h> 
#include <iodef.h>
#include <misc.h>
#include <descrip.h> 

main(){
struct _iosb myiosb;
int status;
unsigned short int chan;
char mybuf[512];
$DESCRIPTOR(null,"NLA0");

status=sys$assign(&null,&chan,0,0,0);

printf("status %x chan %x \n", status,chan);

status=sys$qio(0,chan,IO$_READVBLK,0,0,0,mybuf,512,0,0,0,0);

sleep(2);

printf("status %x chan %x \n", status,chan);

}

