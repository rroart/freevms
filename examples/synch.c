#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <time.h>

signed long long step1;

void my1(int i) {
printf("here1\n");
sys$setef(50);
}

main(){
$DESCRIPTOR(tensec,"0 00:00:10.00");
 
sys$bintim(&tensec,&step1);

sys$setimr(50,&step1,my1,0,0);

printf("before something%x\n",time(0));

sys$synch(50, 0 ); 

printf("after something%x\n",time(0));

}
