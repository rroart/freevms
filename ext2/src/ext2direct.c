// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thron�s.

/* Direct.c v1.3 */

/*
 This is part of ODS2 written by Paul Nankervis,
 email address:  Paulnank@au1.ibm.com

 ODS2 is distributed freely for all members of the
 VMS community to use. However all derived works
 must maintain comments in their source to acknowledge
 the contibution of the original author.
 */

/*  This module does all directory file handling - mostly
 lookups of filenames in directory files... */

#include <linux/mm.h>
#include <linux/ctype.h>
#include <linux/string.h>

#include <atrdef.h>
#include <ccbdef.h>
#include <fcbdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <irpdef.h>
#include <ucbdef.h>
#include <wcbdef.h>
#include <aqbdef.h>
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
#include "x2p.h"
#include <linux/slab.h>
#include <misc_routines.h>

#include <linux/ext2_fs.h>

#ifdef __x86_64__
#define memchr kernel_memchr
void *kernel_memchr(const void *s, int c, size_t n);
#endif

#define DEBUGx on
#define BLOCKSIZE 1024 // check
#define MAXREC (BLOCKSIZE - 2)

#if 0
#define STRUCT_DIR_SIZE (sizeof(struct ext2_dir_entry_2)) // but this gives one too much
#else
#define STRUCT_DIR_SIZE 7
#endif

/* Some statistical counters... */

/* exttwo_name_check() - take a name specification and return name length without
 the version number, an integer version number, and a wildcard flag */

unsigned exttwo_name_check(char *str, int len, int *retlen, int *retver,
                           int *wildflag)
{
    int wildcard = 0;
    char *name_start = str;
    int dots = 0;
    char *name = name_start;
    char *name_end = name + len;

    /* Go through the specification checking for illegal characters */

    while (name < name_end)
    {
        char ch = *name++;
        if (ch == '.')
        {
            if ((name - name_start) > 40)
                return SS$_BADFILENAME;
            name_start = name;
            {
                // workaround for .DIR?
                if (0 == strncmp(name, "DIR", 3))
                {
                    break;
                }
                // workaround for dcl.;?
                if (*name == ';')
                    break;
            }
            if (dots++ > 1)
                break;
        }
        else
        {
            if (ch == ';')
            {
                break;
            }
            else
            {
                if (ch == '*' || ch == '%')
                {
                    wildcard = 1;
                }
                else
                {
                    if (ch == '[' || ch == ']' || ch == ':' || !isprint(ch))
                        return SS$_BADFILENAME;
                }
            }
        }
    }
    if ((name - name_start) > 40)
        return SS$_BADFILENAME;

    /* Return the name length and start checking the version */

    *retlen = name - str - 1;
    if (name < name_end)
    {
        char ch = *name;
        if (ch == '*')
        {
            if (++name < name_end)
                return SS$_BADFILENAME;
            wildcard = 1;
        }
    }
    *retver = 1;
    *wildflag = wildcard;
    return SS$_NORMAL;
}

#define MAT_LT 0
#define MAT_EQ 1
#define MAT_GT 2
#define MAT_NE 3
#define MAT_SKIP 4

/* name_match() - compare a name specification with a directory entry
 and determine if there is a match, too big, too small... */

