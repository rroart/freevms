// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

#include<linux/vmalloc.h>
#include<linux/linkage.h>

//#include <stdio.h>
#include <linux/string.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <fatdef.h>
#include <ucbdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <iosbdef.h>
#include <irpdef.h>
#include <rmsdef.h>
#include <scbdef.h>
#include <wcbdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <fh2def.h>
#include <fi2def.h>
#include <hm2def.h>
#include <fcbdef.h>
#include <vmstime.h>

#include "../../f11x/src/xqp.h"

//#include "ssdef.h"
#include "cache.h"
#include "access.h"

unsigned deaccesshead(struct _fh2 *head,unsigned idxblk);
unsigned accesshead(struct _vcb *vcb,struct _fiddef *fid,unsigned seg_num,
                    struct _fh2 **headbuff,
                    unsigned *retidxblk,unsigned wrtflg);
unsigned getwindow(struct _fcb * fcb,unsigned vbn,struct _vcb **devptr,
                   unsigned *phyblk,unsigned *phylen,struct _fiddef *hdrfid,
                   unsigned *hdrseq);
struct _vcb *rvn_to_dev(struct _vcb *vcb,unsigned rvn);

unsigned f11b_extend(struct _fcb *fcb,unsigned blocks,unsigned contig);


/* Bitmaps get accesses in 'WORK_UNITs' which can be an integer
   on a little endian machine but must be a byte on a big endian system */

#ifdef FREEVMS_BIG_ENDIAN
#define WORK_UNIT unsigned char
#define WORK_MASK 0xff
#else
#define WORK_UNIT unsigned int
#define WORK_MASK 0xffffffff
#endif
#define WORK_BITS (sizeof(WORK_UNIT) * 8)

/* update_freecount() to read the device cluster bitmap and compute
   the number of un-used clusters */

unsigned update_freecount(struct _vcb *vcbdev,unsigned *retcount)
{
    unsigned sts;
    unsigned free_clusters = 0;
    unsigned map_block, map_end;
    struct _scbdef * scb;
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1,0);
    map_end = ((scb->scb$l_volsize/scb->scb$w_cluster) + 4095) / 4096 + 2;
    for (map_block = 2; map_block < map_end; ) {
        unsigned blkcount;
        WORK_UNIT *bitmap,*work_ptr;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,0,0);
        if (!(sts & 1)) return sts;
        if (blkcount > map_end - map_block) blkcount = map_end - map_block + 1;
        work_ptr = bitmap;
        work_count = blkcount * 512 / sizeof(WORK_UNIT);
        do {
            WORK_UNIT work_val = *work_ptr++;
            if (work_val == WORK_MASK) {
                free_clusters += WORK_BITS;
            } else {
                while (work_val != 0) {
                    if (work_val & 1) free_clusters++;
                    work_val = work_val >> 1;
                }
            }
        } while (--work_count > 0);
        sts = deaccesschunk(0,0,1);
        if (!(sts & 1)) return sts;
        map_block += blkcount;
    }
    *retcount = free_clusters;
    return sts;
}

/* bitmap_modify() will either set or release a block of bits in the
   device cluster bitmap */

unsigned bitmap_modify(struct _vcb *vcbdev,unsigned cluster,unsigned count,
                       unsigned release_flag)
{
    unsigned sts;
    unsigned clust_count = count;
    unsigned map_block = cluster / 4096 + 2;
    unsigned block_offset = cluster % 4096;
    struct _scbdef * scb;
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1,0);
    if (clust_count < 1) return SS$_BADPARAM;
    if (cluster + clust_count > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) return SS$_BADPARAM;
    do {
        unsigned blkcount;
        WORK_UNIT *bitmap;
        WORK_UNIT *work_ptr;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,1,0);
        if (!(sts & 1)) return sts;
        work_ptr = bitmap + block_offset / WORK_BITS;
        if (block_offset % WORK_BITS) {
            unsigned bit_no = block_offset % WORK_BITS;
            WORK_UNIT bit_mask = WORK_MASK;
            if (bit_no + clust_count < WORK_BITS) {
                bit_mask = bit_mask >> (WORK_BITS - clust_count);
                clust_count = 0;
            } else {
                clust_count -= WORK_BITS - bit_no;
            }
            bit_mask = bit_mask << bit_no;
            if (release_flag) {
                *work_ptr++ |= bit_mask;
            } else {
                *work_ptr++ &= ~bit_mask;
            }
            block_offset += WORK_BITS - bit_no;
        }
        work_count = (blkcount * 4096 - block_offset) / WORK_BITS;
        if (work_count > clust_count / WORK_BITS) {
            work_count = clust_count / WORK_BITS;
            block_offset = 1;
        } else {
            block_offset = 0;
        }
        clust_count -= work_count * WORK_BITS;
        if (release_flag) {
            while (clust_count-- > 0) {
                *work_ptr++ = WORK_MASK;
            }
        } else {
            while (work_count-- > 0) {
                *work_ptr++ = 0;
            }
        }
        if (clust_count != 0 && block_offset) {
            WORK_UNIT bit_mask = WORK_MASK >> (WORK_BITS - clust_count);
            if (release_flag) {
                *work_ptr++ |= bit_mask;
            } else {
                *work_ptr++ &= ~bit_mask;
            }
            clust_count = 0;
        }
        sts = deaccesschunk(map_block,blkcount,1);
	sts = writechunk(getmapfcb(vcbdev), map_block, bitmap);
        if (!(sts & 1)) return sts;
        map_block += blkcount;
        block_offset = 0;
    } while (clust_count != 0);
    return sts;
}

