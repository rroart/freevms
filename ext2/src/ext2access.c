// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thron�s.

/* Access.c v1.3 */

/*
 This is part of ODS2 written by Paul Nankervis,
 email address:  Paulnank@au1.ibm.com

 ODS2 is distributed freely for all members of the
 VMS community to use. However all derived works
 must maintain comments in their source to acknowledge
 the contibution of the original author.
 */

/*
 This module implements 'accessing' files on an ODS2
 disk volume. It uses its own low level interface to support
 'higher level' APIs. For example it is called by the
 'RMS' routines.
 */

#include <linux/config.h>
#include <linux/linkage.h>
#include <linux/mm.h>
#include <linux/pagemap.h>

#include <linux/string.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <atrdef.h>
#include <ccbdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <dyndef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fi2def.h>
#include <fi5def.h>
#include <fibdef.h>
#include <fiddef.h>
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
#include "x2p.h"
#include <misc.h>
#include <starlet.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <queue.h>

#include <linux/slab.h>
#include <linux/ext2_fs.h>

#define EXT2_EF 30

#define DEBUGx

extern struct __exttwo *x2p;

void exttwo_vms2_mount(void)
{
    int sts;
    struct VCB *vcb;
    struct item_list_3 it[3];
    it[0].item_code = 1; /*not yet */
    it[0].buflen = strlen(root_device_name);
    it[0].bufaddr = root_device_name;
    it[1].item_code = 0;
    it[2].item_code = 0;
    //        sts = mount(options,devices,devs,labs,&vcb);
    sts = exe$mount(it);
}

unsigned long get_x2p_prim_fcb()
{
    return x2p->primary_fcb;
}

#if 0
int ods2_block_read_full_page3(struct _wcb * wcb,struct page *page, unsigned long pageno)
{
    struct _fcb * fcb = wcb->wcb$l_fcb;
    struct _vcb * vcb=x2p->current_vcb;
    unsigned long iblock, lblock;
    unsigned int blocksize, blocks;
    int nr, i;
    int sts;
    struct _iosb iosb;
    int turns;
    unsigned long blocknr;

    blocksize = 512;

    blocks = PAGE_CACHE_SIZE >> 9;
    iblock = pageno << (PAGE_CACHE_SHIFT - 9);
    lblock = iblock + 8; //not yet fcb->fcb$l_efblk+1;

    nr = 0;
    i = 0;
    turns = 0;

    do
    {
        if (iblock < lblock)
        {
            if (fcb)
                blocknr=exttwo_map_vbn(1+iblock,&fcb->fcb$l_wlfl);
            else
                blocknr=iblock;
        }
        else
        {
            continue;
        }

        nr++;

        sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_READPBLK,&iosb,0,0,
                       page_address(page) + i*blocksize,blocksize, blocknr,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);

    }
    while (i++, iblock++, turns++, turns<(PAGE_SIZE/blocksize));

    return 0;
}

static int __ods2_block_write_full_page2(struct _wcb *wcb, struct page *page, unsigned long pageno)
{
    struct _fcb * fcb = wcb->wcb$l_fcb;
    struct _vcb * vcb=x2p->current_vcb;
    unsigned long iblock, lblock;
    int err, i;
    unsigned long block;
    int need_unlock;
    int sts;
    struct _iosb iosb;
    int turns=0;
    signed int blocknr;
    unsigned long blocksize;

    blocksize = 512;

    block = pageno << (PAGE_CACHE_SHIFT - 9);
    iblock = pageno << (PAGE_CACHE_SHIFT - 9);

    i = 0;

    /* Stage 1: make sure we have all the buffers mapped! */
    do
    {
        /*
         * If the buffer isn't up-to-date, we can't be sure
         * that the buffer has been initialized with the proper
         * block number information etc..
         *
         * Leave it to the low-level FS to make all those
         * decisions (block #0 may actually be a valid block)
         */
        if (fcb)
            blocknr=exttwo_map_vbn(1+iblock,&fcb->fcb$l_wlfl);
        else
            blocknr=iblock;
        if (blocknr==-1)
        {
            panic("...page2\n");
        }
        sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_WRITEPBLK,&iosb,0,0,
                       page_address(page)+turns*blocksize,blocksize, blocknr,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0,0,0);

        turns++;
        block++;
        iblock++;
    }
    while (turns<(PAGE_SIZE/blocksize));

    /* Done - end_buffer_io_async will unlock */
#if 0
    SetPageUptodate(page);
#endif
    return 0;

out:
    /*
     * ENOSPC, or some other error.  We may already have added some
     * blocks to the file, so we need to write these out to avoid
     * exposing stale data.
     */
#if 0
    ClearPageUptodate(page);
#endif
    return err;
}

int ods2_block_write_full_page3(struct _wcb * wcb, struct page *page, unsigned long pageno)
{
    struct _fcb * fcb = wcb->wcb$l_fcb;
    unsigned long end_index = pageno + 8; // not yet fcb->fcb$l_efblk+1;
    unsigned offset;
    int err;

    /* easy case */
    if (pageno < end_index)
        return __ods2_block_write_full_page2(wcb, page, pageno);

    panic("should not be here in ...page3\n");

#if 0
    /* things got complicated... */
    offset = inode->i_size & (PAGE_CACHE_SIZE-1);
    /* OK, are we completely out? */
    if (pageno >= end_index+1 || !offset)
    {
        return -EIO;
    }

    /* Sigh... will have to work, then... */
    err = __block_prepare_write(inode, page, 0, offset, pageno);
    if (!err)
    {
        memset(page_address(page) + offset, 0, PAGE_CACHE_SIZE - offset);
        flush_dcache_page(page);
        __block_commit_write(inode,page,0,offset,pageno);
done:
        kunmap(page);
        return err;
    }
    goto done;
#endif
}
#endif

#if 0
int exttwo_read_writevb(struct _irp * i)
{
    int lbn;
    char * buffer;
    struct _iosb iosb;
    struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
    struct _fcb * fcb = x2p->primary_fcb; // ???? is this right
    struct _wcb * wcb = &fcb->fcb$l_wlfl;
    int blocks=(i->irp$l_qio_p2+511)/512;
    int j;
    for(j=0; j<blocks; j++)
    {
        lbn=exttwo_map_vbn(j+i->irp$l_qio_p3,wcb);
        if (i->irp$v_fcode==IO$_WRITEVBLK)
        {
            exttwo_write_block(vcb,512*j+i->irp$l_qio_p1,lbn,1,&iosb);
        }
        else
        {
            buffer=exttwo_read_block(vcb,lbn,1,&iosb);
            memcpy(512*j+i->irp$l_qio_p1,buffer,512);
            kfree(buffer);
        }
        i->irp$l_iost2+=512;
    }
    //exttwo_io_done(i);
}
#endif

