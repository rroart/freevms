// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

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


#include <linux/vmalloc.h>
#include <stdio.h>
//#include <stdlib.h>
#include <linux/ctype.h>
#include <linux/string.h>
//#include "ssdef.h"
//#include "descrip.h"
//#include "fibdef.h"

#include <mytypes.h>
#include <atrdef.h>
#include <ccbdef.h>
#include <fatdef.h>
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
#include <fh2def.h>
#include <hm2def.h>
#include <dirdef.h>
#include <vmstime.h>
#include "../../freevms/f11x/src/xqp.h"
#include "cache.h"
#include "access.h"
#include "direct.h"

#define DEBUGx on
#define BLOCKSIZE 512
#define MAXREC (BLOCKSIZE - 2)


/* Some statistical counters... */

int direct_lookups = 0;
int direct_searches = 0;
int direct_deletes = 0;
int direct_inserts = 0;
int direct_splits = 0;
int direct_checks = 0;
int direct_matches = 0;


/* direct_show - to print directory statistics */

void direct_show(void)
{
    printk("DIRECT_SHOW Lookups: %d Searches: %d Deletes: %d Inserts: %d Splits: %d\n",
           direct_lookups,direct_searches,direct_deletes,direct_inserts,direct_splits);
}


/* name_check() - take a name specification and return name length without
               the version number, an integer version number, and a wildcard flag */

unsigned name_check(char *str,int len,int *retlen,int *retver,int *wildflag)
{
    int wildcard = 0;
    char *name_start = str;
    int dots = 0;
    char *name = name_start;
    char *name_end = name + len;
    direct_checks++;

    /* Go through the specification checking for illegal characters */

    while (name < name_end) {
        char ch = *name++;
        if (ch == '.') {
            if ((name - name_start) > 40) return SS$_BADFILENAME;
            name_start = name;
            if (dots++ > 1) break;
        } else {
            if (ch == ';') {
                break;
            } else {
                if (ch == '*' || ch == '%') {
                    wildcard = 1;
                } else {
                    if (ch == '[' || ch == ']' || ch == ':' ||
                        !isprint(ch)) return SS$_BADFILENAME;
                }
            }
        }
    }
    if ((name - name_start) > 40) return SS$_BADFILENAME;

    /* Return the name length and start checking the version */

    *retlen = name - str - 1;
    dots = 0;
    if (name < name_end) {
        char ch = *name;
        if (ch == '*') {
            if (++name < name_end) return SS$_BADFILENAME;
            dots = 32768;       /* Wildcard representation of version! */
            wildcard = 1;
        } else {
            int sign = 1;
            if (ch == '-') {
                name++;
                sign = -1;
            }
            while (name < name_end) {
                ch = *name++;
                if (!isdigit(ch)) return SS$_BADFILENAME;
                dots = dots * 10 + (ch - '0');
            }
            dots *= sign;
        }
    }
    *retver = dots;
    *wildflag = wildcard;
    return SS$_NORMAL;
}



#define MAT_LT 0
#define MAT_EQ 1
#define MAT_GT 2
#define MAT_NE 3

/* name_match() - compare a name specification with a directory entry
               and determine if there is a match, too big, too small... */