/* bitmap_search() is a routine to find a pool of free clusters in the
   device cluster bitmap */

unsigned bitmap_search(struct _vcb *vcbdev,unsigned *position,unsigned *count)
{
    unsigned sts;
    unsigned map_block,block_offset;
    unsigned search_words,needed;
    unsigned run = 0,cluster,ret_cluster, first_bit=0;
    unsigned best_run = 0,best_cluster = 0;
    struct _scbdef * scb;
    needed = *count;
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1,0);
    if (needed < 1 || needed > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) return SS$_BADPARAM;
    cluster = *position;
    if (cluster + *count > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) cluster = 0;
    map_block = cluster / 4096 + 2;
    block_offset = cluster % 4096;
    cluster = cluster - (cluster % WORK_BITS);
    ret_cluster = cluster;
    search_words = (scb->scb$l_volsize/scb->scb$w_cluster) / WORK_BITS;
    do {
        unsigned blkcount;
        WORK_UNIT *bitmap;
        WORK_UNIT *work_ptr, work_val;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,1,0);
        if ((sts & 1) == 0) return sts;
        work_ptr = bitmap + block_offset / WORK_BITS;
        work_val = *work_ptr++;
        if (block_offset % WORK_BITS) {
            work_val = work_val & (WORK_MASK << block_offset % WORK_BITS);
        }        
        work_count = (blkcount * 4096 - block_offset) / WORK_BITS;
        if (work_count > search_words) work_count = search_words;
        search_words -= work_count;
        do {
	    unsigned bit_no = 0;
            if (work_val == WORK_MASK) {
                run += WORK_BITS;
		if (run > best_run) {
		  best_run = run;
		  best_cluster = cluster + bit_no;
		}
		if (best_run >= needed)
		  goto out_of_here;
            } else {
                while (work_val != 0) {
                    if (work_val & 1) {
                        run++;
                        if (run > best_run) {
                            best_run = run;
                            best_cluster = cluster + bit_no;
                        }
			if (first_bit==0)
			  first_bit = bit_no;
			if (best_run >= needed)
			  goto out_of_here;
                    } else {
                        if (run > best_run) {
                            best_run = run;
                            best_cluster = cluster + bit_no;
                        }
                        run = 0;
			ret_cluster = cluster;
			first_bit = bit_no+1;
                    }
                    work_val = work_val >> 1;
                    bit_no++;
                }
		if (bit_no < WORK_BITS) {
		  if (run > best_run) {
		    best_run = run;
		    best_cluster = cluster + bit_no;
		  }
		  run = 0;
		  ret_cluster = cluster;
		  first_bit = bit_no;
                }
            }
            cluster += WORK_BITS;
	    if (run==0)
	      ret_cluster=cluster;
            if (work_count-- > 0) {
                work_val = *work_ptr++;
            } else {
                break;
            }
        } while (best_run < needed);
        deaccesschunk(map_block,0,1);
	writechunk(getmapfcb(vcbdev),map_block,bitmap);
        if ((sts & 1) == 0) break;
        map_block += blkcount;
        block_offset = 0;
    } while (best_run < needed && search_words != 0);
 out_of_here:
    best_cluster = ret_cluster + first_bit;
    if (best_run > needed) best_run = needed;
    *count = best_run;
    *position = best_cluster;
    return sts;
}