signed int ext2_map_vbn(unsigned int vbn, struct _wcb *wcb)
{
    // thing there should be more here?
    signed int lbn = -1;
    ioc_std$mapvblk(vbn, 0, wcb, 0, 0, &lbn, 0, 0);
    return lbn;
}

signed int exttwo_map_idxvbn(struct _vcb * vcb, unsigned int vbn)
{
    // thing there should be more here?
    signed int lbn = -1;
    // struct _wcb * wcb=&((struct _fcb *) getidxfcb(vcb))->fcb$l_wlfl;
    // ioc_std$mapvblk(vbn,0,wcb,0,0,&lbn,0,0);
#if 0
    struct ext2_super_block * sb = exttwo_read_home(vcb);

    long blocks_per_group = le32_to_cpu(sb->s_blocks_per_group);
    long inodes_per_group = le32_to_cpu(sb->s_inodes_per_group);
    long block_size_bits = (10 + le32_to_cpu(sb->s_log_block_size));
    long block_size = 1 << (10 + le32_to_cpu(sb->s_log_block_size));
    long groups = le32_to_cpu(sb->s_blocks_count)/blocks_per_group;
    if (vcb->vcb$l_quocache == 0)
        vcb->vcb$l_quocache = exttwo_read_block(vcb, 2*vms_block_factor2(block_size),sizeof(gd)*groups);
    struct ext2_group_desc * gd = vcb->vcb$l_quocache;

    vbn--;
    long group = vbn / inodes_per_group;
    long groupoff = vbn % inodes_per_group;

    lbn = blocks_per_group * group + gd[group].bg_inode_table + groupoff;

    return lbn;
#endif
    struct buffer_head * bh;
    struct ext2_inode * raw_inode;
    unsigned long block_group;
    unsigned long group_desc;
    unsigned long desc;
    unsigned long block;
    unsigned long offset;
    struct ext2_group_desc * gdp;
    struct ext2_super_block * sb = vcb->vcb$l_cache;
    int ino = vbn - 1;

    if ((ino != EXT2_ROOT_INO && ino != EXT2_ACL_IDX_INO
            && ino != EXT2_ACL_DATA_INO && ino < EXT2_FIRST_INO(sb))||
            ino > le32_to_cpu(sb->s_inodes_count))
    {
        ext2_error (vcb, "ext2_read_inode",
                    "bad inode number: %lu", ino);
        goto bad_inode;
    }
    block_group = (ino - 1) / EXT2_INODES_PER_GROUP(sb);
    if (block_group >= EXT2_GROUPS_COUNT(sb))
    {
        ext2_error(vcb, "ext2_read_inode", "group >= groups count");
        goto bad_inode;
    }
    group_desc = block_group / EXT2_DESC_PER_BLOCK(sb);
    desc = block_group & (EXT2_DESC_PER_BLOCK(sb) - 1);
    gdp = ext2_get_group_desc(vcb, block_group, 0);
    if (!gdp)
    {
        ext2_error(vcb, "ext2_read_inode", "Descriptor not loaded");
        goto bad_inode;
    }

    /*
     * Figure out the offset within the block group inode table
     */
    offset = ((ino - 1) % EXT2_INODES_PER_GROUP(sb)) * EXT2_INODE_SIZE(sb);
    block = le32_to_cpu(gdp[desc].bg_inode_table)
            + (offset >> EXT2_BLOCK_SIZE_BITS(sb));
    return block * vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb));
bad_inode:
    return 0;
}

void * exttwo_read_block(struct _vcb * vcb, unsigned long lbn,
                         unsigned long count, struct _iosb * iosb)
{
    struct _iosb myiosb;
    unsigned char * buf = kmalloc(512 * count, GFP_KERNEL);
    unsigned long phyblk = lbn; // one to one
    //  printk("r1 %x %x %x %x %x %x\n",EXT2_EF,x2p->io_channel,buf,512*count,phyblk,((struct _ucb *)vcb->vcb$l_rvt)->ucb$w_fill_0);
    unsigned long sts = sys$qiow(EXT2_EF, x2p->io_channel, IO$_READLBLK,
                                 &myiosb, 0, 0, buf, 512 * count, phyblk,
                                 ((struct _ucb *) vcb->vcb$l_rvt)->ucb$w_fill_0, 0, 0);
    if (iosb)
        iosb->iosb$w_status = myiosb.iosb$w_status;
    return buf;
}

void * exttwo_write_block(struct _vcb * vcb, unsigned char * buf,
                          unsigned long lbn, unsigned long count, struct _iosb * iosb)
{
    struct _iosb myiosb;
    unsigned long phyblk = lbn; // one to one
    unsigned long sts = sys$qiow(EXT2_EF, x2p->io_channel, IO$_WRITELBLK,
                                 &myiosb, 0, 0, buf, 512 * count, phyblk,
                                 ((struct _ucb *) vcb->vcb$l_rvt)->ucb$w_fill_0, 0, 0);
    if (iosb)
        iosb->iosb$w_status = myiosb.iosb$w_status;
    return buf;
}

void * exttwo_search_fcb(struct _vcb * vcb, struct _fiddef * fid)
{
    struct _fcb * head = &vcb->vcb$l_fcbfl;
    struct _fcb * tmp = head->fcb$l_fcbfl;
    while (tmp != head)
    {
        if (FCB_FID_TO_INO(tmp) == FID_TO_INO(fid))
            return tmp;
        tmp = tmp->fcb$l_fcbfl;
    }
    return 0;
}

