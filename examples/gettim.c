#include<stdio.h>
#include <descrip.h>

signed long long timevalue;

main()
{

    unsigned int status;
    char timestr[23];
    $DESCRIPTOR(atimenow, timestr);

    status = sys$gettim(&timevalue);
    status = sys$asctim(0,&atimenow,&timevalue,0);

    printf("str %s\n",atimenow.dsc$a_pointer);
}
