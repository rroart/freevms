// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

#include<linux/vmalloc.h>
#include<linux/linkage.h>

#include <stdio.h>
#include <linux/string.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <fatdef.h>
#include <ucbdef.h>
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



/* Bitmaps get accesses in 'WORK_UNITs' which can be an integer
   on a little endian machine but must be a byte on a big endian system */

#ifdef BIG_ENDIAN
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
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1);
    map_end = ((scb->scb$l_volsize/scb->scb$w_cluster) + 4095) / 4096 + 2;
    for (map_block = 2; map_block < map_end; ) {
        unsigned blkcount;
        WORK_UNIT *bitmap,*work_ptr;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,0);
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
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1);
    if (clust_count < 1) return SS$_BADPARAM;
    if (cluster + clust_count > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) return SS$_BADPARAM;
    do {
        unsigned blkcount;
        WORK_UNIT *bitmap;
        WORK_UNIT *work_ptr;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,1);
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
    unsigned run = 0,cluster;
    unsigned best_run = 0,best_cluster = 0;
    struct _scbdef * scb;
    needed = *count;
    sts = accesschunk(getmapfcb(vcbdev),1,(char **) &scb,0,1);
    if (needed < 1 || needed > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) return SS$_BADPARAM;
    cluster = *position;
    if (cluster + *count > (scb->scb$l_volsize/scb->scb$w_cluster) + 1) cluster = 0;
    map_block = cluster / 4096 + 2;
    block_offset = cluster % 4096;
    cluster = cluster - (cluster % WORK_BITS);
    search_words = (scb->scb$l_volsize/scb->scb$w_cluster) / WORK_BITS;
    do {
        unsigned blkcount;
        WORK_UNIT *bitmap;
        WORK_UNIT *work_ptr, work_val;
        unsigned work_count;
        sts = accesschunk(getmapfcb(vcbdev),map_block,(char **) &bitmap,&blkcount,1);
        if ((sts & 1) == 0) return sts;
        work_ptr = bitmap + block_offset / WORK_BITS;
        work_val = *work_ptr++;
        if (block_offset % WORK_BITS) {
            work_val = work_val && (WORK_MASK << block_offset % WORK_BITS);
        }        
        work_count = (blkcount * 4096 - block_offset) / WORK_BITS;
        if (work_count > search_words) work_count = search_words;
        search_words -= work_count;
        do {
            if (work_val == WORK_MASK) {
                run += WORK_BITS;
            } else {
                unsigned bit_no = 0;
                while (work_val != 0) {
                    if (work_val & 1) {
                        run++;
                    } else {
                        if (run > best_run) {
                            best_run = run;
                            best_cluster = cluster + bit_no;
                        }
                        run = 0;
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
                }
            }
            cluster += WORK_BITS;
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
    sts = accesschunk(getidxfcb(vcbdev),map_block,(char **) &bitmap,NULL,1);
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
            (char **) &bitmap,&blkcount,1);
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
                        sts = accesschunk(getidxfcb(vcbdev),idxblk,(char **) headbuff,NULL,1);
                        if (sts & 1) {
                            *work_ptr |= 1 << bit_no;
                            modify_flag = 1;
                            if ((*headbuff)->fh2$w_checksum != 0 || (*headbuff)->fh2$w_fid.fid$w_num != 0 ||
                                VMSLONG((*headbuff)->fh2$l_filechar) & FH2$M_MARKDEL == 0) {
                                sts = deaccesschunk(0,0,0);
				writechunk(getidxfcb(vcbdev),idxblk,*headbuff);
                            } else {
                                *rethead_no = head_no + 1;
                                *retidxblk = idxblk;
                                deaccesschunk(map_block,blkcount,modify_flag);
				writechunk(getidxfcb(vcbdev),map_block,bitmap);
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
    exe$gettim(id->fi2$q_credate);
    memcpy(id->fi2$q_revdate,id->fi2$q_credate,sizeof(id->fi2$q_credate));
    memcpy(id->fi2$q_expdate,id->fi2$q_credate,sizeof(id->fi2$q_credate));
    head->fh2$w_recattr.fat$l_efblk = VMSSWAP(1);
    {
        unsigned short check = checksum((vmsword *) head);
        head->fh2$w_checksum = VMSWORD(check);
    }
    writechunk(getidxfcb(vcbdev),idxblk,head);
    return 1;
}

/* update_create() will create a new file... */

unsigned update_create(struct _vcb *vcb,struct _fiddef *did,char *filename,
                       struct _fiddef *fid,struct _fcb **fcb)
{
    struct _fh2 *head;
    unsigned idxblk;
    unsigned sts;
    sts = update_addhead(vcb,filename,did,0,fid,&head,&idxblk);
    if (!(sts & 1)) return sts;
    sts = deaccesshead(head,idxblk);
    //    sts = writehead(getidxfcb(vcb),head);
    if (sts & 1 && fcb != NULL) {
        sts = accessfile(vcb,fid,fcb,1);
    }
    printk("(%d,%d,%d) %d\n",fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn,sts);
    return sts;
}

unsigned update_extend(struct _fcb *fcb,unsigned blocks,unsigned contig)
{
    unsigned sts;
    struct _vcb *vcbdev;
    struct _fh2 *head, *head2;
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
            sts = accesshead(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,&hdrfid,hdrseq,&head,&headvbn,1);
            if ((sts & 1) == 0) return sts;
        } else {
	  sts = gethead (fcb, &head);
        }
    } else {
      sts = gethead (fcb, &head);
      start_pos = 0;          /* filenum * 3 /indexfsize * volumesize; */
    }
    vcbdev = rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,fcb->fcb$b_fid_rvn);
    if (vcbdev->vcb$l_free == 0 || head->fh2$b_map_inuse + 4 >=
                head->fh2$b_acoffset - head->fh2$b_mpoffset) {
        struct _fh2 *nhead;
        unsigned nidxblk;
        sts = update_addhead(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,"",&head->fh2$w_fid,head->fh2$w_seg_num+1,
              &head->fh2$w_ext_fid,&nhead,&nidxblk);
        if (!(sts & 1)) return sts;
        deaccesshead(head,headvbn);
	writehead(getidxfcb(vcbdev,head));
        head = nhead;
        headvbn = nidxblk;
        vcbdev = rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,head->fh2$w_fid.fid$b_rvn);
    }
    sts = bitmap_search(vcbdev,&start_pos,&block_count);
    printk("Update_extend %d %d\n",start_pos,block_count);
    if (sts & 1) {
        if (block_count < 1 || contig && block_count * vcbdev->vcb$l_cluster < blocks) {
            sts = SS$_DEVICEFULL;
        } else {
            unsigned short *mp;
            mp = (unsigned short *) head + head->fh2$b_mpoffset + head->fh2$b_map_inuse;
            *mp++ = (3 << 14) | ((block_count *vcbdev->vcb$l_cluster - 1) >> 16);
            *mp++ = (block_count * vcbdev->vcb$l_cluster - 1) & 0xffff;
            *mp++ = (start_pos * vcbdev->vcb$l_cluster) & 0xffff;
            *mp++ = (start_pos * vcbdev->vcb$l_cluster) >> 16;
            head->fh2$b_map_inuse += 4;
            fcb->fcb$l_efblk += block_count * vcbdev->vcb$l_cluster;
	    sts = gethead (fcb, &head2);
            head2->fh2$w_recattr.fat$l_hiblk = VMSSWAP(fcb->fcb$l_efblk * vcbdev->vcb$l_cluster);
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
	sts = gethead (fcb, &head);
        do {
            unsigned ext_seg_num = 0;
            struct _fiddef extfid;
            struct _vcb *vcbdev;
            unsigned *bitmap;
            unsigned filenum = (head->fh2$w_fid.fid$b_nmx << 16) +
                head->fh2$w_fid.fid$w_num - 1;
            unsigned idxblk;
            vcbdev = rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,rvn);
            if (vcbdev == NULL) break;
            idxblk = filenum / 4096 +
                vcbdev->vcb$l_cluster * 4 + 1;
            sts = accesschunk(getidxfcb(vcbdev),idxblk,
                (char **) &bitmap,NULL,1);
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
                sts = accesshead(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,&extfid,ext_seg_num,&head,&headvbn,1);
                if ((sts & 1) == 0) break;
            } else {
                break;
            }
        } while (1);
    }
    return sts;
}