int name_match(char *spec,int spec_len,char *dirent,int dirent_len)
{
    int percent = MAT_GT;
    char *name = spec,*entry = dirent;
    char *name_end = name + spec_len,*entry_end = entry + dirent_len;
    direct_matches++;

    /* See how much name matches without wildcards... */

    while (name < name_end && entry < entry_end) {
        char sch = *name;
        if (sch != '*') {
            char ech = *entry;
                if (sch != ech) if (toupper(sch) != toupper(ech))
                    if (sch == '%') {
                        percent = MAT_NE;
                    } else {
                        break;
                    }
        } else {
            break;
        }
        name++;
        entry++;
    }

    /* Mismatch - return result unless wildcard... */

    if (name >= name_end) {
        if (entry >= entry_end) {
            return MAT_EQ;
        } else {
            return percent;
        }
    } else {

        /* See if we can find a match with wildcards */

        if (*name != '*') {
            if (percent == MAT_NE) return MAT_NE;
            if (entry < entry_end)
                if (toupper(*entry) > toupper(*name)) return MAT_GT;
            return MAT_LT;
        }
        /* Strip out wildcard(s) - if end then we match! */

        do {
            name++;
        } while (name < name_end && *name == '*');
        if (name >= name_end) return MAT_EQ;

        /* Proceed to examine the specification past the wildcard... */

        while (name < name_end) {
            int offset = 1;
            char fch = toupper(*name++);

            /* See if can can find a match for the first character... */

            if (fch != '%') {
                while (entry < entry_end) {
                    if (toupper(*entry) != fch) {
                        entry++;
                    } else {
                        break;
                    }
                }
            }
            /* Give up if we can't find that one lousy character... */

            if (entry >= entry_end) return MAT_NE;
            entry++;

            /* See how much of the rest we can match... */

            while (name < name_end && entry < entry_end) {
                char sch = *name,ech;
                if (sch == '*') break;
                    if (sch != (ech = *entry)) if (toupper(sch) != toupper(ech))
                        if (sch != '%') break;
                name++;
                entry++;
                offset++;
            }

            /* If matching died because of a wildcard we are OK... */

            if (name < name_end && *name == '*') {
                do {
                    name++;
                } while (name < name_end && *name == '*');
                if (name >= name_end) return MAT_EQ;

                /* Otherwise we finished OK or we need to try again... */

            } else {
                if (name >= name_end && entry >= entry_end) return MAT_EQ;
                name -= offset;
                entry -= offset - 1;
            }
        }
    }

    /* No more specification - match depends on remainder of entry... */

    if (entry < entry_end) return MAT_NE;
    return MAT_EQ;
}


/* insert_ent() - procedure to add a directory entry at record dr entry de */

