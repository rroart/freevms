// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

#include <linux/config.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>

#include<linux/linkage.h>
#include<linux/mm.h>
#include<linux/dirent.h>
#include<linux/file.h>
#ifdef __x86_64__
#include<linux/init.h>
#endif

#include<system_data_cells.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <atrdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <dyndef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <irpdef.h>
#include <rmsdef.h>
#include <sbkdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <ucbdef.h>
#include <ddbdef.h>
#include <fcbdef.h>
#include <scbdef.h>
#include <wcbdef.h>
#include <vmstime.h>

#include <pridef.h>
#include <iodef.h>
#include <misc.h>

#include <fibdef.h>
#include <fiddef.h>

#include <queue.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <starlet.h>

#include <linux/slab.h>

#include "x2p.h"

#define EXT2_EF 30

#if 0
struct __exttwo xe2ps[1]; // number of pids 

struct __exttwo *x2p;
#else
struct __exttwo *x2p = 0x7fff1000;
#endif
void __init exttwo_init(void) {
#if 0
  int i;
  for(i=0;i<1;i++) {
    qhead_init(&xe2ps[i].exttwo_head);
  }
  x2p = &xe2ps[0];
#endif
}

void exttwo_init2(void * vcb) {
  qhead_init(&x2p->exttwo_head);
  x2p->current_vcb = vcb;
}

void * exttwo_get_current_vcb() {
 return x2p->current_vcb;
}

void exttwo$dispatch(struct _irp * i) {
  insque(i, &x2p->exttwo_head);
  //  insque(i->irp$l_ioqfl, &xqps[current->pid].xqp_queue);
  exttwo$dispatcher();
}

void exttwo$dispatcher(void) {
  int pid=current->pcb$l_pid;
  struct _irp * i;
  int sts;
  int fcode, fmode;
  pid=0;
  while (!aqempty(&x2p->exttwo_head)) {
    i=remque(x2p->exttwo_head,0);
    x2p->io_channel=i->irp$w_chan;
    x2p->current_ucb=i->irp$l_ucb;
    x2p->current_vcb=((struct _ucb *)x2p->current_ucb)->ucb$l_vcb;
    fcode=i->irp$v_fcode;
    fmode=i->irp$v_fmod;
    iosbret(i,SS$_NORMAL);

    switch (fcode) {
    case IO$_ACCESS:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	struct dsc$descriptor * fibdsc=i->irp$l_qio_p1;
	struct dsc$descriptor * filedsc=i->irp$l_qio_p2;
	unsigned short *reslen=i->irp$l_qio_p3;
	struct dsc$descriptor * resdsc=i->irp$l_qio_p4;
	void * atrp=i->irp$l_qio_p5;
	struct _fibdef * fib=(struct _fibdef *)fibdsc->dsc$a_pointer;
	unsigned action=0;
	if (i->irp$l_func & IO$M_ACCESS) action=0;
	if (i->irp$l_func & IO$M_CREATE) action=2;
	if (i->irp$l_func & IO$M_DELETE) action=1;
	//if (fib->fib$w_did_num>0) 
	sts=exttwo_access(vcb,i);
	if (sts==SS$_NOSUCHFILE && (i->irp$l_func & IO$M_CREATE))
	  goto create;
      }
      break;
    case IO$_READVBLK:
    case IO$_WRITEVBLK:
      {
	char * buffer;
	exttwo_read_writevb(i);
	//return; // too early, maybe, but because of io_done
	//accesschunk(0,i->irp$l_qio_p3,&buffer,0,0,i);
	//memcpy(i->irp$l_qio_p1,buffer,512);
      }
      break;
    case IO$_CREATE:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	//exttwo$create(vcb,i);
      create:
	exttwo_create(vcb,i);
      }
      break;
    case IO$_DELETE:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	exttwo_delete(vcb,i);
      }
      break;
    case IO$_MODIFY:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	exttwo_modify(vcb,i);
      }
      break;
    default:
      printk ("x2p %x code not implemented yet\n",fcode);
      break;
    }
    if (i) 
      exttwo_io_done(i);
  }
}

#if 0
exttwo_create(struct _vcb *vcb,struct _irp * i)
{
  struct dsc$descriptor * fibdsc=i->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=i->irp$l_qio_p2;
  struct _fibdef * fib=fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&fib->fib$w_fid_num;
  char *filename=filedsc->dsc$a_pointer;
  struct _fh2 *head;
  unsigned idxblk;
  unsigned sts;
  struct _fcb *fcb;
  struct _iosb iosb;
  sts = 1; //update_addhead(vcb,filename,fib,0,fid,&head,&idxblk);
  if (!(sts & 1)) return sts;
  //sts = deaccesshead(head,idxblk);
  //    sts = writehead(getidxfcb(vcb),head);

  if ((i->irp$l_func & IO$M_ACCESS) && (sts & 1)) {
    unsigned short reslen;
    char retbuf[256];
    struct dsc$descriptor resdsc;
    resdsc.dsc$w_length=255;
    resdsc.dsc$a_pointer=&retbuf;
    //fib->fib$w_did_num = 0;
    //fib->fib$w_did_seq = 0;
    //fib->fib$b_did_rvn = 0;
    //fib->fib$b_did_nmx = 0;
    sts = exttwo_access(vcb,i); // should not be, but can not implement otherwise for a while
    // the if 0s is copied, too, but not changed
#if 0
    fcb=f11b_search_fcb(vcb,fid);
    head = f11b_read_header(vcb,fid,fcb,&iosb);
    sts=iosb.iosb$w_status;
    if (sts & 1) {
    } else {
      printk("Accessfile status in create %d\n",sts);
      iosbret(i,sts);
      return sts;
    }

    if (fcb==NULL) {
      fcb=exttwo_fcb_create2(head,&sts);
    }

    if (fcb == NULL) { iosbret(i,sts); return sts; }

    xqp->primary_fcb=fcb;
    xqp->current_window=&fcb->fcb$l_wlfl;
#endif

  }

  x2p->primary_fcb=exttwo_search_fcb(x2p->current_vcb,&fib->fib$w_fid_num);

#if 0
  // not yet?
  if ((fib->fib$w_exctl&FIB$M_EXTEND) && (sts & 1)) {
    struct _fcb * newfcb;
    newfcb=exttwo_search_fcb(x2p->current_vcb,&fib->fib$w_fid_num);
    sts = exttwo_extend(newfcb,fib->fib$l_exsz,0);
  }
#endif

  printk("(%d,%d,%d) %d\n",fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn,sts);
  return sts;
}
#endif