void exttwo_read_attrib(struct _fcb * fcb, struct _atrdef * atrp)
{
    struct _iosb iosb;
    int sts = SS$_NORMAL;
    struct ext2_inode * head = exttwo_read_header(x2p->current_vcb, 0, fcb,
                               &iosb);

    while (atrp->atr$w_type != 0)
    {
        switch (atrp->atr$w_type)
        {
        case ATR$C_RECATTR:
        {
            struct _fatdef * f = atrp->atr$l_addr;
            f->fat$b_rtype = (FAT$C_SEQUENTIAL << 4) | FAT$C_FIXED;
            f->fat$b_rattrib = 0;
            f->fat$w_rsize = 0;
            //printk("readat %x %x\n",head->i_blocks,head->i_size%512);
            f->fat$l_hiblk = VMSSWAP((1+head->i_blocks));
            f->fat$l_efblk = VMSSWAP((1+head->i_size/512));
            f->fat$w_ffbyte = head->i_size % 512;
            //printk("readat %x %x %x %x\n",head->i_size,VMSSWAP(f->fat$l_efblk),f->fat$w_ffbyte,VMSSWAP(f->fat$l_hiblk));
            f->fat$b_bktsize = 0;
            f->fat$b_vfcsize = 0;
            f->fat$w_maxrec = 512;
            f->fat$w_defext = 0;
            f->fat$w_gbc = 0;
            f->fat$w_versions = 1;
        }
        break;
        case ATR$C_STATBLK:
        {
            struct _sbkdef * s = atrp->atr$l_addr;
            s->sbk$l_fcb = fcb;
        }
        break;
#if 0
        //not now
        //change to ext2 inode later
        case ATR$C_HEADER:
        {
            struct _fh2 * head;
            head = exttwo_read_header (x2p->current_vcb, 0, fcb, &iosb);
            sts=iosb.iosb$w_status;
            memcpy(atrp->atr$l_addr,head,atrp->atr$w_size);
            kfree(head); // wow. freeing something
        }
        break;
#endif
        case ATR$C_CREDATE:
        {
#if 0
            *(unsigned long long*)atrp->atr$l_addr=unix_to_vms_time(head->i_ctime);
#endif
        }
        break;
        case ATR$C_REVDATE:
        {
#if 0
            *(unsigned long long*)atrp->atr$l_addr=unix_to_vms_time(head->i_mtime);
#endif
        }
        break;
        case ATR$C_EXPDATE:
        {
            *(unsigned long long*) atrp->atr$l_addr = 0;
        }
        break;
        case ATR$C_BAKDATE:
        {
            *(unsigned long long*) atrp->atr$l_addr = 0;
        }
        break;

        case ATR$C_UIC:
            memcpy(atrp->atr$l_addr, &head->i_uid, atrp->atr$w_size);
            break;

        case ATR$C_FPRO:
            memcpy(atrp->atr$l_addr, &head->i_mode, atrp->atr$w_size);
            break;

        default:
            printk("atr %x not supported\n", atrp->atr$w_type);
            break;
        }
        atrp++;
    }
#if 1
    kfree(head);
#endif
}

void exttwo_write_attrib(struct _fcb * fcb, struct _atrdef * atrp)
{
    struct _iosb iosb;
    int sts = SS$_NORMAL;
    struct ext2_inode * head = exttwo_read_header(x2p->current_vcb, 0, fcb,
                               &iosb);

    while (atrp->atr$w_type != 0)
    {
        switch (atrp->atr$w_type)
        {
        case ATR$C_RECATTR:
        {
            struct _fatdef * f = atrp->atr$l_addr;
            //printk("writeat %x %x\n",head->i_size,head->i_blocks);
            head->i_blocks = VMSSWAP(f->fat$l_hiblk) - 1;
            head->i_size = (VMSSWAP(f->fat$l_efblk) << 9) - 512
                           + f->fat$w_ffbyte;
            //printk("writeat %x %x %x %x\n",head->i_size,VMSSWAP(f->fat$l_efblk),f->fat$w_ffbyte,VMSSWAP(f->fat$l_hiblk));
        }
        break;
        case ATR$C_STATBLK:
        {
#if 0
            // not yet?
            struct _sbkdef * s=atrp->atr$l_addr;
            s->sbk$l_fcb=fcb;
#endif
        }
        break;
#if 0
        //not now
        //change to ext2 inode later
        case ATR$C_HEADER:
        {
            struct _fh2 * head;
            head = exttwo_read_header (x2p->current_vcb, 0, fcb, &iosb);
            sts=iosb.iosb$w_status;
            memcpy(atrp->atr$l_addr,head,atrp->atr$w_size);
            kfree(head); // wow. freeing something
        }
        break;
#endif
        case ATR$C_CREDATE:
        {
#if 0
            head->i_ctime=unix_to_vms_time(*(unsigned long long*)atrp->atr$l_addr);
#endif
        }
        break;
        case ATR$C_REVDATE:
        {
#if 0
            head->i_mtime=unix_to_vms_time(*(unsigned long long*)atrp->atr$l_addr);
#endif
        }
        break;
        case ATR$C_EXPDATE:
        {
#if 0
            //*(unsigned long long*)atrp->atr$l_addr=0;
#endif
        }
        break;
        case ATR$C_BAKDATE:
        {
            //*(unsigned long long*)atrp->atr$l_addr=0;
        }
        break;

        case ATR$C_UIC:
            memcpy(&head->i_uid, atrp->atr$l_addr, atrp->atr$w_size);
            break;

        case ATR$C_FPRO:
            memcpy(&head->i_mode, atrp->atr$l_addr, atrp->atr$w_size);
            break;

        default:
            printk("atr %x not supported\n", atrp->atr$w_type);
            break;
        }
        atrp++;
    }
    ext2_sync_inode(x2p->current_vcb, fcb);
#if 1
    kfree(head);
#endif
}

void * exttwo_getvcb(void)
{
    return x2p->current_vcb;
}

#if 0
// remove later?
int getchan_not(struct _vcb * v)
{
    unsigned short int c;
    if (ucb2chan(v, &c))
        return c;
    return ((struct _ucb *)v->vcb$l_rvt)->ucb$ps_adp;
}
#endif

extern struct _ucb * myfilelist[50];
extern char * myfilelists[50];
extern int myfilelistptr;

/* rvn_to_dev() find device from relative volume number */

// half broken. is this switch_volume?
struct _vcb *exttwo_rvn_to_dev(struct _vcb *vcb, unsigned rvn)
{
    return vcb;
}

/* deaccesshead() release header from INDEXF... */

unsigned exttwo_deaccesshead(struct ext2_inode *head, unsigned idxblk)
{
    return deaccesschunk(idxblk, 1, 1);
}

unsigned exttwo_writechunk(struct _fcb * fcb, unsigned long vblock, char * buff)
{
    struct _iosb iosb;
    struct _vcb * vcb = x2p->current_vcb;
    struct _ucb * ucb = ((struct _ucb *) vcb->vcb$l_rvt); //was:  struct _ucb * ucb=finducb(fcb);
    int pbn;
    int sts = ioc_std$mapvblk(vblock, 0, &fcb->fcb$l_wlfl, 0, 0, &pbn, 0, 0);
    sts = sys$qiow(EXT2_EF, x2p->io_channel, IO$_WRITELBLK, &iosb, 0, 0, buff,
                   512, pbn, ucb->ucb$w_fill_0, 0, 0);
    return iosb.iosb$w_status;
}

static unsigned gethead(struct _fcb * fcb, struct ext2_inode **headbuff)
{
    struct _iosb iosb;
    struct _vcb * vcb = x2p->current_vcb;
    struct _ucb * ucb = ((struct _ucb *) vcb->vcb$l_rvt); //was:  struct _ucb * ucb=finducb(fcb);
    int vbn;
    int sts;
    struct _fiddef fid;
    fid.fid$w_num = fcb->fcb$w_fid[0];
    fid.fid$w_seq = fcb->fcb$w_fid[1];
    fid.fid$w_rvn = 0;
    *headbuff = exttwo_read_header(ucb->ucb$l_vcb, &fid, fcb, &iosb);
    return iosb.iosb$w_status;
}

