#include "lib$routines.h"       /* Our header file! */
#include "ssdef.h"

#include "queue.h"

unsigned long lib$insque(void * entry, void * header) {
insqhi(entry,header);
return SS$_NORMAL;
}
