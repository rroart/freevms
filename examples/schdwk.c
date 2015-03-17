#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <time.h>

signed long long now;
signed long long step1;

main()
{
    $DESCRIPTOR(secs,"0 00:00:20.00");

    sys$gettim(&now);
    sys$bintim(&secs,&step1);
    if (now<0) printf("now less than 0\n");
    if (step1<0) printf("step1 less than 0\n");
    now=now-step1;

    sys$schdwk(0,0,&now,&step1);

    printf("before hiber %x\n",time(0));

    sys$hiber();

    sys$hiber();

    sys$hiber();

    printf("after hiber %x\n",time(0));

}