unsigned exttwo_writehead(struct _fcb * fcb, struct ext2_inode *headbuff)
{
    struct _vcb * vcb = x2p->current_vcb;
    struct _ucb * ucb = ((struct _ucb *) vcb->vcb$l_rvt); //was:  struct _ucb * ucb=finducb(fcb);
    int vbn = FCB_FID_TO_INO(fcb) + 1;
    ext2_write_inode(x2p->current_vcb, fcb, 1);
    //  return writechunk(getidxfcb(ucb->ucb$l_vcb),vbn, headbuff);
}

#if 0
void * exttwo_read_home(struct _vcb * vcb)
{
    char * buf;
    struct ext2_super_block * es;
    unsigned long sb_block = 1;
    unsigned short resuid = EXT2_DEF_RESUID;
    unsigned short resgid = EXT2_DEF_RESGID;
    unsigned long logic_sb_block = 1;
    unsigned long offset = 0;
    int blocksize = BLOCK_SIZE;
    int db_count;
    int i, j;

    if (vcb->vcb$l_cache)
        return vcb->vcb$l_cache;

    blocksize = 1024; // not yet get_hardsect_size(dev);
    if (blocksize < BLOCK_SIZE )
        blocksize = BLOCK_SIZE;

#if 0
    if (set_blocksize(dev, blocksize) < 0)
    {
        printk ("EXT2-fs: unable to set blocksize %d\n", blocksize);
        return NULL;
    }
#endif

    if (blocksize != BLOCK_SIZE)
    {
        logic_sb_block = (sb_block*BLOCK_SIZE) / blocksize;
        offset = (sb_block*BLOCK_SIZE) % blocksize;
    }

    long long iosb;
    if (!(buf = exttwo_read_block(vcb, logic_sb_block*vms_block_factor2(blocksize), 1024, &iosb)))
    {
        printk ("EXT2-fs: unable to read superblock\n");
        return NULL;
    }
    /*
     * Note: s_es must be initialized as soon as possible because
     *       some ext2 macro-instructions depend on its value
     */
    es = (struct ext2_super_block *) (buf + offset);
    if (le16_to_cpu(es->s_magic) != EXT2_SUPER_MAGIC)
    {
        printk ("VFS: Can't find ext2 filesystem on dev.\n");
    }
    vcb->vcb$l_cache = es;
    return es;
}
#endif

void * exttwo_read_header(struct _vcb *vcb, struct _fiddef *fid,
                          struct _fcb * fcb, unsigned long * retsts)
{
    struct _iosb iosb;
    unsigned sts;
    char * headbuff;
    struct _vcb *vcbdev;
    unsigned idxvblk;
    signed long idxlblk;
    struct ext2_inode idxfh;
    struct _fiddef * locfid;
    vcbdev = exttwo_rvn_to_dev(vcb, 0);
    if (vcbdev == NULL )
    {
        if (retsts)
            *retsts = SS$_DEVNOTMOUNT;
        return 0;
    }
    //if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) return SS$_WRITLCK;
    if (fcb)
    {
        // not yet    idxlblk = fcb->fcb$l_hdlbn;
        idxlblk = FCB_FID_TO_INO(fcb);
        locfid = &fcb->fcb$w_fid_num;
        //    printk("ino2 %x ",idxlblk);
    }
    else
    {
        idxvblk = FID_TO_INO(fid) + 1;
        idxlblk = exttwo_map_idxvbn(vcb, idxvblk);
        idxlblk = FID_TO_INO(fid);
        locfid = fid;
        //    printk("ino3 %x ",idxlblk);
    }
    x2p->header_lbn = idxlblk;
#if 0
    if (vcbdev->idxfcb->head != NULL)
        if (idxvblk >= VMSSWAP(vcbdev->idxfcb->head->fh2$w_recattr.fat$l_efblk))
            sys$qiow(EXT2_EF,irp->irp$w_chan,IO$_READLBLK,&iosb,0,0,(char *)&idxfh,sizeof(struct ext2_inode),vcb->vcb$l_ibmaplbn,0,0,0);
#endif
#if 0
    not yet
    if (idxvblk >= VMSSWAP(idxfh.fh2$w_recattr.fat$l_efblk))
    {
        {
            printk("Not in index file\n");
            return SS$_NOSUCHFILE;
        }
    }
#endif
    //  sts = accesschunk(getidxfcb(vcb),idxvblk,(char **) headbuff,NULL, 0,0);

    //  headbuff = exttwo_read_block(vcb,idxlblk,1,&iosb);
    ext2_read_inode(vcb, 0, idxlblk, &headbuff, &iosb);
    sts = iosb.iosb$w_status;
    if (0)
    {
        int i;
        for (i = 0; i < 20; i++)
            printk("%x ", ((unsigned char) headbuff[i]));
        printk("\n");
    }

    if (retsts)
        *retsts = sts;
    return headbuff;
}

struct WCBKEY_NOT
{
    unsigned vbn;
    struct _fcb *fcb;
    struct _wcb *prevwcb;
};
/* WCBKEY passes info to compare/create routines... */

/* premap_indexf() called to physically read the header for indexf.sys
 so that indexf.sys can be mapped and read into virtual cache.. */

#if 0
struct ext2_inode *premap_indexf(struct _fcb *fcb,struct _ucb *ucb,unsigned *retsts)
{
    struct _iosb iosb;
    struct ext2_inode *head;
    struct _vcb *vcbdev = rvn_to_dev(ucb->ucb$l_vcb,fcb->fcb$b_fid_rvn);
    if (vcbdev == NULL)
    {
        *retsts = SS$_DEVNOTMOUNT;
        return NULL;
    }
    head = (struct ext2_inode *) kmalloc(sizeof(struct ext2_inode),GFP_KERNEL);
    //  *(unsigned long *)head=0; not need when no vmalloc
    if (head == NULL)
    {
        *retsts = SS$_INSFMEM;
    }
    else
    {
        int sts;
#if 0
        struct _hm2 home;
        sts = sys$qiow(EXT2_EF,irp->irp$w_chan,IO$_READLBLK,&iosb,0,0,(char *) &home,sizeof(struct _hm2),vcbdev->vcb$l_homelbn,0,0,0);
#endif
        *retsts = sys$qiow(EXT2_EF,x2p->io_channel,IO$_READLBLK,&iosb,0,0, (char *) head,sizeof(struct ext2_inode),VMSLONG(vcbdev->vcb$l_ibmaplbn) + VMSWORD(vcbdev->vcb$l_ibmapsize),((struct _ucb *)vcbdev->vcb$l_rvt)->ucb$w_fill_0,0,0);
        *retsts = iosb.iosb$w_status;
        if (!(*retsts & 1))
        {
            kfree(head);
            head = NULL;
        }
        else
        {
        }
    }
    return head;
}
#endif