int exttwo_name_match(char *spec, int spec_len, char *dirent, int dirent_len,
                      int ino)
{
    int percent = MAT_GT;
#if 0
    printk("match %x %x %x %x %x %s %s\n",ctl$gl_pcb,spec,dirent,spec_len,dirent_len,spec,dirent);
#endif
    char *name = spec, *entry = dirent;
#if 1
    // temp workaround for handling dot-less names.
    char localname[128];
    memcpy(localname, dirent, dirent_len); // chech len later
    localname[dirent_len] = 0;
    int wild = memchr(spec, '%', spec_len) || memchr(spec, '*', spec_len);
    if (wild && 0 == strchr(localname, '.'))
        localname[dirent_len++] = '.';
    entry = localname;
#endif
    char *name_end = name + spec_len, *entry_end = entry + dirent_len;

    {
        if (ino != 2 && dirent_len == 1 && dirent[0] == '.')
        {
            return MAT_SKIP;
        }
    }
    {
        // workaround for .
        if (dirent_len == 1 && *dirent == '.'
                && 0 == strncmp(spec, "000000", 6))
            return MAT_EQ;
    }
    {
        // workaround for ..
        if (dirent_len == 2 && dirent[0] == '.' && dirent[1] == '.')
        {
            return MAT_SKIP;
        }
    }

    /* See how much name matches without wildcards... */

    while (name < name_end && entry < entry_end)
    {
        char sch = *name;
        if (sch != '*')
        {
            char ech = *entry;
            if (sch != ech)
                if (toupper(sch) != toupper(ech))
                    if (sch == '%')
                    {
                        percent = MAT_NE;
                    }
                    else
                    {
                        break;
                    }
        }
        else
        {
            break;
        }
        name++;
        entry++;
    }

    /* Mismatch - return result unless wildcard... */

    if (name >= name_end)
    {
        if (entry >= entry_end)
        {
            return MAT_EQ;
        }
        else
        {
            return percent;
        }
    }
    else
    {

        /* See if we can find a match with wildcards */

        if (*name != '*')
        {
            if (percent == MAT_NE)
                return MAT_NE;
            if (entry < entry_end)
                if (toupper(*entry) > toupper(*name))
                    return MAT_GT;
            return MAT_LT;
        }
        /* Strip out wildcard(s) - if end then we match! */

        do
        {
            name++;
        }
        while (name < name_end && *name == '*');
        if (name >= name_end)
            return MAT_EQ;

        /* Proceed to examine the specification past the wildcard... */

        while (name < name_end)
        {
            int offset = 1;
            char fch = toupper(*name++);

            /* See if can can find a match for the first character... */

            if (fch != '%')
            {
                while (entry < entry_end)
                {
                    if (toupper(*entry) != fch)
                    {
                        entry++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            /* Give up if we can't find that one lousy character... */

            if (entry >= entry_end)
                return MAT_NE;
            entry++;

            /* See how much of the rest we can match... */

            while (name < name_end && entry < entry_end)
            {
                char sch = *name, ech;
                if (sch == '*')
                    break;
                if (sch != (ech = *entry))
                    if (toupper(sch) != toupper(ech))
                        if (sch != '%')
                            break;
                name++;
                entry++;
                offset++;
            }

            /* If matching died because of a wildcard we are OK... */

            if (name < name_end && *name == '*')
            {
                do
                {
                    name++;
                }
                while (name < name_end && *name == '*');
                if (name >= name_end)
                    return MAT_EQ;

                /* Otherwise we finished OK or we need to try again... */

            }
            else
            {
                if (name >= name_end && entry >= entry_end)
                    return MAT_EQ;
                name -= offset;
                entry -= offset - 1;
            }
        }
    }

    /* No more specification - match depends on remainder of entry... */

    if (entry < entry_end)
        return MAT_NE;
    return MAT_EQ;
}

/* insert_ent() - procedure to add a directory entry at record dr entry de */

unsigned exttwo_insert_ent(struct _fcb * fcb, unsigned eofblk, unsigned curblk,
                           char *buffer, struct ext2_dir_entry_2 * dr, struct _dir1 * de,
                           char *filename, unsigned filelen, unsigned version,
                           struct _fiddef * fid)
{
    struct _iosb iosb;
    unsigned sts = 1;
    int inuse = 0;
    struct ext2_inode * head;

    /* Compute space required... */

    int addlen = 0;
    if (de == NULL )
        addlen += EXT2_DIR_REC_LEN(filelen);

    /* Compute block space in use ... */

    {
        char invalid_dr = 1;
        do
        {
            int sizecheck;
            struct ext2_dir_entry_2 *nr = (struct ext2_dir_entry_2 *) (buffer
                                          + inuse);
            if (dr == nr)
                invalid_dr = 0;
            if (nr->inode == 0)
                break;
            sizecheck = le16_to_cpu(nr->rec_len);
            inuse += sizecheck;
            if (inuse > MAXREC || (inuse & EXT2_DIR_ROUND)|| sizecheck <= 0)
            {
                deaccesschunk(0, 0, 0);
                return SS$_BADIRECTORY;
            }
        }
        while (1);

        if (invalid_dr)
        {
            panic("BUGCHECK invalid dr\n");
        }
    }

    /* If not enough space free extend the directory... */

    if (addlen > MAXREC - inuse)
    {
        struct ext2_dir_entry_2 *nr;
        unsigned keep_new = 0;
        char *newbuf;
        unsigned newblk = eofblk + 1;
        printk("Splitting record... %d %d\n", dr, de);
        if (newblk > fcb->fcb$l_efblk)
        {
            panic("I can't extend a directory yet!!\n");
        }
        fcb->fcb$l_highwater = 0;
        sts = exttwo_accesschunk(fcb, newblk, &newbuf, NULL, 1, 0);
        if (sts & 1)
        {
            while (newblk > curblk + 1)
            {
                char *frombuf;
                sts = exttwo_accesschunk(fcb, newblk - 1, &frombuf, NULL, 1, 0);
                if ((sts & 1) == 0)
                    break;
                memcpy(newbuf, frombuf, BLOCKSIZE);
                ;
                sts = deaccesschunk(newblk, 1, 1);
                newbuf = frombuf;
                newblk--;
                if ((sts & 1) == 0)
                    break;
            }
        }
        else
        {
        }
        if ((sts & 1) == 0)
        {
            deaccesschunk(0, 0, 0);
            deaccesschunk(0, 0, 0);
            return sts;
        }
        memset(newbuf, 0, BLOCKSIZE);
        eofblk++;
        head = exttwo_read_header(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0,
                                  fcb, &iosb);
        sts = iosb.iosb$w_status;
        head->i_blocks = eofblk + 1;

        /* First find where the next record is... */

        nr = dr;
        if (le16_to_cpu(nr->rec_len))
            nr = (struct ext2_dir_entry_2 *) ((char *) nr
                                              + le16_to_cpu(nr->rec_len));

        /* Can we split between records? */

        if (de == NULL || (char *) dr != buffer || le16_to_cpu(nr->rec_len))
        {
            struct ext2_dir_entry_2 *sp = dr;
            if ((char *) dr == buffer && de != NULL )
                sp = nr;
            memcpy(newbuf, sp, ((buffer + BLOCKSIZE) - (char *) sp));
            memset(sp, 0, ((buffer + BLOCKSIZE) - (char *) sp));
            sp->rec_len = cpu_to_le16(0xffff);
            if (sp == dr
                    && (de != NULL || (char *) sp >= buffer + MAXREC - addlen))
            {
                dr = (struct ext2_dir_entry_2 *) (newbuf
                                                  + ((char *) dr - (char *) sp));
                keep_new = 1;
            }
            /* OK, we have to split the record then.. */

        }
        else
        {
            unsigned reclen = EXT2_DIR_REC_LEN(dr->name_len);
            struct ext2_dir_entry_2 *nbr = (struct ext2_dir_entry_2 *) newbuf;
            printk("Super split %d %d\n", dr, de);
            memcpy(newbuf, buffer, reclen);
            memcpy(newbuf + reclen, de, ((char *) nr - (char *) de) + 2);
            nbr->rec_len = cpu_to_le16(
                               reclen + ((char *) nr - (char *) de) - 2);

            memset((char *) de + 2, 0, ((char *) nr - (char *) de));
            ((struct ext2_dir_entry_2 *) de)->rec_len = cpu_to_le16(0xffff);
            dr->rec_len = cpu_to_le16(((char *) de - (char *) dr) - 2);
            if ((char *) de >= (char *) nr)
            {
                dr = (struct ext2_dir_entry_2 *) newbuf;
                keep_new = 1;
            }
        }

        /* Need to decide which buffer we are going to keep (to write to) */

        if (keep_new)
        {
            sts = deaccesschunk(curblk, 1, 1);
            curblk = newblk;
            buffer = newbuf;
        }
        else
        {
            sts = deaccesschunk(newblk, 1, 1);
        }
        if ((sts & 1) == 0)
            printk("Bad status %d\n", sts);
    }
    /* After that we can just add the record or entry as appropriate... */

    if (de == NULL )
    {
        memmove((char *) dr + addlen, dr,
                BLOCKSIZE - (((char *) dr + addlen) - buffer));
        dr->rec_len = cpu_to_le16(addlen);
        dr->name_len = filelen;
        memcpy(dr->name, filename, filelen);
    }
    else
    {
        dr->rec_len = cpu_to_le16(le16_to_cpu(dr->rec_len) + addlen);
        memmove((char *) de + addlen, de,
                BLOCKSIZE - (((char *) de + addlen) - buffer));
    }

    /* Write the entry values are we are done! */

#if 0
    fid_copy(&de->dir$fid,fid,0);
#else
    dr->inode = FID_TO_INO(fid);
    printk("fidi2 %x\n", dr->inode);
#endif
    writechunk(fcb, curblk, buffer);
    return deaccesschunk(curblk, 1, 1);
}

/* exttwo_delete_ent() - delete a directory entry */

unsigned exttwo_delete_ent(struct _fcb * fcb, unsigned curblk,
                           struct ext2_dir_entry_2 * dr, struct _dir1 * de, char *buffer,
                           unsigned eofblk)
{
    struct _iosb iosb;
    unsigned sts = 1;
    unsigned ent;
    struct ext2_inode * head;
    ent = 1;
    if (ent > 1)
    {
    }
    else
    {
        char *nr = (char *) dr + le16_to_cpu(dr->rec_len);
        if (eofblk == 1 || (char *) dr > buffer
                || (nr <= buffer + MAXREC && (unsigned short) *nr < BLOCKSIZE))
        {
            memcpy(dr, nr, BLOCKSIZE - (nr - buffer));
        }
        else
        {
            while (curblk < eofblk)
            {
                char *nxtbuffer;
                sts = exttwo_accesschunk(fcb, curblk + 1, &nxtbuffer, NULL, 1,
                                         0);
                if ((sts & 1) == 0)
                    break;
                memcpy(buffer, nxtbuffer, BLOCKSIZE);
                sts = deaccesschunk(curblk++, 1, 1);
                if ((sts & 1) == 0)
                    break;
                buffer = nxtbuffer;
            }
            if (sts & 1)
            {
                head = exttwo_read_header(
                           fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb,
                           &iosb);
                sts = iosb.iosb$w_status;
                head->i_blocks = cpu_to_le32(eofblk);
                eofblk--;
            }
        }
    }
    {
        unsigned retsts = deaccesschunk(curblk, 1, 1);
        if (sts & 1)
            sts = retsts;
        writechunk(fcb, curblk, buffer);
        return sts;
    }
}

/* return_ent() - return information about a directory entry */

unsigned exttwo_return_ent(struct _fcb * fcb, unsigned curblk,
                           struct ext2_dir_entry_2 * dr, struct _dir1 * de, struct _fibdef * fib,
                           unsigned short *reslen, struct dsc$descriptor * resdsc, int wildcard)
{
    struct _iosb iosb;
    int scale = 10;
    int version = 1;
    int length = dr->name_len;
    char *ptr = resdsc->dsc$a_pointer;
    int outlen = resdsc->dsc$w_length;
    if (length > outlen)
        length = outlen;
    {
        // another workaround
        if (dr->inode == 2 && length == 1)
        {
            length = 10;
            memcpy(ptr, "000000.DIR", 10);
            goto skip;
        }
        if (length == 1 && dr->name[0] == '.')
        {
            length = 3;
            memcpy(ptr, "DOT", 3);
            goto skip;
        }
        if (length == 2 && dr->name[0] == '.' && dr->name[1] == '.')
        {
            length = 6;
            memcpy(ptr, "DOTDOT", 6);
            goto skip;
        }
    }
    memcpy(ptr, dr->name, length);
skip:
    while (version >= scale)
        scale *= 10;
    ptr += length;
    if (length < outlen)
    {
        *ptr++ = ';';
        length++;
        do
        {
            if (length >= outlen)
                break;
            scale /= 10;
            *ptr++ = version / scale + '0';
            version %= scale;
            length++;
        }
        while (scale > 1);
    }
    *reslen = length;
#if 0
    fid_copy((struct _fiddef *)&fib->fib$w_fid_num,&de->dir$fid,0);
#else
    struct _fiddef * fid = &fib->fib$w_fid_num;
    SET_FID_FROM_INO(fid, dr->inode);
    exttwo_translate_ino(x2p->current_vcb, fid);
#if 0
    printk("fidi %x %x\n",ctl$gl_pcb,dr->inode);
#endif
#endif
    if (fib->fib$b_fid_rvn == 0)
        fib->fib$b_fid_rvn = fcb->fcb$b_fid_rvn;
    if (wildcard || (fib->fib$w_nmctl & FIB$M_WILD))
    {
        fib->fib$l_wcc = curblk;
    }
    else
    {
        fib->fib$l_wcc = 0;
    }
#if 0
    printk("x %x %s\n",length,resdsc->dsc$a_pointer);
#endif
    return deaccesschunk(0, 0, 1);
}

// equivalent of FIND

/* search_ent() - search for a directory entry */

unsigned exttwo_search_ent(struct _fcb * fcb, struct dsc$descriptor * fibdsc,
                           struct dsc$descriptor * filedsc, unsigned short *reslen,
                           struct dsc$descriptor * resdsc, unsigned eofblk, unsigned action)
{
    struct _iosb iosb;
    unsigned sts, curblk;
    char *searchspec, *buffer = 0;
    int searchlen, version, wildcard, wcc_flag;
    struct _fibdef *fib = (struct _fibdef *) fibdsc->dsc$a_pointer;

    /* 1) Generate start block (wcc gives start point)
     2) Search for start
     3) Scan until found or too big or end   */

    curblk = fib->fib$l_wcc;
    if (curblk != 0)
    {
        searchspec = resdsc->dsc$a_pointer;
        sts = exttwo_name_check(searchspec, *reslen, &searchlen, &version,
                                &wildcard);
        if (action || wildcard)
            sts = SS$_BADFILENAME;
        wcc_flag = 1;
    }
    else
    {
        searchspec = filedsc->dsc$a_pointer;
        sts = exttwo_name_check(searchspec, filedsc->dsc$w_length, &searchlen,
                                &version, &wildcard);
        if ((action && wildcard) || (action > 1 && version < 0))
            sts = SS$_BADFILENAME;
        wcc_flag = 0;
    }
#if 0
    printk("match %x %x %s\n",ctl$gl_pcb,searchspec,searchspec);
#endif
    if ((sts & 1) == 0)
        return sts;

    /* Identify starting block...*/

    if (*searchspec == '*' || *searchspec == '%')
    {
        curblk = 1;
    }
    unsigned loblk = 1, hiblk = eofblk;
    if (curblk < 1 || curblk > eofblk)
        curblk = 1;

    /* Now to read sequentially to find entry... */

    char last_name[80];
    unsigned last_len = 0;
    while ((sts & 1) && curblk < eofblk)   // check. was <=
    {
        struct ext2_dir_entry_2 *dr;

        /* Access a directory block. Reset relative version if it starts
         with a record we haven't seen before... */

        sts = exttwo_accesschunk(fcb, curblk, &buffer, NULL, action ? 1 : 0, 0);
        if ((sts & 1) == 0)
            return sts;
        dr = (struct ext2_dir_entry_2 *) buffer;

        /* Now loop through the records seeing which match our spec... */

        do
        {
            char *nr = (char *) dr + le16_to_cpu(dr->rec_len);
#if 0
            printk("dr nr %x %x %x %x %x %x \n",dr,nr,buffer + BLOCKSIZE,dr >= buffer + BLOCKSIZE,curblk,eofblk);
#endif
#if 0
            // not here yet?
            if (nr >= buffer + BLOCKSIZE) break;
#else
#if 0
            if (dr >= buffer + BLOCKSIZE) break;
#endif
#endif
            if (dr->name + dr->name_len > nr)
                break;
            sts = exttwo_name_match(searchspec, searchlen, dr->name,
                                    dr->name_len, dr->inode);
            if (sts == MAT_SKIP)
            {
                goto skip;
            }
            if (sts == MAT_EQ && wcc_flag)
            {
                wcc_flag = 0;
                searchspec = filedsc->dsc$a_pointer;
                sts = exttwo_name_check(searchspec, filedsc->dsc$w_length,
                                        &searchlen, &version, &wildcard);
                if ((sts & 1) == 0)
                    break;
                goto skip;
            }
            else
            {
                /* Look at each directory entry to see
                 if it is what we want...    */

                /* Decide what to do with the entry we have found... */

                if (sts == MAT_EQ)
                {
                    switch (action)
                    {
                    case 0:
                    {
                        int ret = exttwo_return_ent(fcb, curblk, dr, 0, fib,
                                                    reslen, resdsc, wildcard);
                        kfree(buffer);
                        return ret;
                    }
                    case 1:
                    {
                        int ret = exttwo_delete_ent(fcb, curblk, dr, 0, buffer,
                                                    eofblk);
                        kfree(buffer);
                        return ret;
                    }
                    default:
                        sts = SS$_DUPFILENAME;
                        break;
                    }
                }
                else
                {
                    // check leak
                    if (sts != MAT_EQ && action == 2)
                    {
                        return exttwo_insert_ent(fcb, eofblk, curblk, buffer,
                                                 dr, 0, searchspec, searchlen, version,
                                                 (struct _fiddef *) &fib->fib$w_fid_num);
                    }
                }
            }
            /*  Finish unless we expect more... */

            /* If this is the last record in the block store the name
             so that if it continues into the next block we can get
             the relative version right! Sigh! */

#if 0
            if ((((struct ext2_dir_entry_2 *) nr)->inode) == 0) // check
            {
#else
#if 0
            printk("dr2 nr %x %x %x %x %x %x\n",dr,nr,buffer + BLOCKSIZE,nr >= buffer + BLOCKSIZE,curblk,eofblk);
#endif
skip:
            if (nr >= buffer + BLOCKSIZE)
            {
#endif
                last_len = dr->name_len;
                if (last_len > sizeof(last_name))
                    last_len = sizeof(last_name);
                memcpy(last_name, dr->name, last_len);
                dr = (struct ext2_dir_entry_2 *) nr;
                break;
            }
            dr = (struct ext2_dir_entry_2 *) nr;
        }
        while (1); /* dr records within block */

        /* We release the buffer ready to get the next one - unless it is the
         last one in which case we can't defer an insert any longer!! */

        if ((sts & 1) == 0 || action != 2 || (sts != MAT_GT && curblk < eofblk))
        {
            kfree(buffer);
            buffer = 0;
            unsigned dests = deaccesschunk(0, 0, 1);
            if ((dests & 1) == 0)
            {
                sts = dests;
                break;
            }
            curblk++;
            sts = 1;
        }
        else
        {
            // check leak
            if (version == 0)
                version = 1;
            return exttwo_insert_ent(fcb, eofblk, curblk, buffer, dr, NULL,
                                     searchspec, searchlen, version,
                                     (struct _fiddef *) &fib->fib$w_fid_num);
        }
    } /* curblk blocks within file */

    /* We achieved nothing! Report the failure... */

    if (sts & 1)
    {
        fib->fib$l_wcc = 0;
        if (wcc_flag || wildcard)
        {
            sts = SS$_NOMOREFILES;
        }
        else
        {
            sts = SS$_NOSUCHFILE;
        }
    }
    return sts;
}

/* direct() - this routine handles all directory manipulations:-
 action 0 - find directory entry
 1 - delete entry
 2 - create an entry   */

unsigned exttwo_direct(struct _vcb * vcb, struct dsc$descriptor * fibdsc,
                       struct dsc$descriptor * filedsc, unsigned short *reslen,
                       struct dsc$descriptor * resdsc, struct _atrdef * atrp, unsigned action,
                       struct _irp *i)
{
    struct _iosb iosb;
    struct _fcb *fcb;
    struct ext2_inode *head;
    unsigned sts, eofblk;
    struct _fibdef *fib = (struct _fibdef *) fibdsc->dsc$a_pointer;
    struct _fibdef dirfib;
    struct dsc$descriptor dirdsc;
    struct _irp * dummyirp = kmalloc(sizeof(struct _irp), GFP_KERNEL);
    memcpy(dummyirp, i, sizeof(struct _irp));
    dirdsc.dsc$w_length = sizeof(struct _fibdef);
    dirdsc.dsc$a_pointer = &dirfib;
    memcpy(&dirfib, fib, sizeof(struct _fibdef));
    dirfib.fib$w_fid_num = fib->fib$w_did_num;
    dirfib.fib$w_fid_seq = fib->fib$w_did_seq;
    dummyirp->irp$l_qio_p1 = &dirdsc;
    sts = exttwo_access(vcb, dummyirp);
    fcb = x2p->primary_fcb;
    if (sts & 1)
    {
        head = exttwo_read_header(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0,
                                  fcb, &iosb);
        sts = iosb.iosb$w_status;
        if (S_ISDIR(le16_to_cpu(head->i_mode)))
        {
            eofblk = le32_to_cpu(head->i_blocks);
            sts = exttwo_search_ent(fcb, fibdsc, filedsc, reslen, resdsc,
                                    eofblk, action);
        }
        else
        {
            sts = SS$_BADIRECTORY;
        }
        {
            unsigned dests = deaccessfile(fcb);
            if (sts & 1)
                sts = dests;
        }
    }
    iosbret(i, sts);
    return sts;
}
