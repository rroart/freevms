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
$DESCRIPTOR(onesec,"0 00:00:01.00");
 
sys$bintim(&onesec,&step1);
step2=4*step1;

if (step1<0) printf("less then 0\n");

sys$setimr(0,&step1,my1,0,0);
sys$setimr(0,&step2,my2,0,0);

/* not quite compatible yet, should have been one sleep(30) */

sleep(30);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
sleep(5);
}

