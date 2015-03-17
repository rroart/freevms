#include "../../lib/src/mytypes.h"
#include "../../starlet/src/uicdef.h"
#include "../../lib/src/hm2def.h"
#include "../../lib/src/fi2def.h"
#include "../../lib/src/fm2def.h"
#include "../../lib/src/fatdef.h"
#include "../../starlet/src/fiddef.h"
#include "../../lib/src/dirdef.h"
#include "../../lib/src/scbdef.h"
#include "../../lib/src/fh2def.h"

#ifdef FREEVMS_BIG_ENDIAN
#define VMSLONG(l) ((l & 0xff) << 24 | (l & 0xff00) << 8 | (l & 0xff0000) >> 8 | l >> 24)
#define VMSWORD(w) ((w & 0xff) << 8 | w >> 8)
#define VMSSWAP(l) ((l & 0xff0000) << 8 | (l & 0xff000000) >> 8 |(l & 0xff) << 8 | (l & 0xff00) >> 8)#else
#define VMSLONG(l) l
#define VMSWORD(w) w
#define VMSSWAP(l) ((l & 0xffff) << 16 | l >> 16)
#endif

struct FM2_C_FORMAT2
{
    unsigned short int fm2$w_word0;
    unsigned short int fm2$v_count2;
};