unsigned insert_ent(struct _fcb * fcb,unsigned eofblk,unsigned curblk,
                    char *buffer,
                    struct _dir * dr,struct _dir1 * de,
                    char *filename,unsigned filelen,
                    unsigned version,struct _fiddef * fid)
{
  struct _iosb iosb;
    unsigned sts = 1;
    int inuse = 0;
    struct _fh2 * head;

    /* Compute space required... */

    int addlen = sizeof(struct _dir1);
    direct_inserts++;
    if (de == NULL)
        addlen += (filelen + sizeof(struct _dir)) & ~1;

    /* Compute block space in use ... */

    {
        char invalid_dr = 1;
        do {
            int sizecheck;
            struct _dir *nr = (struct _dir *) (buffer + inuse);
            if (dr == nr) invalid_dr = 0;
            sizecheck = VMSWORD(nr->dir$w_size);
            if (sizecheck == 0xffff)
                break;
            sizecheck += 2;
            inuse += sizecheck;
            sizecheck -= (nr->dir$b_namecount + sizeof(struct _dir)) & ~1;
            if (inuse > MAXREC || (inuse & 1) || sizecheck <= 0 ||
                sizecheck % sizeof(struct _dir1) != 0) {
                deaccesschunk(0,0,0);
                return SS$_BADIRECTORY;
            }
        } while (1);

        if (invalid_dr) {
            panic("BUGCHECK invalid dr\n");
        }
        if (de != NULL) {
            if (VMSWORD(dr->dir$w_size) > MAXREC || (char *) de < dr->dir$t_name +
                dr->dir$b_namecount || (char *) de > (char *) dr + VMSWORD(dr->dir$w_size) + 2) {
                panic("BUGCHECK invalid de\n");
            }
        }
    }

    /* If not enough space free extend the directory... */

    if (addlen > MAXREC - inuse) {
        struct _dir *nr;
        unsigned keep_new = 0;
        char *newbuf;
        unsigned newblk = eofblk + 1;
        direct_splits++;
        printk("Splitting record... %d %d\n",dr,de);
        if (newblk > fcb->fcb$l_efblk) {
            panic("I can't extend a directory yet!!\n");
        }
        fcb->fcb$l_highwater = 0;
        sts = accesschunk(fcb,newblk,&newbuf,NULL,1,0);
        if (sts & 1) {
            while (newblk > curblk + 1) {
                char *frombuf;
                sts = accesschunk(fcb,newblk - 1,&frombuf,NULL,1,0);
                if ((sts & 1) == 0) break;
                memcpy(newbuf,frombuf,BLOCKSIZE);;
                sts = deaccesschunk(newblk,1,1);
                newbuf = frombuf;
                newblk--;
                if ((sts & 1) == 0) break;
            }
        } else {
        }
        if ((sts & 1) == 0) {
            deaccesschunk(0,0,0);
            deaccesschunk(0,0,0);
            return sts;
        }
        memset(newbuf,0,BLOCKSIZE);
        eofblk++;
	head = f11b_read_header (fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb, &iosb);  
	sts=iosb.iosb$w_status;
        head->fh2$w_recattr.fat$l_efblk = VMSWORD(eofblk + 1);

        /* First find where the next record is... */

        nr = dr;
        if (VMSWORD(nr->dir$w_size) <= MAXREC)
            nr = (struct _dir *) ((char *) nr + VMSWORD(nr->dir$w_size) + 2);

        /* Can we split between records? */

        if (de == NULL || (char *) dr != buffer || VMSWORD(nr->dir$w_size) <= MAXREC) {
            struct _dir *sp = dr;
            if ((char *) dr == buffer && de != NULL) sp = nr;
            memcpy(newbuf,sp,((buffer + BLOCKSIZE) - (char *) sp));
            memset(sp,0,((buffer + BLOCKSIZE) - (char *) sp));
            sp->dir$w_size = VMSWORD(0xffff);
            if (sp == dr && (de != NULL || (char *) sp >= buffer + MAXREC - addlen)) {
                if (de != NULL)
                    de = (struct _dir1 *)
                        (newbuf + ((char *) de - (char *) sp));
                dr = (struct _dir *) (newbuf + ((char *) dr - (char *) sp));
                keep_new = 1;
            }
            /* OK, we have to split the record then.. */

        } else {
            unsigned reclen = (dr->dir$b_namecount +
                                        sizeof(struct _dir)) & ~1;
            struct _dir *nbr = (struct _dir *) newbuf;
            printk("Super split %d %d\n",dr,de);
            memcpy(newbuf,buffer,reclen);
            memcpy(newbuf + reclen,de,((char *) nr - (char *) de) + 2);
            nbr->dir$w_size = VMSWORD(reclen + ((char *) nr - (char *) de) - 2);

            memset((char *) de + 2,0,((char *) nr - (char *) de));
            ((struct _dir *) de)->dir$w_size = VMSWORD(0xffff);
            dr->dir$w_size = VMSWORD(((char *) de - (char *) dr) - 2);
            if ((char *) de >= (char *) nr) {
                dr = (struct _dir *) newbuf;
                de = (struct _dir1 *) (newbuf + reclen);
                keep_new = 1;
            }
        }

        /* Need to decide which buffer we are going to keep (to write to) */

        if (keep_new) {
            sts = deaccesschunk(curblk,1,1);
            curblk = newblk;
            buffer = newbuf;
        } else {
            sts = deaccesschunk(newblk,1,1);
        }
        if ((sts & 1) == 0) printk("Bad status %d\n",sts);
    }
    /* After that we can just add the record or entry as appropriate... */

    if (de == NULL) {
        memmove((char *) dr + addlen,dr,BLOCKSIZE - (((char *) dr + addlen) - buffer));
        dr->dir$w_size = VMSWORD(addlen - 2);
        dr->dir$w_verlimit = 0;
        dr->dir$b_flags = 0;
        dr->dir$b_namecount = filelen;
        memcpy(dr->dir$t_name,filename,filelen);
        de = (struct _dir1 *) ((char *) dr + (addlen - sizeof(struct _dir1)));
    } else {
        dr->dir$w_size = VMSWORD(VMSWORD(dr->dir$w_size) + addlen);
        memmove((char *) de + addlen,de,BLOCKSIZE - (((char *) de + addlen) - buffer));
    }

    /* Write the entry values are we are done! */

    de->dir$w_version = VMSWORD(version);
    fid_copy(&de->dir$fid,fid,0);
    return deaccesschunk(curblk,1,1);
}


