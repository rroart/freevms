#include <fabdef.h>
#include <namdef.h>
#include <rabdef.h>
#include <xaballdef.h>
#include <xabcxfdef.h>
#include <xabcxrdef.h>
#include <xabdatdef.h>
#include <xabdef.h>
#include <xabfhcdef.h>
#include <xabitmdef.h>
#include <xabkeydef.h>
#include <xabprodef.h>
#include <xabrdtdef.h>
#include <xabrudef.h>
#include <xabsumdef.h>
#include <xabtrmdef.h>

// I think there is enough 0s

struct _fabdef cc$rms_fab={FAB$C_BID,FAB$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _namdef cc$rms_nam={NAM$C_BID,NAM$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _namldef cc$rms_naml={NAML$C_BID,NAML$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _rabdef cc$rms_rab={RAB$C_BID,RAB$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _xaballdef cc$rms_xaball={XAB$C_ALL,XAB$C_ALLLEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _xabdatdef cc$rms_xabdat={XAB$C_DAT,XAB$C_DATLEN,0,0,0,0,0,0,0,0,0,0};
struct _xabfhcdef cc$rms_xabfhc={XAB$C_FHC,XAB$C_FHCLEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _xabkeydef cc$rms_xabkey={XAB$C_KEY,XAB$C_KEYLEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _xabprodef1 cc$rms_xabpro={XAB$C_PRO,XAB$C_PROLEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _xabrdtdef cc$rms_xabrdt={XAB$C_RDT,XAB$C_RDTLEN,0,0,0,0,0};
struct _xabsumdef cc$rms_xabsum={XAB$C_SUM,XAB$C_SUMLEN,0,0,0,0,0};
struct _xabtrmdef cc$rms_xabtrm={XAB$C_TRM,XAB$C_TRMLEN,0,0,0,0,0,0,0,0,0,0};

#if 0
struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
struct _xabdatdef cc$rms_xabdat = {XAB$C_DAT,0,
				   0, 0, 0, 0,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO};
struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
struct _xabprodef1 cc$rms_xabpro = {XAB$C_PRO,0,0,0};
struct _rabdef cc$rms_rab = {NULL,NULL,NULL,NULL,0,0,0,{0,0,0}};
struct _xabkeydef cc$rms_xabkey = {XAB$C_KEY,XAB$C_KEYLEN};
#endif