/* headmap_clear() will release a header from the indexf.sys file header
   bitmap */

unsigned headmap_clear(struct _vcb *vcbdev,unsigned head_no)
{
    WORK_UNIT *bitmap;
    unsigned sts;
    unsigned map_block;

    map_block = head_no / 4096 + vcbdev->vcb$l_cluster * 4 + 1;
    if (head_no < 10) return 0;
    sts = accesschunk(getidxfcb(vcbdev),map_block,(char **) &bitmap,NULL,1,0);
    if (sts & 1) {
        bitmap[(head_no % 4096) / WORK_BITS] &= ~(1 << (head_no % WORK_BITS));
        sts = deaccesschunk(map_block,1,1);
	writechunk(getidxfcb(vcbdev),map_block,bitmap);
    }
    return sts;
}

/* update_findhead() will locate a free header from indexf.sys */

unsigned update_findhead(struct _vcb *vcbdev,unsigned *rethead_no,
                         struct _fh2 **headbuff,
                         unsigned *retidxblk)
{
    unsigned head_no = 0;
    unsigned sts;
    do {
        int modify_flag = 0;
        unsigned blkcount;
        WORK_UNIT *bitmap,*work_ptr;
        unsigned map_block,work_count;
        map_block = head_no / 4096 + vcbdev->vcb$l_cluster * 4 + 1;
        sts = accesschunk(getidxfcb(vcbdev),map_block,
            (char **) &bitmap,&blkcount,1,0);
        if ((sts & 1) == 0) return sts;
        work_count = (head_no % 4096) / WORK_BITS;
        work_ptr = bitmap + work_count;
        work_count = blkcount * 512 / WORK_BITS - work_count;
        do {
            WORK_UNIT work_val = *work_ptr;
            if (work_val == WORK_MASK) {
                head_no += WORK_BITS;
            } else {
                unsigned bit_no = 0;
                for (bit_no = 0; bit_no < WORK_BITS; bit_no++) {
                    if ((work_val & (1 << bit_no)) == 0) {
                        unsigned idxblk = head_no +
                            VMSWORD(vcbdev->vcb$l_ibmapvbn) +
                            VMSWORD(vcbdev->vcb$l_ibmapsize);
                        sts = accesschunk(getidxfcb(vcbdev),idxblk,(char **) headbuff,NULL,1,0);
                        if (sts & 1) {
                            *work_ptr |= 1 << bit_no;
                            modify_flag = 1;
                            if ((*headbuff)->fh2$w_checksum != 0 && (*headbuff)->fh2$w_fid.fid$w_num != 0 &&
                                (VMSLONG((*headbuff)->fh2$l_filechar) & FH2$M_MARKDEL) == 0) {
                                sts = deaccesschunk(0,0,0);
				writechunk(getidxfcb(vcbdev),idxblk,*headbuff);
                            } else {
			      signed int newvbn;
                                *rethead_no = head_no + 1;
                                *retidxblk = idxblk;
                                deaccesschunk(map_block,blkcount,modify_flag);
				writechunk(getidxfcb(vcbdev),map_block,bitmap);
				newvbn=f11b_map_idxvbn(vcbdev,idxblk);
				if (newvbn==-1) {
				  sts=f11b_extend(getidxfcb(vcbdev),1,1);
				}
                                return SS$_NORMAL;
                            }
                        }
                    }
                    head_no++;
                }
            }
            work_ptr++;
        } while (--work_count != 0);
        deaccesschunk(map_block,blkcount,modify_flag);
	writechunk(getidxfcb(vcbdev),map_block,bitmap);
        if ((sts & 1) == 0) break;
    } while (head_no < VMSLONG(vcbdev->vcb$l_maxfiles));
    return sts;
}

