#ifndef rdtdef_h
#define rdtdef_h

#define RDT$C_LENGTH 40
#define RDT$S_RDTDEF 41
	
struct _rdt {
  char rdtdef$fill;
};

#define CRDT$S_CRDTDEF 40
	
struct _crdt {
  unsigned char crdt$b_scs_maint_block [16];
  void *crdt$l_waitfl;
  void *crdt$l_waitbl;
  unsigned short int crdt$w_size;
  unsigned char crdt$b_type;
  unsigned char crdt$b_subtyp;
  void *crdt$l_freerd;
  unsigned int crdt$l_maxrdidx;
  unsigned int crdt$l_qrdt_cnt;
};

#endif