/* delete_ent() - delete a directory entry */

unsigned delete_ent(struct _fcb * fcb,unsigned curblk,
                    struct _dir * dr,struct _dir1 * de,
                    char *buffer,unsigned eofblk)
{
  struct _iosb iosb;
    unsigned sts = 1;
    unsigned ent;
    struct _fh2 * head;
    direct_deletes++;
    ent = (VMSWORD(dr->dir$w_size) - sizeof(struct _dir)
           - dr->dir$b_namecount + 3) / sizeof(struct _dir1);
    if (ent > 1) {
        char *ne = (char *) de + sizeof(struct _dir1);
        memcpy(de,ne,BLOCKSIZE - (ne - buffer));
        dr->dir$w_size = VMSWORD(VMSWORD(dr->dir$w_size) - sizeof(struct _dir1));
    } else {
        char *nr = (char *) dr + VMSWORD(dr->dir$w_size) + 2;
        if (eofblk == 1 || (char *) dr > buffer ||
            (nr <= buffer + MAXREC && (unsigned short) *nr < BLOCKSIZE)) {
            memcpy(dr,nr,BLOCKSIZE - (nr - buffer));
        } else {
            while (curblk < eofblk) {
                char *nxtbuffer;
                sts = accesschunk(fcb,curblk + 1,&nxtbuffer,NULL,1,0);
                if ((sts & 1) == 0) break;
                memcpy(buffer,nxtbuffer,BLOCKSIZE);
                sts = deaccesschunk(curblk++,1,1);
                if ((sts & 1) == 0) break;
                buffer = nxtbuffer;
            }
            if (sts & 1) {
	      head = f11b_read_header (fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb, &iosb);  
	      sts=iosb.iosb$w_status;
                head->fh2$w_recattr.fat$l_efblk = VMSSWAP(eofblk);
                eofblk--;
            }
        }
    }
    {
        unsigned retsts = deaccesschunk(curblk,1,1);
        if (sts & 1) sts = retsts;
        return sts;
    }
}


/* return_ent() - return information about a directory entry */

unsigned return_ent(struct _fcb * fcb,unsigned curblk,
                    struct _dir * dr,struct _dir1 * de,struct _fibdef * fib,
                    unsigned short *reslen,struct dsc$descriptor * resdsc,
                    int wildcard)
{
  struct _iosb iosb;
    int scale = 10;
    int version = VMSWORD(de->dir$w_version);
    int length = dr->dir$b_namecount;
    char *ptr = resdsc->dsc$a_pointer;
    int outlen = resdsc->dsc$w_length;
    if (length > outlen) length = outlen;
    memcpy(ptr,dr->dir$t_name,length);
    while (version >= scale) scale *= 10;
    ptr += length;
    if (length < outlen) {
        *ptr++ = ';';
        length++;
        do {
            if (length >= outlen) break;
            scale /= 10;
            *ptr++ = version / scale + '0';
            version %= scale;
            length++;
        } while (scale > 1);
    }
    *reslen = length;
    fid_copy((struct _fiddef *)&fib->fib$w_fid_num,&de->dir$fid,0);
    if (fib->fib$b_fid_rvn == 0) fib->fib$b_fid_rvn = fcb->fcb$b_fid_rvn;
    if (wildcard || (fib->fib$w_nmctl & FIB$M_WILD)) {
        fib->fib$l_wcc = curblk;
    } else {
        fib->fib$l_wcc = 0;
    }
    return deaccesschunk(0,0,1);
}

