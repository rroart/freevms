#include <stdio.h> 
#include <time.h>
#include <ssdef.h> 
#include <iodef.h>
#include <misc.h>
#include <descrip.h> 

#define MBXBUFSIZ 1280
#define MBXBUFQUO 3840



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

 void *p1, mbxast();
 char mbuffer[MBXBUFSIZ], prmflg=0;
 unsigned short mbxchan, mbxiosb;
 unsigned int status, outlen;
 unsigned int mbuflen=MBXBUFSIZ, bufquo=MBXBUFQUO, promsk=0;
 $DESCRIPTOR(mblognam,"GROUP100_MAILBOX");

 status = sys$crembx(prmflg,&mbxchan,mbuflen,bufquo,promsk,0,&mblognam,0);

status=sys$assign(&null,&chan,0,0,0);

printf("status %x chan %x \n", status,chan);

printf("before qio %x\n",time(0));
status=sys$qio(0,chan,IO$_READVBLK,0,myast,0,mybuf,512,0,0,0,0);
printf("after qio %x\n",time(0));

sleep(2);
sleep(2);
sleep(2);
sleep(2);

printf("status %x done %x \n", status,done);

printf("before qiow %x\n",time(0));
status=sys$qiow(efn,chan,IO$_READVBLK,0,myast,0,mybuf,512,0,0,0,0);
printf("after qiow %x\n",time(0));

printf("status %x done %x \n", status,done);

}

