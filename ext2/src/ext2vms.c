#include <linux/config.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>

#include<linux/vmalloc.h>
#include<linux/linkage.h>
#include<linux/mm.h>
#include<linux/dirent.h>
#include<linux/file.h>

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

struct __exttwo {
  // unsigned long long xqp_queue;
  unsigned long exttwo_head;
  unsigned long exttwo_tail;
unsigned long exttwo_dispatcher;
unsigned long io_ccb;
unsigned long io_channel;
unsigned long user_status[2];
unsigned long io_status[2];
unsigned long io_packet;
unsigned long current_ucb;
unsigned long current_vcb;
unsigned long current_rvt;
unsigned long current_rvn;
char stsflags;
char block_check;
unsigned long header_lbn;
unsigned long bitmap_vbn;
unsigned long bitmap_rvn;
unsigned long bitmap_buffer;
unsigned long save_status;
unsigned long long privs_used;
unsigned long acb_addr;
unsigned long bfr_list[8];
unsigned long bfr_credits[2];
unsigned long bfrs_used[2];
unsigned long cache_hdr;
unsigned long context_start;
unsigned long cleanup_flags;
unsigned long file_header;
unsigned long primary_fcb;
unsigned long current_window;
unsigned long current_fib;
unsigned long curr_lckindx;
unsigned long prim_lckindx;
unsigned long loc_rvn;
unsigned long loc_lbn;
unsigned long unrec_lbn;
unsigned long unrec_count;
unsigned long unrec_rvn;
char prev_link[6];
char context_save[54];
unsigned long lb_lockid[5];
unsigned long lb_basis[5];
unsigned long lb_hdrseq[5];
unsigned long lb_dataseq[5];
unsigned long lb_oldhdrseg[5];
unsigned long lb_olddataseg[5];
unsigned long quota_dataseq;
unsigned long quota_olddataseq;
unsigned long dir_fcb;
unsigned long dir_lckindx;
unsigned long dir_record;
char dir_context[112];
char old_version_fid[6];
unsigned long prev_version;
char prev_name[81];
unsigned long block_vcb;
unsigned long audit_arglist[16];
unsigned long prev_fp;
};

struct __exttwo xe2ps[1]; // number of pids 

struct __exttwo *x2p;

