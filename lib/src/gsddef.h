#ifndef gsddef_h
#define gsddef_h

#define GSD$K_LENGTH 49
#define GSD$C_LENGTH 49
#define GSD$K_EXTGSDLNG 61
#define GSD$C_EXTGSDLNG 61
#define GSD$S_GSDDEF 61
#define GSD$C_MAXNAMLEN 43

struct _gsd {
  struct _gsd *gsd$l_gsdfl;
  struct _gsd *gsd$l_gsdbl;
  unsigned short int gsd$w_size;
  unsigned char gsd$b_type;
  unsigned char gsd$b_fill;
  unsigned int gsd$l_hash;
  union  {
    unsigned int gsd$l_pcbuic;
    struct  {
      char gsddef$$_filler [2];
      unsigned short int gsd$w_pcbgrp;
    };
  };
  unsigned int gsd$l_filuic;
  unsigned int gsd$l_prot;
  unsigned int gsd$l_gstx;
  unsigned int gsd$l_ident;
  struct _orb *gsd$l_orb;
  union  {
    unsigned int gsd$l_ipid;
    unsigned int gsd$l_related_gstx;
  };
  unsigned int gsd$l_flags;
  union  {
    struct  {
      char gsd$t_gsdnam;
      char gsd$b_fillme2 [3];
    };
    unsigned int gsd$l_basepfn;
  };
  unsigned int gsd$l_pages;
  unsigned int gsd$l_refcnt;
  char gsd$t_pfngsdnam;
  char gsd$b_fillme [3];
};
 
#endif
 
