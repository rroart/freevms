#ifndef sbkdef_h
#define sbkdef_h

#define SBK$K_LENGTH 32
#define SBK$C_LENGTH 32
	
struct _sbkdef {
  union  {
    unsigned int sbk$l_stlbn;
    struct  {
      unsigned short int sbk$w_stlbnh;
      unsigned short int sbk$w_stlbnl;
    } sbk$r_stlbn_fields;
  } sbk$r_stlbn_overlay;
  union  {
    unsigned int sbk$l_filesize;
    struct  {
      unsigned short int sbk$w_filesizh;
      unsigned short int sbk$w_filesizl;
    } sbk$r_filesize_fields;
  } sbk$r_filesize_overlay;
  unsigned char sbk$b_acnt;
  unsigned char sbk$b_lcnt;
  unsigned int sbk$l_fcb;
  short int sbkdef$$_fill_1;
  unsigned short int sbk$w_acnt;
  unsigned short int sbk$w_lcnt;
  unsigned short int sbk$w_wcnt;
  unsigned short int sbk$w_tcnt;
  unsigned int sbk$l_reads;
  unsigned int sbk$l_writes;
};
 
#define		sbk$l_filesize		sbk$r_filesize_overlay.sbk$l_filesize
#define		sbk$l_stlbn		sbk$r_stlbn_overlay.sbk$l_stlbn
#define		sbk$w_filesizh		sbk$r_filesize_overlay.sbk$r_filesize_fields.sbk$w_filesizh
#define		sbk$w_filesizl		sbk$r_filesize_overlay.sbk$r_filesize_fields.sbk$w_filesizl
#define		sbk$w_stlbnh		sbk$r_stlbn_overlay.sbk$r_stlbn_fields.sbk$w_stlbnh
#define		sbk$w_stlbnl		sbk$r_stlbn_overlay.sbk$r_stlbn_fields.sbk$w_stlbnl

#endif
 