// equivalent of FIND

/* search_ent() - search for a directory entry */

unsigned search_ent(struct _fcb * fcb,
                    struct dsc$descriptor * fibdsc,struct dsc$descriptor * filedsc,
                    unsigned short *reslen,struct dsc$descriptor * resdsc,unsigned eofblk,unsigned action)
{
  struct _iosb iosb;
    unsigned sts,curblk;
    char *searchspec,*buffer;
    int searchlen,version,wildcard,wcc_flag;
    struct _fibdef *fib = (struct _fibdef *) fibdsc->dsc$a_pointer;
    direct_lookups++;

    /* 1) Generate start block (wcc gives start point)
       2) Search for start
       3) Scan until found or too big or end   */

    curblk = fib->fib$l_wcc;
    if (curblk != 0) {
        searchspec = resdsc->dsc$a_pointer;
        sts = name_check(searchspec,*reslen,&searchlen,&version,&wildcard);
        if (action || wildcard) sts = SS$_BADFILENAME;
        wcc_flag = 1;
    } else {
        searchspec = filedsc->dsc$a_pointer;
        sts = name_check(searchspec,filedsc->dsc$w_length,&searchlen,&version,&wildcard);
        if ((action && wildcard) || (action > 1 && version < 0)) sts = SS$_BADFILENAME;
        wcc_flag = 0;
    }
    if ((sts & 1) == 0) return sts;


    /* Identify starting block...*/

    if (*searchspec == '*' || *searchspec == '%') {
        curblk = 1;
    } else {
        unsigned loblk = 1,hiblk = eofblk;
        if (curblk < 1 || curblk > eofblk) curblk = (eofblk + 1) / 2;
        while (loblk < hiblk) {
            int cmp;
            unsigned newblk;
            struct _dir *dr;
            direct_searches++;
            sts = accesschunk(fcb,curblk,&buffer,NULL,action ? 1 : 0,0);
            if ((sts & 1) == 0) return sts;
            dr = (struct _dir *) buffer;
            if (VMSWORD(dr->dir$w_size) > MAXREC) {
                cmp = MAT_GT;
            } else {
                cmp = name_match(searchspec,searchlen,dr->dir$t_name,dr->dir$b_namecount);
                if (cmp == MAT_EQ) {
                    if (wildcard || version < 1 || version > 32767) {
                        cmp = MAT_NE;   /* no match - want to find start */
                    } else {
                        struct _dir1 *de =
                            (struct _dir1 *) (dr->dir$t_name + ((dr->dir$b_namecount + 1) & ~1));
                        if (VMSWORD(de->dir$w_version) < version) {
                            cmp = MAT_GT;       /* too far... */
                        } else {
                            if (VMSWORD(de->dir$w_version) > version) {
                                cmp = MAT_LT;   /* further ahead... */
                            }
                        }
                    }
                }
            }
            switch (cmp) {
                case MAT_LT:
                    if (curblk == fib->fib$l_wcc) {
                        newblk = hiblk = loblk = curblk;
                    } else {
                        loblk = curblk;
                        newblk = (loblk + hiblk + 1) / 2;
                    }
                    break;
                case MAT_GT:
                case MAT_NE:
                    newblk = (loblk + curblk) / 2;
                    hiblk = curblk - 1;
                    break;
                default:
                    newblk = hiblk = loblk = curblk;
            }
            if (newblk != curblk) {
                sts = deaccesschunk(0,0,1);
                if ((sts & 1) == 0) return sts;
                curblk = newblk;
            }
        }
    }


    /* Now to read sequentially to find entry... */

    {
        char last_name[80];
        unsigned last_len = 0;
        int relver = 0;
        while ((sts & 1) && curblk <= eofblk) {
            struct _dir *dr;
            int cmp = MAT_LT;

            /* Access a directory block. Reset relative version if it starts
               with a record we haven't seen before... */

	    sts = accesschunk(fcb,curblk,&buffer,NULL,action ? 1 : 0,0);
                if ((sts & 1) == 0) return sts;

            dr = (struct _dir *) buffer;
            if (last_len != dr->dir$b_namecount) {
                relver = 0;
            } else {
                if (name_match(last_name,last_len,dr->dir$t_name,last_len) != MAT_EQ) {
                    relver = 0;
                }
            }

            /* Now loop through the records seeing which match our spec... */

            do {
                char *nr = (char *) dr + VMSWORD(dr->dir$w_size) + 2;
                if (nr >= buffer + BLOCKSIZE) break;
                if (dr->dir$t_name + dr->dir$b_namecount >= nr) break;
                cmp = name_match(searchspec,searchlen,dr->dir$t_name,dr->dir$b_namecount);
                if (cmp == MAT_GT && wcc_flag) {
                    wcc_flag = 0;
                    searchspec = filedsc->dsc$a_pointer;
                    sts = name_check(searchspec,filedsc->dsc$w_length,&searchlen,&version,&wildcard);
                    if ((sts & 1) == 0) break;
                } else {
                    if (cmp == MAT_EQ) {
                        struct _dir1 *de = (struct _dir1 *) (dr->dir$t_name +
                                                                          ((dr->dir$b_namecount + 1) & ~1));
                        if (version == 0 && action == 2) {
                            version = VMSWORD(de->dir$w_version) + 1;
                            if (version > 32767) {
                                sts = SS$_BADFILENAME;
                                break;
                            }
                        }
                        /* Look at each directory entry to see
                           if it is what we want...    */

                        if ((char *) dr != buffer) relver = 0;
                        cmp = MAT_LT;
                        while ((char *) de < nr) {
                            if ((version < 1) ? (relver > version) : (version < VMSWORD(de->dir$w_version))) {
                                relver--;
                                de++;
                            } else {
                                if (version > 32767 || version == relver || version == VMSWORD(de->dir$w_version)) {
                                    cmp = MAT_EQ;
                                } else {
                                    cmp = MAT_GT;
                                }
                                if (wcc_flag == 0) {
                                    break;
                                } else {
                                    wcc_flag = 0;
                                    searchspec = filedsc->dsc$a_pointer;
                                    sts = name_check(searchspec,filedsc->dsc$w_length,&searchlen,&version,&wildcard);
                                    if ((sts & 1) == 0) break;
                                    if (name_match(searchspec,searchlen,dr->dir$t_name,
                                                   dr->dir$b_namecount) != MAT_EQ) {
                                        cmp = MAT_NE;
                                        break;
                                    }
                                    if (version < 0) {
                                        relver = -32768;
                                        cmp = MAT_GT;
                                        break;
                                    }
                                    if (cmp == MAT_EQ) {
                                        relver--;
                                        de++;
                                    }
                                    cmp = MAT_LT;
                                }
                            }
                        }
                        if ((sts & 1) == 0) break;

                        /* Decide what to do with the entry we have found... */

                        if (cmp == MAT_EQ) {
                            switch (action) {
                                case 0:
                                    return return_ent(fcb,curblk,dr,de,fib,reslen,resdsc,wildcard);
                                case 1:
                                    return delete_ent(fcb,curblk,dr,de,buffer,eofblk);
                                default:
                                    sts = SS$_DUPFILENAME;
                                    break;
                            }
                        } else {
                            if (cmp != MAT_NE && action == 2) {
                                return insert_ent(fcb,eofblk,curblk,buffer,dr,de,
                                                  searchspec,searchlen,version,(struct _fiddef *) & fib->fib$w_fid_num);
                            }
                        }
                    }
                    /*  Finish unless we expect more... */

                    if (cmp == MAT_GT && wildcard == 0) break;

                    /* If this is the last record in the block store the name
                       so that if it continues into the next block we can get
                       the relative version right! Sigh! */

                    if (VMSWORD(((struct _dir *) nr)->dir$w_size) > MAXREC) {
                        last_len = dr->dir$b_namecount;
                        if (last_len > sizeof(last_name)) last_len = sizeof(last_name);
                        memcpy(last_name,dr->dir$t_name,last_len);
                        dr = (struct _dir *) nr;
                        break;
                    }
                    dr = (struct _dir *) nr;
                }
            } while (1);        /* dr records within block */

            /* We release the buffer ready to get the next one - unless it is the
               last one in which case we can't defer an insert any longer!! */

            if ((sts & 1) == 0 || action != 2 || (cmp != MAT_GT && curblk < eofblk)) {
                unsigned dests = deaccesschunk(0,0,1);
                if ((dests & 1) == 0) {
                    sts = dests;
                    break;
                }
                curblk++;
            } else {
                if (version == 0) version = 1;
                return insert_ent(fcb,eofblk,curblk,buffer,dr,NULL,
                                  searchspec,searchlen,version,(struct _fiddef *) & fib->fib$w_fid_num);
            }
        }                       /* curblk blocks within file */
    }

    /* We achieved nothing! Report the failure... */

    if (sts & 1) {
        fib->fib$l_wcc = 0;
        if (wcc_flag || wildcard) {
            sts = SS$_NOMOREFILES;
        } else {
            sts = SS$_NOSUCHFILE;
        }
    }
    return sts;
}