/* wcb_create() creates a window control block by reading appropriate
 file headers... */

int exttwo_wcb_create_all(struct _vcb * vcb, struct _fcb * fcb)
{
    struct ext2_super_block * sb = vcb->vcb$l_cache;
    struct ext2_super_block * s = vcb->vcb$l_cache;
    int ptrs = EXT2_ADDR_PER_BLOCK(sb);
    int ptrs_bits = EXT2_ADDR_PER_BLOCK_BITS(sb);
    const long direct_blocks = EXT2_NDIR_BLOCKS, indirect_blocks = ptrs,
               double_blocks = (1 << (ptrs_bits * 2));
    struct _iosb iosb;
    unsigned int retsts;
    unsigned curvbn = 1;
    unsigned extents = 0;
    int i;

    struct _ucb * ucb = finducb(fcb); // bad bad
    unsigned short *mp;
    unsigned short *me;
    int contin = 0;
    int nextvbn = 0;
    int stvbn = 1;
    int phylen = 1;
    int nextphyblk = 1;
    int * i_data = &fcb->fcb$l_reserve2; // check
    int stphyblk = i_data[0];
    int phyblk = stphyblk;
    int l1 = 0, l2 = 0, l3 = 0;
    int l1p = 0, l2p = 0, l3p = 0;
    int *b1, *b2, *b3;

    retsts = 0;
    for (i = 0; i < 15; i++)
        retsts |= i_data[i];
    if (retsts == 0)
        return SS$_NORMAL;

    b1 = kmalloc(EXT2_BLOCK_SIZE(s), GFP_KERNEL);
    b2 = kmalloc(EXT2_BLOCK_SIZE(s), GFP_KERNEL);
    b3 = kmalloc(EXT2_BLOCK_SIZE(s), GFP_KERNEL);

    for (i = 1; i < 15 && nextphyblk; 1)
    {

        phyblk = nextphyblk;

        if (i < 12)
        {
            nextphyblk = i_data[i];
            i++;
        }
        else
        {
            if (i_data[i] == 0)
            {
                i++;
                nextphyblk = 0;
            }
            if (l1 == 0)
            {
                l1 = i_data[i];
                myqio(READ, b1, EXT2_BLOCK_SIZE(s), l1, 0,
                      vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb)));
            }
            if (i == 12)
            {
                nextphyblk = b1[l1p];
                l1p++;
                if (l1p == ptrs)
                {
                    l1p = 0;
                    l1 = 0;
                    i++;
                    goto out;
                }
            }
            if (i == 13)
            {
                if (l2 == 0)
                {
                    l2 = b1[l1p];
                    myqio(READ, b2, EXT2_BLOCK_SIZE(s), l2, 0,
                          vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb)));
                }
                nextphyblk = b2[l2p];
                l2p++;
                if (l2p == ptrs)
                {
                    l2p = 0;
                    l2 = 0;
                    l1p++;
                }
                if (l1p == ptrs)
                {
                    l1p = 0;
                    l1 = 0;
                    i++;
                }
            }
            if (i == 14)
            {
                // 3rd not supported yet?
                //i++;
                printk("beware 3rd level\n");
                if (l2 == 0)
                {
                    l2 = b1[l1p];
                    myqio(READ, b2, EXT2_BLOCK_SIZE(s), l2, 0,
                          vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb)));
                }
                if (l3 == 0)
                {
                    l3 = b2[l2p];
                    myqio(READ, b2, EXT2_BLOCK_SIZE(s), l3, 0,
                          vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb)));
                }
                nextphyblk = b3[l3p];
                l3p++;
                if (l3p == ptrs)
                {
                    l3p = 0;
                    l3 = 0;
                    l2p++;
                }
                if (l2p == ptrs)
                {
                    l2p = 0;
                    l2 = 0;
                    l1p++;
                }
                if (l1p == ptrs)
                {
                    l1p = 0;
                    l1 = 0;
                    i++;
                }
            }
        }

out:

        if ((phyblk + 1) != nextphyblk)
        {
            contin = 0;
        }
        else
        {
            contin = 1;
            phyblk++;
            phylen++;
        }

        if (contin == 0)
        {
            struct _wcb * wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb),
                                GFP_KERNEL);
            memset(wcb, 0, sizeof(struct _wcb));
            if (wcb == NULL )
            {
                retsts = SS$_INSFMEM;
                return retsts;
            }

            wcb->wcb$b_type = DYN$C_WCB;
            wcb->wcb$l_orgucb = x2p->current_ucb;
            insque(wcb, &fcb->fcb$l_wlfl);
            wcb->wcb$l_fcb = fcb;

            wcb->wcb$l_stvbn = stvbn;
            wcb->wcb$l_p1_count = phylen;
            wcb->wcb$l_p1_lbn = stphyblk;

            stvbn += phylen;
            stphyblk = nextphyblk;

            phylen = 1;
            phyblk = 0;

        }

    }

    kfree(b1);
    kfree(b2);
    kfree(b3);

    retsts = SS$_NORMAL;
    return retsts;
}

/* getwindow() find a window to map VBN to LBN ... */

unsigned exttwo_getwindow(struct _fcb * fcb, unsigned vbn, struct _vcb **devptr,
                          unsigned *phyblk, unsigned *phylen, struct _fiddef *hdrfid,
                          unsigned *hdrseq)
{
    unsigned sts = SS$_NORMAL;
    struct _wcb *wcb;
#ifdef DEBUG
    printk("Accessing window for vbn %d, file (%x)\n",vbn,fcb->cache.hashval);
#endif

    wcb = fcb->fcb$l_wlfl;

    *devptr = exttwo_rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0);
    sts = ioc_std$mapvblk(vbn, 0, &fcb->fcb$l_wlfl, 0, 0, phyblk, 0, 0);
    *phylen = 1;
    if (hdrfid != NULL )
        memcpy(hdrfid, &wcb->wcb$l_fcb->fcb$w_fid, sizeof(struct _fiddef));
    //    if (hdrseq != NULL) *hdrseq = wcb->hd_seg_num;
#ifdef DEBUG
    printk("Mapping vbn %d to %d (%d -> %d)[%d] file (%x)\n",
           vbn,*phyblk,wcb->loblk,wcb->hiblk,wcb->hd_basevbn,fcb->cache.hashval);
#endif

    if (*devptr == NULL )
        return SS$_DEVNOTMOUNT;
    return SS$_NORMAL;
}

/* deaccesschunk() to deaccess a VIOC (chunk of a file) */

