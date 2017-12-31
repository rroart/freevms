#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <time.h>

main()
{
    $DESCRIPTOR(NAME1,"numberone");
    $DESCRIPTOR(NAME2,"numbertwo");

    sys$ascefc(66,&NAME1);
    sys$ascefc(99,&NAME2);

    printf("before waitfr %x\n",time(0));

    sys$waitfr( 80 );

    printf("after waitfr %x\n",time(0));

}