unsigned update_addhead(struct _vcb *vcb,char *filename,struct _fiddef *back,
                     unsigned seg_num,struct _fiddef *fid,
                     struct _fh2 **rethead,
                     unsigned *idxblk)
{
    unsigned free_space = 0;
    unsigned device,rvn,sts;
    unsigned head_no;
    struct _fi2 *id;
    struct _fh2 *head;
    struct _vcb *vcbdev = vcb;

    free_space = vcbdev->vcb$l_free;
    rvn = 0;

    if (vcbdev == NULL) return SS$_DEVICEFULL;

    sts = update_findhead(vcbdev,&head_no,&head,idxblk);
    if (!(sts & 1)) return sts;
    printk("Header %d index %d rvn %d\n",head_no,*idxblk,rvn);
    fid->fid$w_num = head_no;
    fid->fid$w_seq = ++head->fh2$w_fid.fid$w_seq;
    if (fid->fid$w_seq == 0) fid->fid$w_seq = 1;
    if (rvn > 0) {
        fid->fid$b_rvn = rvn + 1;
    } else {
        fid->fid$b_rvn = 0;
    }
    fid->fid$b_nmx = head_no >> 16;
    memset(head,0,512);
    head->fh2$b_idoffset = 40;
    head->fh2$b_mpoffset = 100;
    head->fh2$b_acoffset = 255;
    head->fh2$b_rsoffset = 255;
    head->fh2$w_seg_num = seg_num;
    head->fh2$w_struclev = 513;
    head->fh2$l_fileowner.uic$w_mem = 4;
    head->fh2$l_fileowner.uic$w_grp = 1;
    fid_copy(&head->fh2$w_fid,fid,0);
    if (back != NULL) fid_copy(&head->fh2$w_backlink,back,0);
    id = (struct _fi2 *) ((unsigned short *) head + 40);
    memset(id->fi2$t_filenamext,' ',66);
    if (strlen(filename) < 20) {
        memset(id->fi2$t_filename,' ',20);
        memcpy(id->fi2$t_filename,filename,strlen(filename));
    } else {
        memcpy(id->fi2$t_filename,filename,20);
        memcpy(id->fi2$t_filenamext,filename+20,strlen(filename+20));
    }
    id->fi2$w_revision = 1;
    exe$gettim(&id->fi2$q_credate);
    memcpy(id->fi2$q_revdate,id->fi2$q_credate,sizeof(id->fi2$q_credate));
    memcpy(id->fi2$q_expdate,id->fi2$q_credate,sizeof(id->fi2$q_credate));
    head->fh2$w_recattr.fat$l_efblk = VMSSWAP(1);
    {
      unsigned short check = checksum((vmsword *) head);
      head->fh2$w_checksum = VMSWORD(check);
    }
    writechunk(getidxfcb(vcbdev),*idxblk,head);
    *rethead=head;
    return 1;
}

/* update_create() will create a new file... */

unsigned f11b_create(struct _vcb *vcb,struct _irp * i)
{
  struct dsc$descriptor * fibdsc=i->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=i->irp$l_qio_p2;
  struct _fibdef * fib=fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&fib->fib$w_fid_num;
  struct _fiddef * did=&fib->fib$w_did_num;
  char *filename=filedsc->dsc$a_pointer;
  struct _fh2 *head;
  unsigned idxblk;
  unsigned sts;
  struct _fcb *fcb;
  struct _iosb iosb;
  sts = update_addhead(vcb,filename,did,0,fid,&head,&idxblk);
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
    sts = f11b_access(vcb,i); // should not be, but can not implement otherwise for a while
    if ( (sts & 1) == 0) { iosbret(i,sts); return sts; }
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
      fcb=fcb_create2(head,&sts);
    }

    if (fcb == NULL) { iosbret(i,sts); return sts; }

    xqp->primary_fcb=fcb;
    xqp->current_window=&fcb->fcb$l_wlfl;
#endif

  }

  xqp->primary_fcb=f11b_search_fcb(xqp->current_vcb,&fib->fib$w_fid_num);

  if ((fib->fib$w_exctl&FIB$M_EXTEND) && (sts & 1)) {
    struct _fcb * newfcb;
    newfcb=f11b_search_fcb(xqp->current_vcb,&fib->fib$w_fid_num);
    sts = f11b_extend(newfcb,fib->fib$l_exsz,0);
  }

  struct _fatdef * fat = ((long *)i->irp$l_qio_p5)[1];
  fat->fat$l_efblk = VMSSWAP(1); // so this won't be changed to 0

  struct _fcb * newfcb=f11b_search_fcb(xqp->current_vcb,&fib->fib$w_fid_num);
  f11b_write_attrib(newfcb, i->irp$l_qio_p5);

  printk("(%d,%d,%d) %d\n",fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn,sts);
  return sts;
}