unsigned exttwo_deaccesschunk(unsigned wrtvbn, int wrtblks, int reuse)
{
#ifdef DEBUG
    printk("Deaccess chunk %8x\n",vioc->cache.hashval);
#endif
    if (wrtvbn)
    {
        unsigned modmask;
#if 0
        if (wrtvbn <= vioc->cache.hashval ||
                wrtvbn + wrtblks > vioc->cache.hashval + VIOC_CHUNKSIZE + 1)
        {
            return SS$_BADPARAM;
        }
#endif
#if 0
        modmask = 1 << (wrtvbn - vioc->cache.hashval - 1);
        while (--wrtblks > 0) modmask |= modmask << 1;
        if ((vioc->wrtmask | modmask) != vioc->wrtmask) return SS$_WRITLCK;
        vioc->modmask |= modmask;
        if (vioc->cache.refcount == 1) vioc->wrtmask = 0;
        vioc->cache.objmanager = vioc_manager;
#endif
    }
    //cache_untouch(&vioc->cache,reuse);
    return SS$_NORMAL;
}

void *exttwo_readvblock(struct _fcb * fcb, unsigned curvbn, unsigned *retsts)
{
    struct _iosb iosb;
    struct _vcb * vcb = x2p->current_vcb;
    struct ext2_super_block * sb = vcb->vcb$l_cache;
    int factor = vms_block_factor(EXT2_BLOCK_SIZE_BITS(sb));
    int length;
    char *address = 0;
    length = fcb->fcb$l_efblk - curvbn + 1;
    length = 1;
    do
    {
        // no breaking of the water for now?
        if (fcb->fcb$l_highwater != 0 && curvbn >= fcb->fcb$l_highwater)
        {
            //memset(address,0,length * 512);
            break;
        }
        else
        {
            unsigned sts;
            unsigned phyblk, phylen;
            struct _vcb *vcbdev;
            sts = exttwo_getwindow(fcb, curvbn, &vcbdev, &phyblk, &phylen, NULL,
                                   NULL );
            if (sts & 1)
            {
                if (phylen > length)
                    phylen = length;
                if (fcb->fcb$l_highwater != 0
                        && curvbn + phylen > fcb->fcb$l_highwater)
                {
                    phylen = fcb->fcb$l_highwater - curvbn;
                }
                address = exttwo_read_block(vcbdev, factor * phyblk,
                                            factor * phylen, &iosb);
                sts = iosb.iosb$w_status;
            }
            if ((sts & 1) == 0)
            {
                *retsts = sts;
                return NULL ;
            }
            length -= phylen;
            curvbn += phylen;
            //address += phylen * 512;
        }
    }
    while (length > 0);
#if 0
    // not yet?
    if (address==0)
        *retsts=0;
    else
#endif
        *retsts = SS$_NORMAL;
    return address;
}

/* accesschunk() return pointer to a 'chunk' of a file ... */

unsigned exttwo_accesschunk(struct _fcb *fcb, unsigned vbn, char **retbuff,
                            unsigned *retblocks, unsigned wrtblks, struct _irp * irp)
{
    unsigned sts;
    int blocks;
    if (!fcb)
        fcb = x2p->primary_fcb;
    //  if (vbn < 1 || vbn > fcb->fcb$l_efblk) return SS$_ENDOFFILE;
    if (vbn < 1 || vbn > 100000)
    {
        iosbret(irp, SS$_ENDOFFILE);    // for second read
        return SS$_ENDOFFILE;
    }

    *retbuff = exttwo_readvblock(fcb, vbn, &sts);

    if (retblocks)
        *retblocks = 1;
#if 0
    // not yet?
    iosbret(irp,sts);
    return sts;
#else
    iosbret(irp, SS$_NORMAL);
    return SS$_NORMAL;
#endif
}

unsigned deallocfile(struct _fcb *fcb);

/* deaccessfile() finish accessing a file.... */

unsigned exttwo_deaccessfile(struct _fcb *fcb)
{
    struct _iosb iosb;
    int sts;
    struct ext2_inode * head;
#ifdef DEBUG
    printk("Deaccessing file (%x) reference %d\n",fcb->cache.hashval,fcb->cache.refcount);
#endif
    head = exttwo_read_header(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb,
                              &iosb);
    kfree(head);
    sts = iosb.iosb$w_status;

    return SS$_NORMAL;
}

static void *fcb_create_not(unsigned filenum, unsigned *retsts)
{
    struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb), GFP_KERNEL);
    if (fcb == NULL )
    {
        if (retsts)
            *retsts = SS$_INSFMEM;
    }
    else
    {
        qhead_init(&fcb->fcb$l_wlfl);
        fcb->fcb$b_type = DYN$C_FCB;
        fcb->fcb$l_efblk = 100000;
        fcb->fcb$l_highwater = 0;
        fcb->fcb$l_status = 0;
        fcb->fcb$b_fid_rvn = 0;
        fcb->fcb$l_fill_5 = 0;
    }
    return fcb;
}

void *exttwo_fcb_create2(struct ext2_inode * head, int i_ino, unsigned *retsts)
{
    struct _vcb * vcb = x2p->current_vcb;
    struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb), GFP_KERNEL);
    memset(fcb, 0, sizeof(struct _fcb));
    if (fcb == NULL )
    {
        if (retsts)
            *retsts = SS$_INSFMEM;
        return;
    }
    fcb->fcb$b_type = DYN$C_FCB;
    fcb->fcb$l_fill_5 = 1;
    qhead_init(&fcb->fcb$l_wlfl);

    SET_FCB_FID_FROM_INO(fcb, i_ino);
    fcb->fcb$l_lockbasis = i_ino; // add rvn later

    insque(fcb, &vcb->vcb$l_fcbfl);

    fcb->fcb$l_efblk = head->i_blocks;
    fcb->fcb$l_highwater = 0;

    fcb->fcb$l_filesize = head->i_size;

    if (S_ISDIR(le16_to_cpu(head->i_mode)))
        fcb->fcb$v_isdir = 1;
    if (S_ISDIR(le16_to_cpu(head->i_mode)))
        fcb->fcb$v_dir = 1;

    memcpy(&fcb->fcb$l_reserve2, &head->i_block, 4 * EXT2_N_BLOCKS);
    exttwo_wcb_create_all(vcb, fcb);

    fcb->fcb$l_hdlbn = exttwo_map_idxvbn(vcb, i_ino + 1);

    *retsts = SS$_NORMAL;
    return fcb;
}

/* accessfile() open up file for access... */

