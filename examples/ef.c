#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <time.h>

signed long long step1;

void my1(int i)
{
    printf("here1\n");
    sys$setef(63);
}

main()
{
    int flag ;

    sys$readef(31,&flag);
    printf("flag %x\n",flag);
    sys$readef(63,&flag);
    printf("flag %x\n",flag);

    sys$setef(51);
    sys$setef(20);

    sys$readef(31,&flag);
    printf("flag %x\n",flag);
    sys$readef(51,&flag);
    printf("flag %x\n",flag);

    sys$clref(51);

    sys$readef(31,&flag);
    printf("flag %x\n",flag);
    sys$readef(63,&flag);
    printf("flag %x\n",flag);


}