void __init exttwo_init(void) {
  int i;
  for(i=0;i<1;i++) {
    qhead_init(&xe2ps[i].exttwo_head);
  }
  x2p = &xe2ps[0];
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

exttwo_create() {}
exttwo_delete() {}
exttwo_modify() {}
int exttwo_io_done(struct _irp * i) {
  return f11b_io_done(i);
}

void myqio(int rw, int data, int size, int blocknr,kdev_t dev, int block_factor)
{
  int sts;
  int type;
  unsigned long long iosb;
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  sts = exe_qiow(0,(unsigned short)dev2chan(dev),type,&iosb,0,0,
		     data,size, blocknr*block_factor,MINOR(dev)&31,0,0);
  return sts;
}

void vms_submit_bh(int rw, struct buffer_head * bh)
{
  int sts;
  int type;
  unsigned long long iosb;
  if (rw)
    type=IO$_WRITEPBLK;
  else
    type=IO$_READPBLK;
  sts = exe_qiow(0,(unsigned short)dev2chan(bh->b_dev),type,&iosb,0,0,
		     bh->b_data,bh->b_size, bh->b_blocknr,MINOR(bh->b_dev)&31,0,0);
  return sts;
}

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

int ext2_wcb_create_all(struct _fcb * fcb, struct inode * inode)
{
  struct super_block * s=inode->i_sb;
  int ptrs = EXT2_ADDR_PER_BLOCK(inode->i_sb);
  int ptrs_bits = EXT2_ADDR_PER_BLOCK_BITS(inode->i_sb);
  const long direct_blocks = EXT2_NDIR_BLOCKS,
    indirect_blocks = ptrs,
    double_blocks = (1 << (ptrs_bits * 2));
  struct _iosb iosb;
  unsigned int retsts;
  unsigned curvbn=1;
  unsigned extents = 0;
  int i;

  struct _ucb * ucb = finducb(fcb); // bad bad
  unsigned short *mp;
  unsigned short *me;
  int contin=0;
  int nextvbn=0;
  int stvbn=0;
  int phylen=1;
  int nextphyblk=1;
  int stphyblk=inode->u.ext2_i.i_data[0];
  int phyblk=stphyblk;
  int l1=0,l2=0,l3=0;
  int l1p=0,l2p=0,l3p=0;
  long *b1,*b2,*b3;
 
  retsts=0;
  for(i=0;i<15;i++)
    retsts|=inode->u.ext2_i.i_data[i];
  if (retsts==0)
    return SS$_NORMAL;

  b1=kmalloc(EXT2_BLOCK_SIZE(s),GFP_KERNEL);
  b2=kmalloc(EXT2_BLOCK_SIZE(s),GFP_KERNEL);
  b3=kmalloc(EXT2_BLOCK_SIZE(s),GFP_KERNEL);

  for(i=1; i<15 && nextphyblk; 1) {

    phyblk= nextphyblk;

    if (i<12) {
      nextphyblk=inode->u.ext2_i.i_data[i];
      i++;
    } else {
      if (inode->u.ext2_i.i_data[i]==0) {
	i++;
	nextphyblk=0;
      }
      if (l1==0) {
	l1=inode->u.ext2_i.i_data[i];
	myqio(READ,b1,EXT2_BLOCK_SIZE(s),l1,inode->i_dev,vms_block_factor(inode->i_blkbits));
      }
      if (i==12) {
	nextphyblk=b1[l1p];
	l1p++;
	if (l1p==ptrs) {
	  l1p=0;
	  l1=0;
	  i++;
	  goto out;
	}
      }
      if (i==13) {
	if (l2==0) {
	  l2=b1[l1p];
	  myqio(READ,b2,EXT2_BLOCK_SIZE(s),l2,inode->i_dev,vms_block_factor(inode->i_blkbits));
	}
	nextphyblk=b2[l2p];
	l2p++;
	if (l2p==ptrs) {
	  l2p=0;
	  l2=0;
	  l1p++;
	}
	if (l1p==ptrs) {
	  l1p=0;
	  l1=0;
	  i++;
	}
      }
      if (i==14) {
	// 3rd not supported yet
	i++;
      }
    }

  out:

    if ((phyblk+1)!=nextphyblk) {
      contin=0;
    } else {
      contin=1;
      phyblk++;
      phylen++;
    }

    if (contin==0) {
      struct _wcb * wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb),GFP_KERNEL);
      bzero(wcb,sizeof(struct _wcb));
      if (wcb == NULL) {
	retsts = SS$_INSFMEM;
	return retsts;
      }

      wcb->wcb$b_type=DYN$C_WCB;
      wcb->wcb$l_orgucb=ucb;
      insque(wcb,&fcb->fcb$l_wlfl);
      wcb->wcb$l_fcb=fcb;

      wcb->wcb$l_stvbn=stvbn;
      wcb->wcb$l_p1_count=phylen;
      wcb->wcb$l_p1_lbn=stphyblk;
      
      stvbn += phylen;
      stphyblk = nextphyblk;

      phylen=1;
      phyblk=0;

    }



  }

  kfree(b1);
  kfree(b2);
  kfree(b3);

  retsts = SS$_NORMAL;
  return retsts;
}

extern char root_device_name[64];

unsigned mounte2(unsigned flags, unsigned devices,char *devnam[],char *label[],struct _vcb **retvcb)
{
  struct _iosb iosb;
  unsigned device,sts;
  struct _vcb *vcb;
  struct _vcb *vcbdev;
  struct _ucb *ucb;
  struct ext2_super_block home;
  struct _aqb *aqb;
  int chan=0;
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
    bzero(vcb,sizeof(struct _vcb));
    vcb->vcb$b_type=DYN$C_VCB;
    x2p->current_vcb=vcb; // until I can place it somewhere else
    aqb = (struct _aqb *) kmalloc(sizeof(struct _aqb),GFP_KERNEL);
    bzero(aqb,sizeof(struct _aqb));
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
	sts = phyio_init(strlen(devnam[device])+1,ucb->ucb$l_ddb->ddb$t_name,&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count,0);
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
	sts = sys$qiow(0,chan,IO$_READLBLK,&iosb,0,0,(char *) &home,sizeof(struct ext2_super_block),hba,root_device_name[3]-48,0,0);
	//	printk("aft qio %x\n",sts);
	if (!(sts & 1)) break;
#if 0
	if (hba == VMSLONG(home.hm2$l_homelbn) &&
	    memcmp(home.hm2$t_format,"DECFILE11B  ",12) == 0) break;
#endif
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

struct readdir_callback2 {
  struct dirent64 * dirent;
  int count;
};

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
  bcopy(name, dirent->d_name, namlen);
  dirent->d_name[namlen]=0;
  return 0;
}

