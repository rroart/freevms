#ifndef ccbdef_h
#define ccbdef_h

#define CCB$M_AMB        0x1
#define CCB$M_IMGTMP     0x2
#define CCB$M_RDCHKDON   0x4
#define CCB$M_WRTCHKDON  0x8
#define CCB$M_LOGCHKDON  0x10
#define CCB$M_PHYCHKDON  0x20
#define CCB$M_NOREADACC  0x40
#define CCB$M_NOWRITEACC 0x80
#define CCB$M_CLONE      0x100
	
#define CCB$K_LENGTH 32
#define CCB$S_CCBDEF 32

struct _ccb {
  struct _ucb *ccb$l_ucb;
  struct _wcb *ccb$l_wind;
  union  {
    unsigned int ccb$l_sts;
    struct  {
      unsigned ccb$v_amb        : 1;
      unsigned ccb$v_imgtmp     : 1;
      unsigned ccb$v_rdchkdon   : 1;
      unsigned ccb$v_wrtchkdon  : 1;
      unsigned ccb$v_logchkdon  : 1;
      unsigned ccb$v_phychkdon  : 1;
      unsigned ccb$v_noreadacc  : 1;
      unsigned ccb$v_nowriteacc : 1;
      unsigned ccb$v_clone      : 1;
      unsigned ccb$v_fillme     : 7;
    };
  };
  char ccb$b_amod;
  unsigned char ccb$b_spare_1;
  unsigned short int ccb$w_spare_2;
  unsigned int ccb$l_ioc;
  struct _irp *ccb$l_dirp;
  union  {
    int ccb$l_chan;
    unsigned short int ccb$w_chan;
  };
  int ccb$l_reserved;
};
 
#endif
 
