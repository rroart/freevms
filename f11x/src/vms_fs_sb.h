#ifndef _VMS_FS_SB_H
#define _VMS_FS_SB_H

struct vms_home_info {
	ushort ibvb;
	ushort ibsz;
	struct _fh2 indexh;
	struct _fh2 mfdh;
	struct _hm2 *vhb;
};

#endif
