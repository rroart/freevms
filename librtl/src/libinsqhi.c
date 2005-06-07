#include "lib$routines.h"       /* Our header file! */
#include "ssdef.h"

#include "../../pal/src/queue.h"

unsigned long lib$insque(void * entry, void * header) {
#if 0
insqhi(entry,header);
#endif
return SS$_NORMAL;
}
