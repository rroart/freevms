#include<stdio.h>
void fn(int i)
{
    printf("hi %x %x %x\n",i,getuid(),getgid());
}
main()
{
    int i;
    sys$setast(0);
    sys$dclast(&fn,3,0);
    sys$dclast(&fn,1,0);
    sys$dclast(&fn,0,0);
    sys$dclast(&fn,2,0);
    printf("here\n");
    sys$setast(1);

    sleep(5);
    printf("end %x %x\n",getuid(),getgid());

}

