#ifndef _VMS_FS_H
#define _VMS_FS_H

#include "../../freevms/lib/src/mytypes.h"
#include "../../freevms/lib/src/fatdef.h"
#include "../../freevms/lib/src/vcbdef.h"
#include "../../freevms/librtl/src/descrip.h"
#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/starlet/src/uicdef.h"
#include "../../freevms/starlet/src/namdef.h"
#include "../../freevms/starlet/src/fabdef.h"
#include "../../freevms/starlet/src/rabdef.h"
#include "../../freevms/starlet/src/fibdef.h"
#include "../../freevms/starlet/src/fiddef.h"
#include "../../freevms/starlet/src/rmsdef.h"
#include "../../freevms/starlet/src/xabdef.h"
#include "../../freevms/starlet/src/xabdatdef.h"
#include "../../freevms/starlet/src/xabfhcdef.h"
#include "../../freevms/starlet/src/xabprodef.h"
#include "../../freevms/lib/src/fh2def.h"
#include "../../freevms/lib/src/fi2def.h"
#include "../../freevms/lib/src/hm2def.h"
#include "../../freevms/lib/src/dirdef.h"
#include "../../freevms/lib/src/vmstime.h"

/*
 * adapted by Chris Nott and Karl Maftoum and Amber East
 * from getvms.c by Carl Lydick (I think)
 */


#define VMS_BLOCKSIZE 512
#define VMS_BLOCKSIZE_BITS 9
#define VMS_MAXNAMLEN 20
#define VMS_SUPER_MAGIC 0x49554b44
#define VMS_ROOT_INO 4

#endif
