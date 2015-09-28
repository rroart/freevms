#include "lib$routines.h"       /* Our header file! */
#include "ssdef.h"

#include "../../pal/src/queue.h"

int lib$insqhi(void * entry, void * header)
{
#if 0
    insqhi(entry,header);
#endif
    return SS$_NORMAL;
}

int lib$insqti(void * entry, void * header)
{
    return SS$_NORMAL;
}