unsigned exttwo_access(struct _vcb * vcb, struct _irp * irp)
{
    struct _iosb iosb;
    unsigned sts = SS$_NORMAL;
    unsigned wrtflg = 1;
    struct _fcb *fcb;
    struct ext2_inode *head = 0;
    struct dsc$descriptor * fibdsc = irp->irp$l_qio_p1;
    struct dsc$descriptor * filedsc = irp->irp$l_qio_p2;
    unsigned short *reslen = irp->irp$l_qio_p3;
    struct dsc$descriptor * resdsc = irp->irp$l_qio_p4;
    void * atrp = irp->irp$l_qio_p5;
    struct _fibdef * fib = (struct _fibdef *) fibdsc->dsc$a_pointer;
    struct _fiddef * fid =
        &((struct _fibdef *) fibdsc->dsc$a_pointer)->fib$w_fid_num;
    struct _fiddef * fids =
        &((struct _fibdef *) fibdsc->dsc$a_pointer)->fib$w_did_num;
    exttwo_translate_fid(vcb, fid);
    //  printk("ino %x %x %x\n",FID_TO_INO(fid),FID_TO_INO(fids),irp->irp$l_func);
    unsigned action = 0;
    if (irp->irp$l_func & IO$M_ACCESS)
        action = 0;
    if (irp->irp$l_func & IO$M_DELETE)
        action = 1;
    if (irp->irp$v_fcode == IO$_CREATE)
        action = 2;
    if (irp->irp$l_func & IO$M_CREATE)
        action = 2;
#ifdef DEBUG
    printk("Accessing file (%d,%d,%d)\n",(fid->fid$b_nmx << 16) +
           fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn);
#endif

    x2p->current_vcb = vcb; // until I can place it somewhere else

    x2p->prim_lckindx = serial_file("ext2$s", x2p, fib->fib$w_fid_num);

    if (x2p->primary_fcb)
    {
        struct _fcb * fcb = x2p->primary_fcb;
        if (fid->fid$w_num != fcb->fcb$w_fid_num)
            x2p->primary_fcb = 0; //exttwo_search_fcb(vcb,fid);
    }

    if (fib->fib$w_did_num)
    {
        struct ext2_inode * head;
        struct _fcb * fcb = x2p->primary_fcb;
        if (fcb == 0)
            fcb = exttwo_search_fcb(vcb, &fib->fib$w_did_num);
        head = exttwo_read_header(vcb, 0, fcb, &iosb);
        sts = iosb.iosb$w_status;
        if (S_ISDIR(le16_to_cpu(head->i_mode)))
        {
            unsigned eofblk = head->i_blocks;
            sts = exttwo_search_ent(fcb, fibdsc, filedsc, reslen, resdsc,
                                    eofblk, action);
        }
        else
        {
            sts = SS$_BADIRECTORY;
        }
#if 1
        // check leak remaining
        kfree(head);
#endif
    }

#if 0
    printk("ret %x %x\n",ctl$gl_pcb,sts);
#endif

    if ((sts & 1) == 0)
    {
        iosbret(irp, sts);
        return sts;
    }

    if ((irp->irp$l_func & IO$M_ACCESS) == 0 && irp->irp$l_qio_p5 == 0)
        return SS$_NORMAL;

    if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0))
    {
        iosbret(irp, SS$_WRITLCK);
        return SS$_WRITLCK;
    }

    fcb = exttwo_search_fcb(vcb, fid);
    if (FID_TO_INO(fid) == 0)
        sts = SS$_DATACHECK;
    else
    {
        head = exttwo_read_header(vcb, fid, fcb, &iosb);
        sts = iosb.iosb$w_status;
    }
    if (sts & 1)
    {
    }
    else
    {
#if 1
        // check leak remaining
        kfree(head);
#endif
        printk("Accessfile status %d\n", sts);
        iosbret(irp, sts);
        return sts;
    }

    if (fcb == NULL )
    {
        fcb = exttwo_fcb_create2(head, FID_TO_INO(fid), &sts);
    }
#if 1
    // check leak remaining
    kfree(head);
#endif
    if (fcb == NULL )
    {
        iosbret(irp, sts);
        return sts;
    }
    set_ccb_wind(x2p->io_channel, fcb); // temp fix

    x2p->primary_fcb = fcb;
    x2p->current_window = &fcb->fcb$l_wlfl;

    if (atrp)
    {
        if (action == 0)
            exttwo_read_attrib(fcb, atrp);
    }

    iosbret(irp, SS$_NORMAL);
    return SS$_NORMAL;
}

/* dismount() finish processing on a volume */

unsigned ext2_dismount(struct _vcb * vcb)
{
#if 0
    unsigned sts,device;
    struct _vcb *vcbdev;
    int expectfiles = vcb->devices;
    int openfiles = cache_refcount(&vcb->fcb->cache);
    if (vcb->vcb$b_status & VCB$M_WRITE_IF) expectfiles *= 2;
#ifdef DEBUG
    printk("Dismounting disk %d\n",openfiles);
#endif
    sts = SS$_NORMAL;
    if (openfiles != expectfiles)
    {
        sts = SS$_DEVNOTDISM;
    }
    else
    {
        vcbdev = vcb->vcbdev;
        for (device = 0; device < vcb->devices; device++)
        {
            if (vcbdev->dev != NULL)
            {
                if (vcb->vcb$b_status & VCB$M_WRITE_IF && getmapfcb(vcb) != NULL)
                {
                    sts = deaccessfile(vcbdev->getmapfcb(vcb));
                    if (!(sts & 1)) break;
                    vcbdev->idxfcb->fcb$l_status &= ~FCB_WRITE;
                    //                    vcbdev->mapfcb = NULL;
                }
                cache_remove(&vcb->fcb->cache);
                sts = deaccesshead(vcbdev->idxfcb->headvioc,vcbdev->idxfcb->head,vcbdev->idxfcb->headvbn);
                if (!(sts & 1)) break;
                cache_untouch(&vcbdev->idxfcb->cache,0);
                vcbdev->dev->vcb = NULL;
            }
            vcbdev++;
        }
        if (sts & 1)
        {
            cache_remove(&vcb->fcb->cache);
            while (vcb->dircache) cache_delete((struct CACHE *) vcb->dircache);
#ifdef DEBUG
            printk("Post close\n");
            cachedump();
#endif
            kfree(vcb);
        }
    }
    return sts;
#endif
}

#define HOME_LIMIT 100

/* mount() make disk volume available for processing... */