unsigned f11b_extend(struct _fcb *fcb,unsigned blocks,unsigned contig)
{
  struct _iosb iosb;
  unsigned sts;
  struct _vcb *vcbdev;
  struct _fh2 *head;
  unsigned headvbn;
  struct _fiddef hdrfid;
  unsigned hdrseq;
  unsigned start_pos = 0;
  unsigned block_count = blocks;
  if (block_count < 1) return 0;
  if (fcb->fcb$l_efblk > 0) {
    unsigned mapblk,maplen;
    sts = getwindow(fcb,fcb->fcb$l_efblk,&vcbdev,&mapblk,&maplen,&hdrfid,&hdrseq);
    if ((sts & 1) == 0) return sts;
    start_pos = mapblk + 1;
    if (hdrseq != 0) {
      head = f11b_read_header(xqp->current_vcb,&hdrfid,fcb,&iosb);
      sts=iosb.iosb$w_status;
      if ((sts & 1) == 0) return sts;
    } else {
      head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);
      sts=iosb.iosb$w_status;
    }
  } else {
    head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);
    sts=iosb.iosb$w_status;
    start_pos = 0;          /* filenum * 3 /indexfsize * volumesize; */
  }
  vcbdev = rvn_to_dev(xqp->current_vcb,fcb->fcb$b_fid_rvn);
  if (vcbdev->vcb$l_free == 0 || head->fh2$b_map_inuse + 4 >=
      head->fh2$b_acoffset - head->fh2$b_mpoffset) {
    struct _fh2 *nhead;
    unsigned nidxblk;
    sts = update_addhead(xqp->current_vcb,"",&head->fh2$w_fid,head->fh2$w_seg_num+1,
			 &head->fh2$w_ext_fid,&nhead,&nidxblk);
    if (!(sts & 1)) return sts;
    deaccesshead(head,headvbn);
    writehead(getidxfcb(vcbdev,head));
    head = nhead;
    headvbn = nidxblk;
    vcbdev = rvn_to_dev(xqp->current_vcb,head->fh2$w_fid.fid$b_rvn);
  }
  sts = bitmap_search(vcbdev,&start_pos,&block_count);
  printk("Update_extend %d %d\n",start_pos,block_count);
  if (sts & 1) {
    if (block_count < 1 || (contig && (block_count * vcbdev->vcb$l_cluster) < blocks)) {
      sts = SS$_DEVICEFULL;
    } else {
      unsigned short *mp,*map;
      map = mp = (unsigned short *) head + head->fh2$b_mpoffset + head->fh2$b_map_inuse;
      *mp++ = (3 << 14) | ((block_count *vcbdev->vcb$l_cluster - 1) >> 16);
      *mp++ = (block_count * vcbdev->vcb$l_cluster - 1) & 0xffff;
      *mp++ = (start_pos * vcbdev->vcb$l_cluster) & 0xffff;
      *mp++ = (start_pos * vcbdev->vcb$l_cluster) >> 16;
      head->fh2$b_map_inuse += 4;
      add_wcb(fcb,map);
      fcb->fcb$l_efblk += block_count * vcbdev->vcb$l_cluster;
      //head2 = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);
      //sts=iosb.iosb$w_status;
      head->fh2$w_recattr.fat$l_hiblk = VMSSWAP(fcb->fcb$l_efblk * vcbdev->vcb$l_cluster);
      writehead(fcb,head);
      sts = bitmap_modify(vcbdev,start_pos,block_count,0);
    }
  }
  deaccesshead(head,headvbn);
  writehead(getidxfcb(vcbdev),head);
  return sts;
}




/* This routine has bugs and does NOT work properly yet!!!!
It may be something simple but I haven't had time to look...
So DON'T use mount/write!!!  */

