#include <ssdef.h>
#include <descrip.h>

main()
{
    int flag ;

    sys$setef(51);

    sys$readef(51,&flag);

}