/* direct() - this routine handles all directory manipulations:-
         action 0 - find directory entry
                1 - delete entry
                2 - create an entry   */

unsigned direct(struct _vcb * vcb,struct dsc$descriptor * fibdsc,
                struct dsc$descriptor * filedsc,unsigned short *reslen,
                struct dsc$descriptor * resdsc,struct _atrdef * atrp, unsigned action, struct _irp *i)
{
  struct _iosb iosb;
    struct _fcb *fcb;
    struct _fh2 *head;
    unsigned sts,eofblk;
    struct _fibdef *fib = (struct _fibdef *) fibdsc->dsc$a_pointer;
    struct _fibdef dirfib;
    struct dsc$descriptor dirdsc;
    struct _irp * dummyirp=vmalloc(sizeof(struct _irp));
    bcopy(i,dummyirp,sizeof(struct _irp));
    dirdsc.dsc$w_length=sizeof(struct _fibdef);
    dirdsc.dsc$a_pointer=&dirfib;
    memcpy(&dirfib,fib,sizeof(struct _fibdef));
    dirfib.fib$w_fid_num=fib->fib$w_did_num;
    dirfib.fib$w_fid_seq=fib->fib$w_did_seq;
    dummyirp->irp$l_qio_p1=&dirdsc;
    sts = f11b_access(vcb,dummyirp);
    fcb=xqp->primary_fcb;
    if (sts & 1) {
      head = f11b_read_header (fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb, &iosb);  
      sts=iosb.iosb$w_status;
        if (VMSLONG(head->fh2$l_filechar) & FH2$M_DIRECTORY) {
            eofblk = VMSSWAP(head->fh2$w_recattr.fat$l_efblk);
            if (VMSWORD(head->fh2$w_recattr.fat$w_ffbyte) == 0) --eofblk;
            sts = search_ent(fcb,fibdsc,filedsc,reslen,resdsc,eofblk,action);
        } else {
            sts = SS$_BADIRECTORY;
        }
        {
            unsigned dests = deaccessfile(fcb);
            if (sts & 1) sts = dests;
        }
    }
    iosbret(i,sts);
    return sts;
}