unsigned mounte2(unsigned flags, unsigned devices, char *devnam[],
                 char *label[], struct _vcb **retvcb)
{
    struct _iosb iosb;
    unsigned device, sts = SS$_NORMAL;
    struct _vcb *vcb = 0;
    struct _vcb *vcbdev;
    struct _ucb *ucb;
    struct ext2_super_block home;
    struct _aqb *aqb;
    int islocal;
    int isfile = 0;
    if (sizeof(struct ext2_super_block) != 1024)
        return SS$_NOTINSTALL;
    for (device = 0; device < devices; device++)
    {
        //printk("Trying to mount %s\n",devnam[device]);
        int hba = 0;
        int chan;
        if (strchr(devnam[device], '$'))
        {
            short int chan;
            extern struct _ccb ctl$ga_ccb_table[];
            struct dsc$descriptor dsc;
            dsc.dsc$a_pointer = devnam[device];
            dsc.dsc$w_length = strlen(devnam[device]);

            sts = exe$assign(&dsc, &chan, 0, 0, 0);
            ucb = ctl$ga_ccb_table[chan].ccb$l_ucb;
            //ucb = du_init(devnam[device]);
            islocal = 0;
        }
        else
        {
            struct dsc$descriptor dsc;
            short int chan;
            char buf[7];
            int sts;
#if 0
            dsc.dsc$a_pointer=devnam[device];
#endif
            dsc.dsc$w_length = strlen(devnam[device]);
            memcpy(buf, devnam[device], dsc.dsc$w_length);
            buf[dsc.dsc$w_length] = 0;
            dsc.dsc$a_pointer = buf;

            sts = exe$assign(&dsc, &chan, 0, 0, 0);

            //printk("sts %x\n",sts);
            if ((sts & 1) == 0)
            {
                ucb = fl_init(devnam[device]);
                isfile = 1;
            }
            else
            {
                extern struct _ccb ctl$ga_ccb_table[];
                ucb = ctl$ga_ccb_table[chan].ccb$l_ucb;
            }
            islocal = 1;
        }
        vcb = (struct _vcb *) kmalloc(sizeof(struct _vcb), GFP_KERNEL);
        memset(vcb, 0, sizeof(struct _vcb));
        global_e2_vcb = vcb;
        vcb->vcb$b_type = DYN$C_VCB;
        x2p->current_vcb = vcb; // until I can place it somewhere else
        aqb = (struct _aqb *) kmalloc(sizeof(struct _aqb), GFP_KERNEL);
        memset(aqb, 0, sizeof(struct _aqb));
        aqb->aqb$b_type = DYN$C_AQB;
        qhead_init(&aqb->aqb$l_acpqfl);
        aqb->aqb$l_acppid = 1;
        ucb->ucb$l_vcb = vcb;
        vcb->vcb$l_aqb = aqb;
        if (vcb == NULL )
            return SS$_INSFMEM;
        vcb->vcb$b_status = 0;
        if (flags & 1)
            vcb->vcb$b_status |= VCB$M_WRITE_IF;
        qhead_init(&vcb->vcb$l_fcbfl);
        vcb->vcb$l_cache = NULL; // ?
        vcbdev = vcb;
        sts = SS$_NOSUCHVOL;
        //    vcbdev->dev = NULL;
        if (strlen(devnam[device]))
        {
            struct dsc$descriptor dsc;
            if (islocal && isfile)
                sts = phyio_init(strlen(devnam[device]) + 1, devnam[device],
                                 &ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count, 0, ucb);
            dsc.dsc$a_pointer = do_file_translate(devnam[device]);
            dsc.dsc$w_length = strlen(dsc.dsc$a_pointer);
#if 0
            if (!islocal)
                dsc.dsc$a_pointer=((char *) dsc.dsc$a_pointer)+1;
#endif
            sts = exe$assign(&dsc, &chan, 0, 0, 0);
            x2p->io_channel = chan;
            ucb->ucb$ps_adp = chan; //wrong field and use, but....
            //sts = device_lookup(strlen(devnam[device]),devnam[device],1,&ucbret);
            //if (!(sts & 1)) break;
            //      ucb->handle=vcbdev->dev->handle;
            for (hba = 1; hba <= HOME_LIMIT; hba++)
            {
                sts = sys$qiow(EXT2_EF, chan, IO$_READLBLK, &iosb, 0, 0,
                               (char *) &home, sizeof(struct ext2_super_block), hba,
                               ucb->ucb$w_fill_0, 0, 0);
                if (!(sts & 1))
                    break;
                vcb->vcb$l_quocache = (chan << 16) | ucb->ucb$w_fill_0;
                if (home.s_magic == EXT2_SUPER_MAGIC)
                    break;
                sts = SS$_DATACHECK;
            }
            if (sts & 1)
            {
            }
            if (!(sts & 1))
                break;
        }
        if (sts & 1)
        {
            vcbdev = vcb;
            if (strlen(devnam[device]))
            {
                struct _fiddef idxfid =
                    { 1, 1, 0, 0 };
                struct _fcb * idxfcb;
                struct ext2_inode * idxhd;
                struct _fcb * mapfcb;
                //    vcb->vcb$ibmaplbn = idxfcb->stlbn;
                vcb->vcb$l_homelbn = home.s_first_data_block;
                vcb->vcb$l_ibmaplbn = 0;            //home.hm2$l_ibmaplbn;
                vcb->vcb$l_ibmapvbn = 0;            //home.hm2$w_ibmapvbn;
                vcb->vcb$l_ibmapsize = 0;  //home.hm2$w_ibmapsize;  // wrong use
                vcb->vcb$l_cluster = 0;            //home.hm2$w_cluster;
                vcb->vcb$l_maxfiles = 0;            //home.hm2$l_maxfiles;
                //vcb->vcb$l_free = 500; // how do we compute this?
                memcpy(&vcb->vcb$t_volname, devnam[device], 12);
                memcpy(&vcb->vcb$t_volcknam, devnam[device], 12);
                volume_lock("ext2$a", &vcb->vcb$t_volcknam);
                struct ext2_super_block * sb = &home;
                long blocks_per_group = le32_to_cpu(sb->s_blocks_per_group);
                long block_size = 1 << (10 + le32_to_cpu(sb->s_log_block_size));
                long groups = le32_to_cpu(sb->s_blocks_count)
                              / blocks_per_group;
                long groupsize = groups * sizeof(struct ext2_group_desc);
                struct ext2_super_block * homep = kmalloc(
                                                      sizeof(struct ext2_super_block) + groupsize,
                                                      GFP_KERNEL);
                sts = sys$qiow(EXT2_EF, chan, IO$_READLBLK, &iosb, 0, 0,
                               (char *) homep,
                               sizeof(struct ext2_super_block) + groupsize, hba,
                               ucb->ucb$w_fill_0, 0, 0);
                //  memcpy(homep, &home, 1024);
                vcb->vcb$l_cache = homep;
                if (!(iosb.iosb$w_status & 1))
                {
                    ucb->ucb$l_vcb = NULL;
                }
                else
                {
                    //vcbdev->max_cluster = (scb->scb$l_volsize + scb->scb$w_cluster - 1) / scb->scb$w_cluster;
                    ucb->ucb$l_vcb = vcb;
                    vcb->vcb$l_rvt = ucb; // just single volume so far
                    vcbdev->vcb$l_free = home.s_free_blocks_count;
                    printk("Freespace is %d\n", vcbdev->vcb$l_free);
                }
            }
        }
    }
    if (retvcb != NULL )
        *retvcb = vcb;
    return sts;
}