struct readdir_callback2 {
  struct dirent64 * dirent;
  int count;
};

#if 0
exttwo_delete(struct _vcb * vcb,struct _irp * irp) {
  struct _iosb iosb;
  struct _fcb *fcb;
  struct dsc$descriptor * fibdsc=irp->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=irp->irp$l_qio_p2;
  unsigned short *reslen=irp->irp$l_qio_p3;
  struct dsc$descriptor * resdsc=irp->irp$l_qio_p4;
  struct _fibdef * fib=fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&((struct _fibdef *)fibdsc->dsc$a_pointer)->fib$w_fid_num;
  int sts=1;
  //struct _fh2 *  head;
  char name[256], *tmpname;
  int wild=0;
  unsigned action=1;

  if (fib->fib$w_fid_num==4 && fib->fib$w_fid_seq==4 && fib->fib$w_did_num==0 && fib->fib$w_did_seq==0) {
    memset(&x2p->context_save,0,54);
    x2p->prev_fp=0;
  }

  if (fib->fib$w_did_num) {
    int wildcard=0;
    struct readdir_callback2 buf;
    int fd=0;
    struct file * f=0;
    signed int error=0;
    int dirflg=0;
    struct dirent64 dir;
    struct _fcb * fcb=x2p->primary_fcb;

    if (strchr(filedsc->dsc$a_pointer,'*') || strchr(filedsc->dsc$a_pointer,'%'))
      wildcard=1;

    dir.d_ino=0;
    dir.d_type=0;
    memset(dir.d_name,0,256);
    memset(name,0,256);

    if (wildcard || (fib->fib$w_nmctl & FIB$M_WILD)) {
        fib->fib$l_wcc = 1/*curblk*/;
	strcpy(name,&x2p->context_save);
	wild=1;
    } else {
        fib->fib$l_wcc = 0;
	strcpy(name,&x2p->context_save);
	if (irp->irp$v_fcode != IO$_DELETE) {
	  name[strlen(name)]='/';
	  exttwo_vms_to_unix(name+strlen(name),filedsc);
	  strcpy(&x2p->context_save,name);
	}
	if (strstr(filedsc->dsc$a_pointer,".DIR")) {
	  dirflg=O_DIRECTORY;
	}
    }

    if (irp->irp$v_fcode == IO$_DELETE) {
      printk("unlink/rm %s\n",name);
      f=sys_unlink(name);
    } else {
      //      f=filp_open(name, O_RDONLY|O_NONBLOCK|O_LARGEFILE|dirflg, 0);
    }
    buf.count = 0;
    buf.dirent = &dir;
    if (IS_ERR(f)) {
      printk("IS ERR %x\n",f);
      sts=SS$_NOSUCHFILE;
      iosbret(irp,sts);
      return sts;
    }
  }

  if ( (sts & 1) == 0) { iosbret(irp,sts); return sts; }

  return sts;
}
#endif

exttwo_modify(struct _vcb * vcb, struct _irp * irp) {
  struct _iosb iosb;
  unsigned sts=SS$_NORMAL;
  unsigned wrtflg=1;
  struct _fcb *fcb;
  //struct _fh2 *head;
  struct dsc$descriptor * fibdsc=irp->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=irp->irp$l_qio_p2;
  unsigned short *reslen=irp->irp$l_qio_p3;
  struct dsc$descriptor * resdsc=irp->irp$l_qio_p4;
  void * atrp=irp->irp$l_qio_p5;
  struct _fibdef * fib=(struct _fibdef *)fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&((struct _fibdef *)fibdsc->dsc$a_pointer)->fib$w_fid_num;
  unsigned action=0;
  if (irp->irp$l_func & IO$M_ACCESS) action=0;
  if (irp->irp$l_func & IO$M_DELETE) action=1;
  if (irp->irp$v_fcode == IO$_CREATE) action=2;
  if (irp->irp$l_func & IO$M_CREATE) action=2;
  action=0;
#ifdef DEBUG
  printk("Accessing file (%d,%d,%d)\n",(fid->fid$b_nmx << 16) +
	 fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn);
#endif

  x2p->current_vcb=vcb; // until I can place it somewhere else

  if ( (sts & 1) == 0) { iosbret(irp,sts); return sts; }

  if (irp->irp$l_qio_p5 == 0)
    return SS$_NORMAL;

  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) { iosbret(irp,SS$_WRITLCK);  return SS$_WRITLCK; }

  fcb=exttwo_search_fcb(vcb,fid);
  if (sts & 1) {
  } else {
    printk("Accessfile status %d\n",sts);
    iosbret(irp,sts);
    return sts;
  }

  fcb=x2p->primary_fcb;

