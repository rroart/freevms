#ifndef DFU_H
#define DFU_H

#include <descrip.h>
#include <fh2def.h>

struct f_id;

typedef unsigned long Boolean;

void singlemsg(int stat1, int stat);
void put_disp(void);
void put_status(int x);
int dfu_check_access(int *mask);
int exit_command(int mask);
int spawn_command(int mask);
int SYS$EXPREG(int pagcnt, long ret[2], int a, int b);
int SYS$DELTVA(long in[2], long ret[2], int a);
int strindex(char *input_string, char *symbol, int len);
void disass_map_ptr(struct _fh2 *hdr, int *j);
int parse_item(char *inp, struct dsc$descriptor *return_descr, int *convert, Boolean *flag, int typ);
void do_abort(void);
int movefile(char *defr_file, int flag);
void copy_fid(struct f_id *target, struct f_id *from, Boolean check_rvn);
int compdir(char *comp_file, Boolean matoutput);
int verify_header(struct _fh2 *fh2);
int follow_extents(struct _fh2 *hdr, int *rtv, int *num_hdrs);
int scan_bitmap(short chan, int pages, Boolean locked, int *free, int *large, int *blocks, int *large_lbn, int *table);
int SYS$DCLEXH();
int read_bitmap(short chan, int xpages, Boolean locked, int *addr);
int rebuild_bitmap(struct _fh2 *hdr, int *addr, int cluster, int curvol, Boolean check_only);
int SYS$CANEXH();
int make_syslost(struct f_id *l_fid);
void set_bitmap(int * addr, int cluster, int lbn_start, int lbn_count, Boolean mbs);
int compare_bitmap(short chan, int *addr, int pages, Boolean locked, int cluster, FILE *fp, Boolean rebuild, Boolean matoutput);
int delete_file(unsigned short id[3], unsigned short dchan, Boolean noremove, Boolean deldir, Boolean nolog, Boolean rem_file);
int scan_directories(int *rvt, int rvn, int cnt, int max, Boolean matoutput);

#endif