unsigned deallocfile(struct _fcb *fcb)
{ 
  struct _iosb iosb;
    unsigned sts = 1;
    /*
    First mark all file clusters as free in BITMAP.SYS
    */
    struct _fh2 * head;
    unsigned vbn = 1;
    while (vbn <= fcb->fcb$l_efblk) {
        unsigned sts;
        unsigned phyblk,phylen;
        struct _vcb *vcbdev;
        sts = getwindow(fcb,vbn,&vcbdev,&phyblk,&phylen,NULL,NULL);
        if ((sts & 1) == 0) break;

        sts = bitmap_modify(vcbdev,phyblk,phylen,1);
        if ((sts & 1) == 0) break;
    }
    /*
    Now reset file header bit map in INDEXF.SYS and
    update each of the file headers...
    */
    {
        unsigned rvn = fcb->fcb$b_fid_rvn;
        unsigned headvbn = 0;

        struct _fh2 *head;
	head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);
	sts=iosb.iosb$w_status;
        do {
            unsigned ext_seg_num = 0;
            struct _fiddef extfid;
            struct _vcb *vcbdev;
            unsigned *bitmap;
            unsigned filenum = (head->fh2$w_fid.fid$b_nmx << 16) +
                head->fh2$w_fid.fid$w_num - 1;
            unsigned idxblk;
            vcbdev = rvn_to_dev(xqp->current_vcb,rvn);
            if (vcbdev == NULL) break;
            idxblk = filenum / 4096 +
                vcbdev->vcb$l_cluster * 4 + 1;
            sts = accesschunk(getidxfcb(vcbdev),idxblk,
                (char **) &bitmap,NULL,1,0);
            if (sts & 1) {
                bitmap[(filenum % 4096) / WORK_BITS] &=
                    ~(1 << (filenum % WORK_BITS));
                sts = deaccesschunk(idxblk,1,1);
		sts = writechunk(getidxfcb(vcbdev), idxblk, bitmap);
            } else {
                break;
            }
            head->fh2$w_fid.fid$w_num = 0;
            head->fh2$w_fid.fid$b_rvn = 0;
            head->fh2$w_fid.fid$b_nmx = 0;
            head->fh2$w_checksum = 0;
            ext_seg_num++;
            memcpy(&extfid,&head->fh2$w_ext_fid,sizeof(struct _fiddef));
            sts = deaccesshead(NULL,headvbn);
	    sts = writehead(fcb,head);
            if ((sts & 1) == 0) break;
            if (extfid.fid$b_rvn == 0) {
                extfid.fid$b_rvn = rvn;
            } else {
                rvn = extfid.fid$b_rvn;
            }
            if (extfid.fid$w_num != 0 || extfid.fid$b_nmx != 0) {
	      head = f11b_read_header(xqp->current_vcb,&extfid,fcb,&iosb);
	      sts=iosb.iosb$w_status;
	      if ((sts & 1) == 0) break;
            } else {
                break;
            }
        } while (1);
    }
    return sts;
}



/* accesserase: delete a file... */

unsigned f11b_delete(struct _vcb * vcb,struct _irp * irp)
{
  struct _iosb iosb;
  struct _fcb *fcb;
  struct dsc$descriptor * fibdsc=irp->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=irp->irp$l_qio_p2;
  unsigned short *reslen=irp->irp$l_qio_p3;
  struct dsc$descriptor * resdsc=irp->irp$l_qio_p4;
  struct _fibdef * fib=fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&((struct _fibdef *)fibdsc->dsc$a_pointer)->fib$w_fid_num;
  int sts=0;
  struct _fh2 *  head;
  unsigned action=1;

  if (xqp->primary_fcb) {
    struct _fcb * fcb = xqp->primary_fcb;
    if (fid->fid$w_num!=fcb->fcb$w_fid_num)
      xqp->primary_fcb=0; //f11b_search_fcb(vcb,fid);
  }

  if (fib->fib$w_did_num) {
    struct _fh2 * head;
    struct _fcb * fcb=xqp->primary_fcb;
    if (fcb==0)
      fcb=f11b_search_fcb(vcb,&fib->fib$w_did_num);
    head = f11b_read_header (vcb, 0, fcb, &iosb);  
    sts=iosb.iosb$w_status;
    if (VMSLONG(head->fh2$l_filechar) & FH2$M_DIRECTORY) {
      unsigned eofblk = VMSSWAP(head->fh2$w_recattr.fat$l_efblk);
      if (VMSWORD(head->fh2$w_recattr.fat$w_ffbyte) == 0) --eofblk;
      sts = search_ent(fcb,fibdsc,filedsc,reslen,resdsc,eofblk,action);
    } else {
      sts = SS$_BADIRECTORY;
    }
  }

  if ( (sts & 1) == 0) { iosbret(irp,sts); return sts; }

  fcb=f11b_search_fcb(vcb,fid);

  if (sts & 1) {
    head = f11b_read_header (xqp->current_vcb, fib, 0, &iosb);  
    sts=iosb.iosb$w_status;
    head->fh2$l_filechar |= FH2$M_MARKDEL;
    printk("Accesserase ... \n");
    sts = deaccessfile(fcb);
  }
  return sts;
}