#if 0
  if (fcb==NULL) {
    fcb=fcb_create2(head,&sts);
  }
  if (fcb == NULL) { iosbret(irp,sts); return sts; }
#endif

  x2p->primary_fcb=fcb;
  x2p->current_window=&fcb->fcb$l_wlfl;

  if (atrp) exttwo_write_attrib(fcb,atrp);

  if ((fib->fib$w_exctl&FIB$M_EXTEND) && (sts & 1)) {
    struct _fcb * newfcb;
    newfcb=exttwo_search_fcb(x2p->current_vcb,&fib->fib$w_fid_num);
    sts = exttwo_extend(newfcb,fib->fib$l_exsz,0);
  }

  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}

int exttwo_io_done(struct _irp * i) {
  return f11b_io_done(i);
}

void myqio(long rw, long data, long size, long blocknr,kdev_t dev, long block_factor)
{
  int sts;
  int type;
  struct _iosb iosb;
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  struct _vcb * vcb = x2p->current_vcb;
  sts = sys$qiow(EXT2_EF,(unsigned short)x2p->io_channel,type,&iosb,0,0,
		     data,size, blocknr*block_factor,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
  return sts;
}

void vms_submit_bh(int rw, struct buffer_head * bh)
{
  int sts;
  int type;
  struct _iosb iosb;
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  struct _vcb * vcb = x2p->current_vcb;
  sts = sys$qiow(EXT2_EF,(unsigned short)x2p->io_channel,type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
  return sts;
}

void vms_ll_rw_block(int rw, int nr, struct buffer_head * bhs[],kdev_t dev)
{
  int sts;
  int type;
  struct _iosb iosb;
  struct buffer_head * bh=bhs[0];
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  struct _vcb * vcb = x2p->current_vcb;
  sts = sys$qiow(EXT2_EF,(unsigned short)x2p->io_channel,type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
}

void vms_mark_buffer_dirty(struct buffer_head * bh)
{
  int rw=1;
  int sts;
  int type;
  struct _iosb iosb;
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  struct _vcb * vcb = x2p->current_vcb;
  sts = sys$qiow(EXT2_EF,(unsigned short)x2p->io_channel,type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
}

void __mark_inode_dirty(struct inode *inode, int flags)
{
	struct super_block * sb = inode->i_sb;
	struct _fcb * fcb = inode;

	if (x2p->current_vcb == 0)
		return;
	
	ext2_write_inode(x2p->current_vcb, fcb, flags);

#if 0
	/* Don't do this for I_DIRTY_PAGES - that doesn't actually dirty the inode itself */
	if (flags & (I_DIRTY_SYNC | I_DIRTY_DATASYNC)) {
		if (sb->s_op && sb->s_op->dirty_inode)
			sb->s_op->dirty_inode(inode);
	}

	/* avoid the locking if we can */
	if ((inode->i_state & flags) == flags)
		return;

	spin_lock(&inode_lock);
	if ((inode->i_state & flags) != flags) {
		inode->i_state |= flags;
		/* Only add valid (ie hashed) inodes to the dirty list */
		if (!(inode->i_state & I_LOCK) && !list_empty(&inode->i_hash)) {
			list_del(&inode->i_list);
			list_add(&inode->i_list, &sb->s_dirty);
		}
	}
	spin_unlock(&inode_lock);
#endif
}

extern char root_device_name[64];

void * global_e2_vcb = 0;

#if 0
unsigned mounte2(unsigned flags, unsigned devices,char *devnam[],char *label[],struct _vcb **retvcb)
{
  struct _iosb iosb;
  unsigned device,sts = SS$_NORMAL;
  struct _vcb *vcb = 0;
  struct _vcb *vcbdev;
  struct _ucb *ucb;
  struct ext2_super_block home;
  struct _aqb *aqb;
  unsigned short int chan=0;
  int islocal;
  if (sizeof(struct ext2_super_block) != 1024 ) return SS$_NOTINSTALL;
  for (device = 0; device < devices; device++) {
    if (strchr(devnam[device],'_')) {
      ucb = du_init(devnam[device]);
      islocal=0;
    } else {
      struct dsc$descriptor d;
      d.dsc$w_length=strlen(devnam[device]);
      d.dsc$a_pointer=devnam[device];
      //printk("Trying to assign %s\n",devnam[device]);
      sts=exe$assign(&d,&chan,0,0,0);
      //printk("Assign sts %x chan %x\n",sts, chan);
      ucb = ctl$gl_ccbbase[chan].ccb$l_ucb;
      //printk("Got ucb %x\n",ucb);
      islocal=1;
    }
    vcb = (struct _vcb *) kmalloc(sizeof(struct _vcb),GFP_KERNEL);
    memset(vcb,0,sizeof(struct _vcb));
    global_e2_vcb = vcb;
    vcb->vcb$b_type=DYN$C_VCB;
    x2p->current_vcb=vcb; // until I can place it somewhere else
    aqb = (struct _aqb *) kmalloc(sizeof(struct _aqb),GFP_KERNEL);
    memset(aqb,0,sizeof(struct _aqb));
    aqb->aqb$b_type=DYN$C_AQB;
    qhead_init(&aqb->aqb$l_acpqfl);
    aqb->aqb$l_acppid=1;
    ucb->ucb$l_vcb=vcb;
    vcb->vcb$l_aqb=aqb;
    if (vcb == NULL) return SS$_INSFMEM;
    vcb->vcb$b_status = 0;
    if (flags & 1) vcb->vcb$b_status |= VCB$M_WRITE_IF;
    qhead_init(&vcb->vcb$l_fcbfl);
    vcb->vcb$l_cache = NULL; // ?
    vcbdev = vcb;
    sts = SS$_NOSUCHVOL;
    //    vcbdev->dev = NULL;
    if (strlen(devnam[device])) {
      int hba;
      struct dsc$descriptor dsc;
#if 0
      if (islocal)
	sts = phyio_init(strlen(devnam[device])+1,u&cb->ucb$l_ddb->ddb$t_name[1],&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count,0);
#endif

#if 0
      dsc.dsc$w_length=strlen(devnam[device]);
      dsc.dsc$a_pointer=devnam[device];
      if (!islocal)
	dsc.dsc$a_pointer=((char *) dsc.dsc$a_pointer)+1;
      sts=exe$assign(&dsc,&chan,0,0,0);
#endif
      x2p->io_channel=chan;
      ucb->ucb$ps_adp=chan; //wrong field and use, but....
      //sts = device_lookup(strlen(devnam[device]),devnam[device],1,&ucbret);
      //if (!(sts & 1)) break;
      //      ucb->handle=vcbdev->dev->handle;
#define HOME_LIMIT 5
      for (hba = 1; hba <= HOME_LIMIT; hba++) {
	//printk("bef qio %x %x\n",hba,chan);
	sts = sys$qiow(EXT2_EF,chan,IO$_READLBLK,&iosb,0,0,(char *) &home,sizeof(struct ext2_super_block),hba,root_device_name[3]-48,0,0);
	//	printk("aft qio %x\n",sts);
	if (!(sts & 1)) break;
	if (home.s_magic == EXT2_SUPER_MAGIC)
	  break;
	sts = SS$_DATACHECK;
      }
      if (!(sts & 1)) break;
    }
    if (sts & 1) {
      vcbdev = vcb;
      if (strlen(devnam[device])) {
	struct _fcb * idxfcb;
	struct _fcb * mapfcb;
	//	  vcb->vcb$ibmaplbn = idxfcb->stlbn;
	vcb->vcb$l_homelbn = home.s_first_data_block;
	vcb->vcb$l_ibmaplbn = 0;//home.hm2$l_ibmaplbn;
	vcb->vcb$l_ibmapvbn = 0;//home.hm2$w_ibmapvbn;
	vcb->vcb$l_ibmapsize = 0;//home.hm2$w_ibmapsize;  // wrong use
	vcb->vcb$l_cluster = 0;//home.hm2$w_cluster;
	vcb->vcb$l_maxfiles = 0;//home.hm2$l_maxfiles;
	//vcb->vcb$l_free = 500; // how do we compute this?
	memcpy(&vcb->vcb$t_volname,devnam[device],12);
	//sts = accessfile(vcb,&idxfid,&idxfcb,flags & 1);

	if (!(iosb.iosb$w_status & 1)) {
	  ucb->ucb$l_vcb = NULL;
	} else {
	  ucb->ucb$l_vcb = vcb;
	  vcb->vcb$l_rvt = ucb; // just single volume so far
	  vcbdev->vcb$l_free=home.s_free_blocks_count;
	  printk("%%MOUNT-I-FREE, Freespace is %d\n",vcbdev->vcb$l_free);
	}
      }
    }
  }
  if (retvcb != NULL) *retvcb = vcb;
  return sts;
}
#endif

extern char root_device_name[64];

void vms_mount(void) {
        int sts;
        struct VCB *vcb;
	struct item_list_3 it[3];
	it[0].item_code=1; /*not yet */
	it[0].buflen=strlen(root_device_name);
	it[0].bufaddr=root_device_name;
	it[1].item_code=1;
	it[2].item_code=0;
	//        sts = mount(options,devices,devs,labs,&vcb);
        sts = exe$mount(it);
}

#if 0
static int fillonedir64(void * __buf, const char * name, int namlen, loff_t offset, ino_t ino, unsigned int d_type)
{
  struct readdir_callback2 * buf = (struct readdir_callback2 *) __buf;
  struct dirent64 * dirent;

  if (buf->count)
    return -EINVAL;
  buf->count++;
  dirent = buf->dirent;
  dirent->d_ino=ino;
  dirent->d_off=offset;
  memcpy(dirent->d_name, name, namlen);
  dirent->d_name[namlen]=0;
  return 0;
}
#endif

#if 0
unsigned exttwo_access(struct _vcb * vcb, struct _irp * irp)
{
  struct _iosb iosb;
  unsigned sts=SS$_NORMAL;
  unsigned wrtflg=1;
  struct _fcb *fcb;
  //  struct _fh2 *head;
#ifdef __i386__
  char name[256], *tmpname;
#else
  char * name = kmalloc(256,GFP_KERNEL), *tmpname;
#endif
  struct dsc$descriptor * fibdsc=irp->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=irp->irp$l_qio_p2;
  unsigned short *reslen=irp->irp$l_qio_p3;
  struct dsc$descriptor * resdsc=irp->irp$l_qio_p4;
  void * atrp=irp->irp$l_qio_p5;
  struct _fibdef * fib=(struct _fibdef *)fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&((struct _fibdef *)fibdsc->dsc$a_pointer)->fib$w_fid_num;
  int wild=0;
  unsigned action=0;
  if (irp->irp$l_func & IO$M_ACCESS) action=0;
  if (irp->irp$l_func & IO$M_DELETE) action=1;
  if (irp->irp$v_fcode == IO$_CREATE) action=2;
  if (irp->irp$l_func & IO$M_CREATE) action=2;
#ifdef DEBUG
  printk("Accessing file (%d,%d,%d)\n",(fid->fid$b_nmx << 16) +
	 fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn);
#endif

  x2p->current_vcb=vcb; // until I can place it somewhere else

  if (fib->fib$w_fid_num==4 && fib->fib$w_fid_seq==4 && fib->fib$w_did_num==0 && fib->fib$w_did_seq==0) {
    memset(&x2p->context_save,0,54);
    x2p->prev_fp=0;
  }

  if (fib->fib$w_did_num) {
    int wildcard=0;
    struct readdir_callback2 buf;
    int fd=0;
    struct file * f;
    signed int error=0;
    int dirflg=0;
#ifdef __i386__
    struct dirent64 dir;
#else
    struct dirent64 * dir = kmalloc(sizeof(struct dirent64),GFP_KERNEL);
#endif
    struct _fcb * head;
    struct _fcb * fcb=x2p->primary_fcb;
    head = fcb->fcb$l_primfcb;

    if (strchr(filedsc->dsc$a_pointer,'*') || strchr(filedsc->dsc$a_pointer,'%'))
      wildcard=1;

#ifdef __i386__
    dir.d_ino=0;
    dir.d_type=0;
    memset(dir.d_name,0,256);
#else
    dir->d_ino=0;
    dir->d_type=0;
    memset(dir->d_name,0,256);
#endif
    memset(name,0,256);

    if (wildcard || (fib->fib$w_nmctl & FIB$M_WILD)) {
        fib->fib$l_wcc = 1/*curblk*/;
	strcpy(name,&x2p->context_save);
	wild=1;
    } else {
        fib->fib$l_wcc = 0;
	strcpy(name,&x2p->context_save);
	if (irp->irp$v_fcode != IO$_CREATE) {
	  name[strlen(name)]='/';
	  exttwo_vms_to_unix(name+strlen(name),filedsc);
	  strcpy(&x2p->context_save,name);
	}
	if (strstr(filedsc->dsc$a_pointer,".DIR")) {
	  dirflg=O_DIRECTORY;
	}
    }

    if (irp->irp$v_fcode == IO$_CREATE) {
      f=filp_open(name, O_CREAT|O_NONBLOCK|O_LARGEFILE|dirflg, 0);
    } else {
      f=filp_open(name, O_RDONLY|O_NONBLOCK|O_LARGEFILE|dirflg, 0);
    }
    buf.count = 0;
#ifdef __i386__
    buf.dirent = &dir;
#else
    buf.dirent = dir;
#endif
    if (IS_ERR(f)) {
      sts=SS$_NOSUCHFILE;
      if ( (sts & 1) == 0 && (irp->irp$v_fcode != IO$_CREATE)) { 
#ifndef __i386__
	kfree(name);
#endif
#if 0
	memset(&x2p->context_save,0,54);
#endif
	iosbret(irp,sts);
	return sts;
      }
    }
    error=generic_file_llseek(f, x2p->prev_fp /*fib->fib$l_wcc*//*dir.d_off*/, 0);
    if (error<0)
      goto err;
    error=vfs_readdir(f, fillonedir64, &buf);
    if (error >= 0)
      error = buf.count;
  err:
    if (error == 0)
      sts =  SS$_NOMOREFILES;
    //fib->fib$l_wcc = f->f_pos;
    x2p->prev_fp = f->f_pos;
    if (sts!=SS$_NORMAL)
      x2p->prev_fp=0;

#ifdef __i386__
    if (dir.d_ino==0) {
      dir.d_ino=f->f_dentry->d_inode->i_ino;
      strcpy(dir.d_name,f->f_dentry->d_iname);
    }
#else
    if (dir->d_ino==0) {
      dir->d_ino=f->f_dentry->d_inode->i_ino;
      strcpy(dir->d_name,f->f_dentry->d_iname);
    }
#endif

    filp_close(f,0);
  
#ifdef __i386__  
    if (0==strcmp(name,"/") && 0==strcmp(dir.d_name,"."))
      strcpy(dir.d_name,"000000.DIR");
    if (0==strcmp(dir.d_name,".."))
      strcpy(dir.d_name,"DOTDOT.DIR");
    if (0==strcmp(dir.d_name,"."))
      strcpy(dir.d_name,"DOT.DIR");
    if (reslen) {
      // temp workaround for write?
      *reslen=strlen(dir.d_name);
      dir.d_name[(*reslen)++]=';';
      dir.d_name[(*reslen)++]='1';
      memcpy(resdsc->dsc$a_pointer,dir.d_name,*reslen);
      //printk("resdsc %s %x\n",resdsc->dsc$a_pointer,*reslen);
    }
    if (dir.d_ino!=2) {
      fib->fib$w_fid_num=head->i_dev;
      *(unsigned long*)(&fib->fib$w_fid_seq)=dir.d_ino;
#else
    if (0==strcmp(name,"/") && 0==strcmp(dir->d_name,"."))
      strcpy(dir->d_name,"000000.DIR");
    if (0==strcmp(dir->d_name,".."))
      strcpy(dir->d_name,"DOTDOT.DIR");
    if (0==strcmp(dir->d_name,"."))
      strcpy(dir->d_name,"DOT.DIR");
    if (reslen) {
      // temp workaround for write?
      *reslen=strlen(dir->d_name);
      dir->d_name[(*reslen)++]=';';
      dir->d_name[(*reslen)++]='1';
      memcpy(resdsc->dsc$a_pointer,dir->d_name,*reslen);
      //printk("resdsc %s %x\n",resdsc->dsc$a_pointer,*reslen);
    }
    if (dir->d_ino!=2) {
      fib->fib$w_fid_num=head->i_dev;
      *(unsigned long*)(&fib->fib$w_fid_seq)=dir->d_ino;
#endif
    } else {
      fib->fib$w_fid_num=4;
      fib->fib$w_fid_seq=4;
      fib->fib$b_fid_rvn=0;
      fib->fib$b_fid_nmx=0;
    }

    //temp hack to get a dentry
    {
#ifdef __i386__
      char * c=strchr(&dir.d_name,';');
      if (c) *c=0;
      f=filp_open(&dir.d_name, O_RDONLY,0);
#else
      char * c=strchr(&dir->d_name,';');
      if (c) *c=0;
      f=filp_open(&dir->d_name, O_RDONLY,0);
#endif
      if (f>=0 && f<0xf0000000) {
	if (f->f_dentry && f->f_dentry->d_inode)
	  make_fcb(f->f_dentry->d_inode);
	filp_close(f,0);
      }
      f=filp_open(name, O_RDONLY,0);
      if (f>=0 && f<0xf0000000) {
	if (f->f_dentry && f->f_dentry->d_inode)
	  make_fcb(f->f_dentry->d_inode);
	filp_close(f,0);
      }
#ifdef __i386__
      dir.d_name[strlen(dir.d_name)]='/';
      strcpy(dir.d_name+strlen(dir.d_name),name);
      f=filp_open(&dir.d_name, O_RDONLY,0);
#else
      dir->d_name[strlen(dir->d_name)]='/';
      strcpy(dir->d_name+strlen(dir->d_name),name);
      f=filp_open(&dir->d_name, O_RDONLY,0);
#endif
      if (f>=0 && f<0xf0000000) {
	if (f->f_dentry && f->f_dentry->d_inode)
	  make_fcb(f->f_dentry->d_inode);
	filp_close(f,0);
      }
    }
#ifndef __i386__
    kfree(dir);
#endif

#if 0
      sts = search_ent(fcb,fibdsc,filedsc,reslen,resdsc,eofblk,action);
      // readdir should be replaced with something like search_ent
#endif
  }
#ifndef __i386__
  kfree(name);
#endif

  if ( (sts & 1) == 0) { 
    memset(&x2p->context_save,0,54);
    iosbret(irp,sts); 
    return sts;
  }

  // has to reset context_save somewhere? this is a trial place
  if ((irp->irp$l_func & IO$M_ACCESS) == 0 && wild == 0 && fib->fib$w_did_num) {
    //    { int i; for (i=1000000;i;i--); }
    //    memset(&x2p->context_save,0,54);
  }

  if ((irp->irp$l_func & IO$M_ACCESS) == 0 && irp->irp$l_qio_p5 == 0)
    return SS$_NORMAL;

  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) { iosbret(irp,SS$_WRITLCK);  return SS$_WRITLCK; }

  //ext2_translate_fid(vcb,fid);
  fcb=exttwo_search_fcb2(vcb,fid); // can actually use most of the f11b routine
#if 0
  //head = f11b_read_header(vcb,fid,fcb,&iosb);
  if (sts & 1) {
  } else {
    printk("Accessfile status %d\n",sts);
    iosbret(irp,sts);
    return sts;
  }
#endif

  if (fcb==NULL) {
    //    fcb=fcb_create2(head,&sts);
  }
  if (fcb == NULL) { sts=SS$_NOSUCHFILE; iosbret(irp,sts); return sts; }

  x2p->primary_fcb=fcb;
  x2p->current_window=&fcb->fcb$l_wlfl;

  if (atrp) exttwo_read_attrib(fcb,fcb->fcb$l_primfcb,atrp);
  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}
#endif

int exttwo_read_writevb(struct _irp * i) {
  signed int lbn;
  char * buffer;
  struct _iosb iosb;
  struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
  struct _fcb * fcb = x2p->primary_fcb; // ???? is this right
  struct ext2_super_block * sb = vcb->vcb$l_cache;
  int factor = vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb));
  struct _wcb * wcb = &fcb->fcb$l_wlfl;
  int blocks=(i->irp$l_qio_p2+511)/512;
  if (blocks>1) panic("blocks. copy from corresponding f11 routine/n");
  int e2bn=(i->irp$l_qio_p3-1)/factor;
  int rest=(i->irp$l_qio_p3-1)&(factor-1);
  lbn=f11b_map_vbn(1+e2bn,wcb);
  if (i->irp$v_fcode==IO$_WRITEVBLK) {
    int err=0;
    if (lbn==-1)
      err=ext2_get_block(vcb, fcb, e2bn, &lbn, 1, fcb);
    if (err)
      panic("write lbn -1\n");
    //printk("tried to do an experimental write %x %x %x %x\n",i->irp$l_qio_p1,lbn,blocks,e2bn);
    exttwo_write_block(vcb,i->irp$l_qio_p1,lbn*factor+rest,blocks,&iosb);
    //inode->i_size+=512;
    ext2_sync_inode(vcb, fcb);
    ext2_write_super(vcb);
  } else {
    //if (lbn<0) printk("e2bn %x %x %x %x %x\n",lbn,e2bn,i->irp$l_qio_p1,i->irp$l_qio_p2,i->irp$l_qio_p3);
    if (lbn<0) return SS$_ENDOFFILE;
    buffer=exttwo_read_block(vcb,lbn*factor+rest,blocks,&iosb);
    //memcpy(i->irp$l_qio_p1,buffer+512*rest,512);
    memcpy(i->irp$l_qio_p1,buffer,512);
    kfree(buffer);
  }
  //exttwo_io_done(i);
}

#if 0
void * exttwo_read_block(struct _vcb * vcb, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned char * buf = kmalloc(512*count, GFP_KERNEL);
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(EXT2_EF,x2p->io_channel,IO$_READLBLK,&myiosb,0,0,buf,512*count,phyblk,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}

void * exttwo_write_block(struct _vcb * vcb, unsigned char * buf, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(EXT2_EF,x2p->io_channel,IO$_WRITELBLK,&myiosb,0,0,buf,512*count,phyblk,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}
#endif

void *exttwo_fcb_create(struct _fcb * inode, int i_ino, unsigned *retsts)
{
  struct _vcb * vcb=x2p->current_vcb;
  struct _fcb *fcb4;
  struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  memset(fcb,0,sizeof(struct _fcb));
  if (fcb == NULL) {
    if (retsts) *retsts = SS$_INSFMEM;
    return;
  } 
  fcb->fcb$b_type=DYN$C_FCB;
  fcb->fcb$l_fill_5 = 1;
  qhead_init(&fcb->fcb$l_wlfl);

  //  fcb->fcb$w_fid_dirnum=fcb->fcb$w_fid_dirnum;
  //fcb->fcb$l_fid_recnum=inode->i_ino; // bugged
  SET_FCB_FID_FROM_INO(fcb, FCB_FID_TO_INO(inode));

  insque(fcb,&vcb->vcb$l_fcbfl);

  fcb->fcb$l_efblk = inode -> fcb$l_efblk; //?
  fcb->fcb$l_filesize = inode -> fcb$l_filesize; //?

  if (inode->fcb$v_dir)
    fcb->fcb$v_dir = 1;
  if (inode->fcb$v_dir)
    fcb->fcb$v_isdir = 1;

  exttwo_wcb_create_all(vcb, fcb);

  //  fcb->fcb$l_hdlbn=f11b_map_idxvbn(vcb,head->fh2$w_fid.fid$w_num + (head->fh2$w_fid.fid$b_nmx << 16) - 1 + VMSWORD(vcb->vcb$l_ibmapvbn) + VMSWORD(vcb->vcb$l_ibmapsize));

  *retsts=SS$_NORMAL;
  if (FCB_FID_TO_INO(inode)!=2) return fcb;

  fcb4 = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  memcpy(fcb4,fcb,sizeof(struct _fcb));
  if (fcb == NULL) {
    if (retsts) *retsts = SS$_INSFMEM;
    return;
  } 

  fcb4->fcb$w_fid_num=4;
  fcb4->fcb$w_fid_seq=4;
  fcb4->fcb$w_fid_rvn=0;

  insque(fcb4,&vcb->vcb$l_fcbfl);

  return fcb;
}

#if 0
void make_fcb(struct _fcb * inode) {
  int sts;
  void * fcb;
  if (inode==0) return;
#if 0
  if (sb->s_magic!=EXT2_SUPER_MAGIC) return;
#endif
  fcb=exttwo_search_fcb(x2p->current_vcb,inode);
  if (fcb==0)
    exttwo_fcb_create(inode,&sts);
}
#endif

struct _fcb * e2_search_fcb(void * v) {
  return v;
  if (x2p->current_vcb)
    return exttwo_search_fcb(x2p->current_vcb,v);
  else
    return 0;
}

signed long e2_map_vbn(struct _fcb * fcb, signed long vbn) {
  return f11b_map_vbn(1+vbn,&fcb->fcb$l_wlfl);
}

int e2_fcb_wcb_add_one(struct _fcb * fcb,signed long vbn,signed long result)
{
  int retsts;
  struct _wcb * wcb;
  struct _wcb * head=&fcb->fcb$l_wlfl;
  struct _wcb * tmp=head->wcb$l_wlfl;
  while (tmp!=head) {
    if (vbn>=tmp->wcb$l_stvbn && vbn<=(tmp->wcb$l_stvbn+tmp->wcb$l_p1_count) && result==(tmp->wcb$l_p1_lbn+tmp->wcb$l_p1_count)) {
      tmp->wcb$l_p1_count++;
      return SS$_NORMAL;
    }
    tmp=tmp->wcb$l_wlfl;
  }

  wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb),GFP_KERNEL);
  memset(wcb,0,sizeof(struct _wcb));
  if (wcb == NULL) {
    retsts = SS$_INSFMEM;
    return retsts;
  }

  wcb->wcb$b_type=DYN$C_WCB;
  wcb->wcb$l_orgucb=x2p->current_ucb;
  insque(wcb,&fcb->fcb$l_wlfl);
  wcb->wcb$l_fcb=fcb;
  
  wcb->wcb$l_stvbn=vbn;
  wcb->wcb$l_p1_count=1;
  wcb->wcb$l_p1_lbn=result;

  return SS$_NORMAL;
}

char * exttwo_vms_to_unix(char * name,struct dsc$descriptor * dsc) {
  char *c=name;
  char *d;
  strcpy(c,dsc->dsc$a_pointer);
  if (0==strncmp(c,"000000.",7)) {
    c[1]=0;
    c[0]='/';
    return c;
  }
  d=strstr(c,".DIR");
  if (d)
    *d=0;
  d=strchr(c,';');
  if (d)
    *d=0;
  if (c[strlen(c)-1]=='.')
    c[strlen(c)-1]=0;
  return c;
}

extern int mount_root_vfs;

int path_unix_to_vms(char * new, char * old) {
  int len = strlen(old);
  memcpy(new, old, len);
  new[len]=0;
  if (mount_root_vfs)
    return 1;
  char * c;
  c = strchr(new, '/');
  if (c)
    *c = '[';
  c = strrchr(new, '/');
  if (c)
    *c = ']';
  while (c = strchr(new, '/'))
    *c = '.';
  return 1;
}

int convert_soname(char * name) {
  if (mount_root_vfs)
    return 1;
  char * c = strstr(name, ".so.");
  if (c)
    memcpy(c, ".ele", 4);
  if (c)
    c[4]=0;
  return 1;
}

void exttwo_translate_fid (struct _vcb * vcb, struct _fiddef * fid) {
  if (fid->fid$w_num==4 && fid->fid$w_seq==4 && fid->fid$w_rvn==0) {
#if 0
    fid->fid$w_num=vcb->vcb$l_fcbfl->fcb$w_fid_dirnum; // i_dev
    fid->fid$w_seq=2; // EXT2 root ino
    fid->fid$w_rvn=0; // EXT2 root ino
#else
    SET_FID_FROM_INO(fid,2);
#endif
  }
}

void exttwo_translate_ino (struct _vcb * vcb, struct _fiddef * fid) {
  if (fid->fid$w_num==2 && fid->fid$w_seq==0 && fid->fid$w_rvn==0) {
#if 0
    fid->fid$w_num=vcb->vcb$l_fcbfl->fcb$w_fid_dirnum; // i_dev
    fid->fid$w_seq=2; // EXT2 root ino
    fid->fid$w_rvn=0; // EXT2 root ino
#else
    SET_FID_FROM_INO(fid,4 + (4<<16));
#endif
  }
}

void * exttwo_search_fcb2(struct _vcb * vcb,struct _fiddef * fid)
{
    struct _fcb * head = &vcb->vcb$l_fcbfl;
    struct _fcb * tmp = head->fcb$l_fcbfl;
    while(tmp!=head) {
      if ((tmp->fcb$w_fid_seq==fid->fid$w_seq) && (tmp->fcb$w_fid_num==fid->fid$w_num)&& (tmp->fcb$w_fid_rvn==fid->fid$w_rvn)) return tmp;
      tmp=tmp->fcb$l_fcbfl;
    }
    return 0;
}

#if 0
//not used?
unsigned long e2_access_file(const char *name) {
  char result[256];
  unsigned long reslen;
  int sts;
  struct _iosb iosb;

  struct _fibdef fibblk;
  struct dsc$descriptor fibdsc,resdsc,fildsc;

  memset(&fibblk, 0, sizeof(fibblk));
  fibblk.fib$w_fid_num = 4;
  fibblk.fib$w_fid_seq = 4;

  fibdsc.dsc$w_length = sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer = (char *) &fibblk;

  resdsc.dsc$w_length = 256;
  resdsc.dsc$a_pointer = result;

  fildsc.dsc$w_length = strlen(name);
  fildsc.dsc$a_pointer = name;

  //memcpy(&fibblk.fib$w_did_num,&wcc->wcd_dirid,sizeof(struct _fiddef));
  fibblk.fib$w_nmctl = 0;     /* FIB_M_WILD; */
  fibblk.fib$l_acctl = 0;
  fibblk.fib$w_fid_num = 0;
  fibblk.fib$w_fid_seq = 0;
  fibblk.fib$b_fid_rvn = 0;
  fibblk.fib$b_fid_nmx = 0;
  fibblk.fib$l_wcc = 1; //wcc->wcd_wcc;

  fibblk.fib$w_fid_num = fibblk.fib$w_did_num;
  fibblk.fib$w_fid_seq = fibblk.fib$w_did_seq;
  fibblk.fib$w_did_num = 0;
  fibblk.fib$w_did_seq = 0;
  sts = sys$qiow(EXT2_EF,getchan(x2p->current_vcb),IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		 &fibdsc,&fildsc,&reslen,&resdsc,0,0);
  sts = iosb.iosb$w_status;

  fibblk.fib$w_did_num = fibblk.fib$w_fid_num;
  fibblk.fib$w_did_seq = fibblk.fib$w_fid_seq;
  fibblk.fib$w_fid_num = 0;
  fibblk.fib$w_fid_seq = 0;
  sts = sys$qiow(EXT2_EF,getchan(x2p->current_vcb),IO$_ACCESS,&iosb,0,0,
		 &fibdsc,&fildsc,&reslen,&resdsc,0,0);
  sts = iosb.iosb$w_status;
  return sts;
}
#endif
