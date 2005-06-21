/* Access.h v1.3    Definitions for file access routines */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

#define NO_DOLLAR

#ifdef FREEVMS_BIG_ENDIAN
#define VMSLONG(l) ((l & 0xff) << 24 | (l & 0xff00) << 8 | (l & 0xff0000) >> 8 | l >> 24)
#define VMSWORD(w) ((w & 0xff) << 8 | w >> 8)
#define VMSSWAP(l) ((l & 0xff0000) << 8 | (l & 0xff000000) >> 8 |(l & 0xff) << 8 | (l & 0xff00) >> 8)
#else
#define VMSLONG(l) l
#define VMSWORD(w) w
#define VMSSWAP(l) ((l & 0xffff) << 16 | l >> 16)
#endif

void fid_copy(struct _fiddef *dst,struct _fiddef *src,unsigned rvn);
unsigned device_lookup(unsigned devlen,char *devnam,int create,short int *retchan);

unsigned dismount(struct _vcb *vcb);
unsigned mount(unsigned flags,unsigned devices,char *devnam[],char *label[],struct _vcb **vcb);

unsigned accesserase(struct _vcb *vcb,struct _irp * irp);
unsigned deaccessfile(struct _fcb *fcb);
unsigned f11b_access(struct _vcb * vcb,struct _irp * irp);

unsigned deaccesschunk(unsigned wrtvbn,int wrtblks,int reuse);
unsigned accesschunk(struct _fcb *fcb,unsigned vbn,
                     char **retbuff,unsigned *retblocks,unsigned wrtblks, struct _irp * i);
unsigned access_extend(struct _fcb *fcb,unsigned blocks,unsigned contig);
unsigned update_freecount(struct _vcb *vcbdev,unsigned *retcount);
unsigned update_create(struct _vcb *vcb,struct _irp * i);
unsigned update_extend(struct _fcb *fcb,unsigned blocks,unsigned contig);
unsigned short checksum(vmsword *block);
