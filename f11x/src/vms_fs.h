#ifndef _VMS_FS_H
#define _VMS_FS_H

#include <mytypes.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <rmsdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <fh2def.h>
#include <fi2def.h>
#include <hm2def.h>
#include <dirdef.h>
#include <vmstime.h>

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