/* accesserase: delete a file... */

unsigned accesserase(struct _vcb * vcb,struct _fiddef * fid)
{
    struct _fcb *fcb;
    int sts;
    struct _fh2 *  head;
    sts = accessfile(vcb,fid,&fcb,1);
    if (sts & 1) {
      sts = gethead (fcb, &head);
      head->fh2$l_filechar |= FH2$M_MARKDEL;
      printk("Accesserase ... \n");
      sts = deaccessfile(fcb);
    }
    return sts;
}




#ifdef EXTEND
unsigned extend(struct _fcb *fcb,unsigned blocks)
{
    unsigned sts;
    struct _vcb *vcbdev;
    unsigned clusterno;
    unsigned extended = 0;
    if ((fcb->fcb$l_status & FCB_WRITE) == 0) return SS$_WRITLCK;
    if (fcb->fcb$l_efblk > 0) {
        unsigned phyblk,phylen;
        sts = getwindow(fcb,fcb->fcb$l_efblk,&vcbdev,&phyblk,&phylen,NULL,NULL);
        clusterno = (phyblk + 1) / vcbdev->vcb$l_cluster;
        if ((sts & 1) == 0) return sts;
    } else {
        vcbdev = fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb->vcbdev;
        clusterno = 0;          /* filenum * 3 /indexfsize * volumesize; */
    }
    while (extended < blocks) {
        unsigned *bitmap,blkcount;
        unsigned clustalq = 0;
        unsigned clustersz = vcbdev->vcb$l_cluster;
        sts = accesschunk(getmapfcb(vcbdev),clusterno / 4096 + 2,
            (char **) &bitmap,&blkcount,1);
        if ((sts & 1) == 0) return sts;
        do {
            unsigned wordno = (clusterno % 4096) / WORK_BITS;
            unsigned wordval = bitmap[wordno];
            if (wordval == 0xffff) {
                if (clustalq) break;
                clusterno = (clusterno % WORK_BITS) *
                    WORK_BITS + 1;
            } else {
                unsigned bitno = clusterno % WORK_BITS;
                do {
                    if (wordval & (1 << bitno)) {
                        if (clustalq) break;
                    } else {
                        clustalq++;
                        wordval |= 1 << bitno;
                    }
                    clusterno++;
                    if (clustalq >= (extended - blocks)) break;
                } while (++bitno < WORK_BITS);
                if (clustalq) {
                    bitmap[wordno] = wordval;
                    if (bitno < WORK_BITS) break;
                }
            }
        } while (++wordno < blkcount * 512 / sizeof(unsigned));
        mp = (unsigned word *) fcb->head + fcb->head->fh2$b_mpoffset;
        *mp++ = (3 << 14) | (clustalq >> 16);
        *mp++ = clustalq & 0xff;
        *mp++ = clustno & 0xff;
        *clusertize
            * mp++ = clustno << 16;
        fcb->head->fh2$b_map_inuse + 4;
        fcb->fcb$l_efblk += clustalq;
        fcb->head.fh2$w_recattr.fat$l_hiblk[0] = fcb->fcb$l_efblk >> 16;
        fcb->head.fh2$w_recattr.fat$l_hiblk[1] = fcb->fcb$l_efblk & 0xff;
        sts = deaccesschunk(clusterno / 4096 + 2,blkcount,1);
	sts = writechunk(fcb,clusterno/4096+2,bitmap);
        /* code to append clusterno:clustalq to map */
    }
}
#endif



