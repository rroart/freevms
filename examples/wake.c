#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 

signed long long step1;

void my1(int i) {
sys$wake(0,0);
}

main(){
$DESCRIPTOR(secs,"0 00:00:10.00");
 
sys$bintim(&secs,&step1);

if (step1<0) printf("less then 0\n");

sys$setimr(0,&step1,my1,0,0);

printf("before hiber\n");

sys$hiber();

printf("after wake\n");

}
