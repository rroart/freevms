#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>

main()
{
    $DESCRIPTOR(hello,"HELLO\n");
    $DESCRIPTOR(world,"WORLD\n");
    lib$put_output(&hello);
    lib$put_output(&world);
}
