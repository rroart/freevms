#ifndef x2p_h
#define x2p_h

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
unsigned int lb_lockid_idx;
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

extern struct __exttwo *x2p;

extern void * global_e2_vcb;

struct ext2_inode;

char * exttwo_vms_to_unix(char * name,struct dsc$descriptor * dsc);
void * exttwo_search_fcb2(struct _vcb * vcb,struct _fiddef * fid);
unsigned exttwo_access(struct _vcb * vcb, struct _irp * irp);
void * exttwo_read_block(struct _vcb * vcb, unsigned long lbn, unsigned long count, struct _iosb * iosb);
void * exttwo_write_block(struct _vcb * vcb, unsigned char * buf, unsigned long lbn, unsigned long count, struct _iosb * iosb);
void * exttwo_read_block(struct _vcb * vcb, unsigned long lbn, unsigned long count, struct _iosb * iosb);
void * exttwo_write_block(struct _vcb * vcb, unsigned char * buf, unsigned long lbn, unsigned long count, struct _iosb * iosb);
void exttwo_read_attrib(struct _fcb * fcb, struct _atrdef * atrp);
void make_fcb(struct _fcb * inode);
void exttwo$dispatcher(void);
unsigned exttwo_extend(struct _fcb *fcb,unsigned blocks,unsigned contig);
unsigned exttwo_extend(struct _fcb *fcb,unsigned blocks,unsigned contig);
void * exttwo_search_fcb(struct _vcb * vcb,struct _fiddef * fid);
void exttwo_write_attrib(struct _fcb * fcb, struct _atrdef * atrp);
int exttwo_wcb_create_all(struct _vcb * vcb, struct _fcb * fcb);
unsigned deaccesschunk(unsigned wrtvbn, int wrtblks,int reuse);
void * exttwo_read_header(struct _vcb *vcb,struct _fiddef *fid,struct _fcb * fcb, unsigned long * retsts);
void *exttwo_fcb_create2(struct ext2_inode * head, int i_ino, unsigned *retsts);
unsigned deaccesschunk(unsigned wrtvbn,int wrtblks,int reuse);
unsigned exttwo_accesschunk(struct _fcb *fcb,unsigned vbn, char **retbuff,unsigned *retblocks,unsigned wrtblks, struct _irp * i);
unsigned deaccessfile(struct _fcb *fcb);
void fid_copy(struct _fiddef *dst,struct _fiddef *src,unsigned rvn);
unsigned exttwo_search_ent(struct _fcb * fcb,
                    struct dsc$descriptor * fibdsc,struct dsc$descriptor * filedsc,
			 unsigned short *reslen,struct dsc$descriptor * resdsc,unsigned eofblk,unsigned action);
int exttwo_read_writevb(struct _irp * i);
unsigned exttwo_create(struct _vcb *vcb,struct _irp * i);
unsigned exttwo_delete(struct _vcb * vcb,struct _irp * irp);
exttwo_modify(struct _vcb * vcb, struct _irp * irp);
int exttwo_io_done(struct _irp * i);
void exttwo_translate_fid (struct _vcb * vcb, struct _fiddef * fid);
void exttwo_translate_ino (struct _vcb * vcb, struct _fiddef * fid);

#endif