unsigned exttwo_access(struct _vcb * vcb, struct _irp * irp)
{
  struct _iosb iosb;
  unsigned sts=SS$_NORMAL;
  unsigned wrtflg=1;
  struct _fcb *fcb;
  //  struct _fh2 *head;
  char name[256], *tmpname;
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

  if (fib->fib$w_fid_num==4 && fib->fib$w_fid_seq==4 && fib->fib$w_did_num==0 && fib->fib$w_did_seq==0)
    memset(&x2p->context_save,0,54);


  if (fib->fib$w_did_num) {
    int wildcard=0;
    struct readdir_callback2 buf;
    int fd=0;
    struct file * f;
    signed int error=0;
    int dirflg=0;
    struct dirent64 dir;
    struct inode * head;
    struct _fcb * fcb=x2p->primary_fcb;
    head = fcb->fcb$l_primfcb;

    if (strchr(filedsc->dsc$a_pointer,'*') || strchr(filedsc->dsc$a_pointer,'%'))
      wildcard=1;

    memset(name,0,256);

    if (wildcard || (fib->fib$w_nmctl & FIB$M_WILD)) {
        fib->fib$l_wcc = 1/*curblk*/;
	strcpy(name,&x2p->context_save);
	wild=1;
    } else {
        fib->fib$l_wcc = 0;
	strcpy(name,&x2p->context_save);
	name[strlen(name)]='/';
	ext2_vms_to_unix(name+strlen(name),filedsc);
	strcpy(&x2p->context_save,name);
	if (strstr(filedsc->dsc$a_pointer,".DIR")) {
	  dirflg=O_DIRECTORY;
	}
    }

    dir.d_ino=0;
    dir.d_type=0;
    memset(dir.d_name,0,256);

    f=filp_open(name, O_RDONLY|O_NONBLOCK|O_LARGEFILE|dirflg, 0);
    buf.count = 0;
    buf.dirent = &dir;
    if (IS_ERR(f)) {
      sts=SS$_NOSUCHFILE;
      if ( (sts & 1) == 0) { 
	memset(&x2p->context_save,0,54);
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

    if (dir.d_ino==0) {
      dir.d_ino=f->f_dentry->d_inode->i_ino;
      strcpy(dir.d_name,f->f_dentry->d_iname);
    }

    filp_close(f,0);
    
    if (0==strcmp(name,"/") && 0==strcmp(dir.d_name,"."))
      strcpy(dir.d_name,"000000.DIR");
    if (0==strcmp(dir.d_name,".."))
      strcpy(dir.d_name,"DOTDOT.DIR");
    if (0==strcmp(dir.d_name,"."))
      strcpy(dir.d_name,"DOT.DIR");
    *reslen=strlen(dir.d_name);
    dir.d_name[(*reslen)++]=';';
    dir.d_name[(*reslen)++]='1';
    bcopy(dir.d_name,resdsc->dsc$a_pointer,*reslen);

    if (dir.d_ino!=2) {
      fib->fib$w_fid_num=head->i_dev;
      *(unsigned long*)(&fib->fib$w_fid_seq)=dir.d_ino;
    } else {
      fib->fib$w_fid_num=4;
      fib->fib$w_fid_seq=4;
      fib->fib$b_fid_rvn=0;
      fib->fib$b_fid_nmx=0;
    }

    //temp hack to get a dentry
    {
      char * c=strchr(&dir.d_name,';');
      *c=0;
      f=filp_open(&dir.d_name, O_RDONLY,0);
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
      dir.d_name[strlen(dir.d_name)]='/';
      strcpy(dir.d_name+strlen(dir.d_name),name);
      f=filp_open(&dir.d_name, O_RDONLY,0);
      if (f>=0 && f<0xf0000000) {
	if (f->f_dentry && f->f_dentry->d_inode)
	  make_fcb(f->f_dentry->d_inode);
	filp_close(f,0);
      }
    }

#if 0
      sts = search_ent(fcb,fibdsc,filedsc,reslen,resdsc,eofblk,action);
      // readdir should be replaced with something like search_ent
#endif
  }

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
  if (fcb == NULL) { iosbret(irp,sts); return sts; }

  x2p->primary_fcb=fcb;
  x2p->current_window=&fcb->fcb$l_wlfl;

  if (atrp) exttwo_read_attrib(fcb,fcb->fcb$l_primfcb,atrp);
  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}

int exttwo_read_writevb(struct _irp * i) {
  int lbn;
  char * buffer;
  struct _iosb iosb;
  struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
  struct _fcb * fcb = x2p->primary_fcb; // ???? is this right
  struct inode * inode = fcb->fcb$l_primfcb;
  int factor = vms_block_factor(inode->i_blkbits);
  struct _wcb * wcb = &fcb->fcb$l_wlfl;
  int blocks=(i->irp$l_qio_p2+511)/512;
  int e2bn=(i->irp$l_qio_p3-1)/factor;
  int rest=(i->irp$l_qio_p3-1)&(factor-1);
  lbn=f11b_map_vbn(e2bn,wcb);
  if (i->irp$v_fcode==IO$_WRITEVBLK) {
    panic("tried to do an unfished write\n");
    exttwo_write_block(vcb,i->irp$l_qio_p1,lbn,blocks,&iosb);
  } else {
    buffer=exttwo_read_block(vcb,lbn*factor+rest,blocks,&iosb);
    //memcpy(i->irp$l_qio_p1,buffer+512*rest,512);
    memcpy(i->irp$l_qio_p1,buffer,512);
    vfree(buffer);
  }
  //exttwo_io_done(i);
}

void * exttwo_read_block(struct _vcb * vcb, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned char * buf = vmalloc(512*count);
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(buf[0]=0,x2p->io_channel,IO$_READLBLK,&myiosb,0,0,buf,512*count,phyblk,0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}

void * exttwo_write_block(struct _vcb * vcb, unsigned char * buf, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(0,x2p->io_channel,IO$_WRITELBLK,&myiosb,0,0,buf,512*count,phyblk,0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}

void exttwo_read_attrib(struct _fcb * fcb,struct inode * inode, struct _atrdef * atrp) {
  struct _iosb iosb;
  int sts=SS$_NORMAL;
  struct inode * head=inode;

  while(atrp->atr$w_type!=0) {
    switch (atrp->atr$w_type) {
    case ATR$C_RECATTR:
      {
	struct _fatdef * f=atrp->atr$l_addr;
	f->fat$b_rtype=FAT$C_SEQUENTIAL << 4;
	f->fat$b_rattrib=0;
	f->fat$w_rsize=0;
	f->fat$l_hiblk=VMSSWAP((1+head->i_blocks));
	f->fat$l_efblk=VMSSWAP((1+head->i_size/512));
	f->fat$w_ffbyte=head->i_size%512;
	f->fat$b_bktsize=0;
	f->fat$b_vfcsize=0;
	f->fat$w_maxrec=0;
	f->fat$w_defext=0;
	f->fat$w_gbc=0;
	f->fat$w_versions=1;
	//memcpy(atrp->atr$l_addr,&head->fh2$w_recattr,atrp->atr$w_size);
      }
      break;
    case ATR$C_STATBLK:
      {
	struct _sbkdef * s=atrp->atr$l_addr;
	s->sbk$l_fcb=fcb;
      }
      break;
#if 0
      //not now
      //change to ext2 inode later
    case ATR$C_HEADER:
      {
	struct _fh2 * head;
	head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);  
	sts=iosb.iosb$w_status;
	memcpy(atrp->atr$l_addr,head,atrp->atr$w_size);
	vfree(head); // wow. freeing something
      }
      break;
#endif
    case ATR$C_CREDATE:
      {
	*(unsigned long long*)atrp->atr$l_addr=unix_to_vms_time(head->i_ctime);
      }  
	break;
    case ATR$C_REVDATE:
      {
	*(unsigned long long*)atrp->atr$l_addr=unix_to_vms_time(head->i_mtime);
      }  
	break;
    case ATR$C_EXPDATE:
      {
	*(unsigned long long*)atrp->atr$l_addr=0;
      }  
	break;
    case ATR$C_BAKDATE:
      {
	*(unsigned long long*)atrp->atr$l_addr=0;
      }  
	break;
	
    case ATR$C_UIC:
	memcpy(atrp->atr$l_addr,&head->i_uid,atrp->atr$w_size);
	break;

    case ATR$C_FPRO:
	memcpy(atrp->atr$l_addr,&head->i_mode,atrp->atr$w_size);
	break;

    default:
      printk("atr %x not supported\n",atrp->atr$w_type);
      break;
    }
    atrp++;
  }
  //  vfree(head);
}

void *ext2_fcb_create(struct inode * inode,unsigned *retsts)
{
  struct _vcb * vcb=x2p->current_vcb;
  struct _fcb *fcb4;
  struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  bzero(fcb,sizeof(struct _fcb));
  if (fcb == NULL) {
    if (retsts) *retsts = SS$_INSFMEM;
    return;
  } 
  fcb->fcb$b_type=DYN$C_FCB;
  qhead_init(&fcb->fcb$l_wlfl);

  fcb->fcb$w_fid_dirnum=inode->i_dev;
  //fcb->fcb$l_fid_recnum=inode->i_ino; // bugged
  fcb->fcb$w_fid [1] = inode->i_ino & 0xffff;
  fcb->fcb$w_fid [2] = inode->i_ino >> 16;

  insque(fcb,&vcb->vcb$l_fcbfl);

  fcb->fcb$l_efblk = inode -> i_blocks; //?
  fcb->fcb$l_filesize = inode -> i_size; //?

  ext2_wcb_create_all(fcb,inode);

  //  fcb->fcb$l_hdlbn=f11b_map_idxvbn(vcb,head->fh2$w_fid.fid$w_num + (head->fh2$w_fid.fid$b_nmx << 16) - 1 + VMSWORD(vcb->vcb$l_ibmapvbn) + VMSWORD(vcb->vcb$l_ibmapsize));

  fcb->fcb$l_primfcb = inode; // put this somewhere?

  *retsts=SS$_NORMAL;
  if (inode->i_ino!=2) return fcb;

  fcb4 = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  bcopy(fcb,fcb4,sizeof(struct _fcb));
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

void * exttwo_search_fcb(struct _vcb * vcb,struct inode * inode)
{
    struct _fcb * head = &vcb->vcb$l_fcbfl;
    struct _fcb * tmp = head->fcb$l_fcbfl;
    while(tmp!=head) {
      if (tmp->fcb$w_fid[1]==(inode->i_ino&0xffff) && tmp->fcb$w_fid[2]==(inode->i_ino>>16) && tmp->fcb$w_fid_dirnum==inode->i_dev) return tmp;
      tmp=tmp->fcb$l_fcbfl;
    }
    return 0;
}

void make_fcb(struct inode * inode) {
  int sts;
  void * fcb;
  if (inode==0) return;
  if (inode->i_sb->s_magic!=EXT2_SUPER_MAGIC) return;
  fcb=exttwo_search_fcb(x2p->current_vcb,inode);
  if (fcb==0)
    ext2_fcb_create(inode,&sts);
}

struct _fcb * e2_search_fcb(void * v) {
  if (x2p->current_vcb)
    return exttwo_search_fcb(x2p->current_vcb,v);
  else
    return 0;
}

signed long e2_map_vbn(struct _fcb * fcb, signed long vbn) {
  return f11b_map_vbn(vbn,&fcb->fcb$l_wlfl);
}

int e2_fcb_wcb_add_one(struct _fcb * fcb,signed long vbn,signed long result)
{
  int retsts;
  struct _wcb * wcb;
  struct _wcb * head=&fcb->fcb$l_wlfl;
  struct _wcb * tmp=head->wcb$l_wlfl;
  while (tmp!=head) {
    if (vbn>=tmp->wcb$l_stvbn && vbn<=(tmp->wcb$l_stvbn+tmp->wcb$l_p1_count)) {
      tmp->wcb$l_p1_count++;
      return SS$_NORMAL;
    }
    tmp=tmp->wcb$l_wlfl;
  }

  wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb),GFP_KERNEL);
  bzero(wcb,sizeof(struct _wcb));
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

char * ext2_vms_to_unix(char * name,struct dsc$descriptor * dsc) {
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

void ext2_translate_fid (struct _vcb * vcb, struct _fiddef * fid) {
  if (fid->fid$w_num==4 && fid->fid$w_seq==4 && fid->fid$w_rvn==0) {
    fid->fid$w_num=vcb->vcb$l_fcbfl->fcb$w_fid_dirnum; // i_dev
    fid->fid$w_seq=2; // EXT2 root ino
    fid->fid$w_rvn=0; // EXT2 root ino
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
  sts = sys$qiow(0,getchan(x2p->current_vcb),IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		 &fibdsc,&fildsc,&reslen,&resdsc,0,0);
  sts = iosb.iosb$w_status;

  fibblk.fib$w_did_num = fibblk.fib$w_fid_num;
  fibblk.fib$w_did_seq = fibblk.fib$w_fid_seq;
  fibblk.fib$w_fid_num = 0;
  fibblk.fib$w_fid_seq = 0;
  sts = sys$qiow(0,getchan(x2p->current_vcb),IO$_ACCESS,&iosb,0,0,
		 &fibdsc,&fildsc,&reslen,&resdsc,0,0);
  sts = iosb.iosb$w_status;
  return sts;
}

unsigned long get_prim_fcb() {
  return 	  x2p->primary_fcb;
}
