#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <time.h>

signed long long step1;

void my1(int i) {
printf("here1\n");
sys$setef(126);
}

main(){
int flag ;

$DESCRIPTOR(NAME1,"numberone");
$DESCRIPTOR(NAME2,"numbertwo");

sys$ascefc(66,&NAME1);
sys$ascefc(99,&NAME2);

sys$readef(93,&flag);
printf("flag %x\n",flag);
sys$readef(126,&flag);
printf("flag %x\n",flag);

sys$setef(51);
sys$setef(80);

sys$readef(93,&flag);
printf("flag %x\n",flag);
sys$readef(111,&flag);
printf("flag %x\n",flag);

sys$clref(111);

sys$readef(93,&flag);
printf("flag %x\n",flag);
sys$readef(126,&flag);
printf("flag %x\n",flag);


}
