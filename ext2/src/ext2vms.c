#include <linux/config.h>
#include <linux/fs.h>

#include <pridef.h>
#include <iodef.h>
#include <misc.h>


void vms_ll_rw_block(int rw, int nr, struct buffer_head * bhs[],kdev_t dev)
{
  int sts;
  int type;
  unsigned long long iosb;
  struct buffer_head * bh=bhs[0];
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  sts = exe_qiow(0,(unsigned short)dev2chan(dev),type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,MINOR(dev)&31,0,0);
}

void vms_mark_buffer_dirty(struct buffer_head * bh)
{
  int rw=1;
  int sts;
  int type;
  unsigned long long iosb;

  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  sts = exe_qiow(0,(unsigned short)dev2chan(bh->b_dev),type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,MINOR(bh->b_dev)&31,0,0);
}