#ifdef EXTEND

unsigned access_create(struct _vcb * vcb,struct _fcb ** fcbadd,unsigned blocks) {
    struct _fcb *fcb;
    struct _fiddef fid;
    unsigned create = sizeof(struct _fcb);
    if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) return SS$_WRITLCK;

    sts = headmap_search(struct _vcb * vcbdev,struct _fiddef * fid,
        struct _fh2 ** headbuff,unsigned *retidxblk,) {
        fcb = cachesearch((void *) &vcb->fcb,filenum,0,NULL,NULL,&create);
        if (fcb == NULL) return SS$_INSFMEM;
        /* If not found make one... */
        if (create == 0) {
	  //fcb->cache.objtype = CACHETYPE_DEV;
            fcb->fcb$b_fid_rvn = fid->fid_b_rvn;
            if (fcb->fcb$b_fid_rvn == 0 && vcb->devices > 1) fcb->fcb$b_fid_rvn = 1;
            fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb = vcb;
            fcb->fcb$l_wlfl = NULL;
        }
        if (wrtflg) {
#if 0
            if (fcb->headvioc != NULL && (fcb->cache.status & CACHE_WRITE) == 0) {
#endif
	      if (0) {
                deaccesshead(0,NULL,0);
		writehead(getidxfcb(vcb),headbuff);
                //fcb->headvioc = NULL;
            }
            fcb->cache.status |= CACHE_WRITE;
        }
        if (1) {
            unsigned sts;
            if (vcb->idxboot != NULL) {
                *fcbadd = fcb;
                fcb->fcb$l_efblk = 32767;   /* guess at indexf.sys file size */
                fcb->fcb$l_highwater = 0;
                fcb->head = vcb->idxboot;       /* Load bootup header */
            }
            sts = accesshead(vcb,fid,0,0,&fcb->head,&fcb->headvbn,wrtflg);
            if (sts & 1) {
                fcb->fcb$l_efblk = VMSSWAP(fcb->head->fh2$w_recattr.fat$l_hiblk);
                if (fcb->head->fh2$b_idoffset > 39) {
                    fcb->fcb$l_highwater = fcb->head->fh2$l_highwater;
                } else {
                    fcb->fcb$l_highwater = 0;
                }
            } else {
                printk("Accessfile status %d\n",sts);
                fcb->cache.objmanager = NULL;
                cacheuntouch(&fcb->cache,0,0);
                cachefree(&fcb->cache);
                return sts;
            }
        }
        *fcbadd = fcb;
        return SS$_NORMAL;
    }
#endif

