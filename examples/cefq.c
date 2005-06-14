#include <ssdef.h> 
#include <descrip.h> 

signed long long step1=42;

static signed long long step2;

static long tata[2048];

main(){
int flag ;
step2=step1;
flag=step2;
flag=tata;

$DESCRIPTOR(NAME1,"numberone");
$DESCRIPTOR(NAME2,"numbertwo");

sys$ascefc(66,&NAME1);
sys$ascefc(99,&NAME2);

sys$readef(93,&flag);
sys$readef(126,&flag);

sys$setef(51);
sys$setef(80);

sys$readef(93,&flag);
sys$readef(111,&flag);

sys$clref(111);

sys$readef(93,&flag);
sys$readef(126,&flag);

}
