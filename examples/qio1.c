#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 

signed long long step1;
signed long long step2;

void my1(int i) {
printf("here1\n");
sys$setimr(0,&step1,my1,0,0);
}

void my2(int i) {
printf("here2\n");
sys$setimr(0,&step2,my2,0,0);
}

main(){
int status,chan;
$DESCRIPTOR(null,"NLA0");

status=sys$assign(&null,&chan,0,0,0);

printf("status %x chan %x \n", status,chan);

}

