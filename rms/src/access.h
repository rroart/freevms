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

#ifdef BIG_ENDIAN
#define VMSLONG(l) ((l & 0xff) << 24 | (l & 0xff00) << 8 | (l & 0xff0000) >> 8 | l >> 24)
#define VMSWORD(w) ((w & 0xff) << 8 | w >> 8)
#define VMSSWAP(l) ((l & 0xff0000) << 8 | (l & 0xff000000) >> 8 |(l & 0xff) << 8 | (l & 0xff00) >> 8)
#else
#define VMSLONG(l) l
#define VMSWORD(w) w
#define VMSSWAP(l) ((l & 0xffff) << 16 | l >> 16)
#endif

#define FH2$M_NOBACKUP   0x2
#define FH2$M_CONTIG     0x80
#define FH2$M_DIRECTORY  0x2000
#define FH2$M_MARKDEL    0x8000
#define FH2$M_ERASE      0x20000

#ifdef __ALPHA
#pragma member_alignment save
#pragma nomember_alignment
#endif

#ifdef __ALPHA
#pragma member_alignment restore
#endif

#define EXTMAX 20

struct WCB {
    struct CACHE cache;
    unsigned loblk,hiblk;       /* Range of window */
    unsigned hd_basevbn;        /* File blocks prior to header */
    unsigned hd_seg_num;        /* Header segment number */
    struct _fiddef hd_fid;       /* Header FID */
    unsigned short extcount;    /* Extents in use */
    unsigned phylen[EXTMAX];
    unsigned phyblk[EXTMAX];
    unsigned char rvn[EXTMAX];
};                              /* Window control block */


#define VIOC_CHUNKSIZE 4

struct VIOC {
    struct CACHE cache;
    struct FCB_not *fcb;            /* File this chunk is for */
    unsigned wrtmask;           /* Bit mask for writable blocks */
    unsigned modmask;           /* Bit mask for modified blocks */
    char data[VIOC_CHUNKSIZE][512];     /* Chunk data */
};                              /* Chunk of a file */


#define FCB_WRITE 1             /* FCB open for write... */

struct FCB_not {
    struct CACHE cache;
    struct VCB *vcb;            /* Volume this file is for */
    struct VIOC *headvioc;      /* Index file chunk for file header */
    struct _fh2 *head;          /* Pointer to header block */
    struct WCB *wcb;            /* Window control block tree */
    struct VIOC *vioc;          /* Virtual I/O chunk tree */
    unsigned headvbn;           /* vbn for file header */
    unsigned hiblock;           /* Highest block mapped */
    unsigned highwater;         /* First high water block */
    unsigned char status;       /* FCB status bits */
    unsigned char rvn;          /* Initial file relative volume */
};                              /* File control block */


struct DIRCACHE {
    struct CACHE cache;
    int dirlen;                 /* Length of directory name */
    struct _fiddef dirid;        /* File ID of directory */
    char dirnam[1];             /* Directory name */
};                              /* Directory cache entry */


#define VCB_WRITE 1

struct VCB {
    unsigned status;            /* Volume status */
    unsigned devices;           /* Number of volumes in set */
    struct FCB_not *fcb;            /* File control block tree */
    struct DIRCACHE *dircache;  /* Directory cache tree */
    struct VCBDEV {
        struct DEV *dev;        /* Pointer to device info */
        struct FCB_not *idxfcb;     /* Index file control block */
        struct FCB_not *mapfcb;     /* Bitmap file control block */
        unsigned clustersize;   /* Cluster size of the device */
        unsigned max_cluster;   /* Total clusters on the device */
	unsigned free_clusters;	/* Free clusters on disk volume */
        struct _hm2 home;       /* Volume home block */
    } vcbdev[1];                /* List of volumes devices */
};                              /* Volume control block */


struct DEV {
    struct CACHE cache;
    struct VCB *vcb;            /* Pointer to volume (if mounted) */
    struct file * handle;            /* Device physical I/O handle */
    unsigned status;            /* Device physical status */
    unsigned sectors;           /* Device physical sectors */
    unsigned sectorsize;        /* Device physical sectorsize */
    char devnam[1];             /* Device name */
};                              /* Device information */

void fid_copy(struct _fiddef *dst,struct _fiddef *src,unsigned rvn);
unsigned device_lookup(unsigned devlen,char *devnam,int create,struct DEV **retdev);

unsigned dismount(struct VCB *vcb);
unsigned mount(unsigned flags,unsigned devices,char *devnam[],char *label[],struct VCB **vcb);

unsigned accesserase(struct VCB *vcb,struct _fiddef *fid);
unsigned deaccessfile(struct FCB_not *fcb);
unsigned accessfile(struct VCB *vcb,struct _fiddef *fid,
                    struct FCB_not **fcb,unsigned wrtflg);

unsigned deaccesschunk(struct VIOC *vioc,unsigned wrtvbn,int wrtblks,int reuse);
unsigned accesschunk(struct FCB_not *fcb,unsigned vbn,struct VIOC **retvioc,
                     char **retbuff,unsigned *retblocks,unsigned wrtblks);
unsigned access_extend(struct FCB_not *fcb,unsigned blocks,unsigned contig);
unsigned update_freecount(struct VCBDEV *vcbdev,unsigned *retcount);
unsigned update_create(struct VCB *vcb,struct _fiddef *did,char *filename,
                       struct _fiddef *fid,struct FCB_not **fcb);
unsigned update_extend(struct FCB_not *fcb,unsigned blocks,unsigned contig);
unsigned short checksum(vmsword *block);
