#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <starlet.h>

#include <rabdef.h>
#include <fabdef.h>

#include <xaballdef.h>
#include <xabkeydef.h>

#define DO_INPUT 1
#define DO_OUTPUT 0
#undef DO_OUTPUT

struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
struct _rabdef cc$rms_rab = {NULL,NULL,NULL,NULL,0,0,0,{0,0,0}};

#define INPUT_NAME      "INFILE.DAT" 
#define OUTPUT_NAME     "OUTFILE.DAT" 
#define RECORD_SIZE     132 
#define TYPING_SIZE     100 

struct  _fabdef     infab, outfab; 
struct  _rabdef     inrab, outrab; 

struct _xabkeydef mykey;
struct _xaballdef dst_all0, dst_all1;

on_error_exit (long code, long value)  {
  void lib$signal(); 
  printf ("code %x value %x\n", code, value); 
  exit (0); 
} 

main () 
{ 
  char    record [RECORD_SIZE];
  char    key [RECORD_SIZE];
  char    choice [TYPING_SIZE];
  long    status; 
  
  bzero(&mykey,sizeof(struct _xabkeydef));
  mykey.xab$b_cod=XAB$C_KEY;
  mykey.xab$b_bln=XAB$C_KEYLEN;

  mykey.xab$b_ref=0;
  mykey.xab$b_dan=0;
  mykey.xab$w_dfl=1536;
  //mykey.xab$v_dup=1;
  mykey.xab$b_dtp=XAB$C_DSTG;
  mykey.xab$b_ian=1;
  mykey.xab$w_ifl=1536;
  mykey.xab$b_prolog=3;
  mykey.xab$w_pos0=12;
  mykey.xab$w_pos1=0;
  mykey.xab$w_pos2=11;
  mykey.xab$b_siz0=15;
  mykey.xab$b_siz1=1;
  mykey.xab$b_siz2=1;
  mykey.xab$l_nxt=&dst_all0;

  bzero(&dst_all0,sizeof(struct _xaballdef));
  dst_all0.xab$b_cod=XAB$C_ALL;
  dst_all0.xab$b_bln=XAB$C_ALLLEN;
  dst_all0.xab$b_aid=0;
  dst_all0.xab$l_alq=328;

  dst_all0.xab$b_bkz=4;
  dst_all0.xab$w_deq=112;
  dst_all0.xab$l_nxt=&dst_all1;

  bzero(&dst_all1,sizeof(struct _xaballdef));
  dst_all1.xab$b_cod=XAB$C_ALL;
  dst_all1.xab$b_bln=XAB$C_ALLLEN;
  dst_all1.xab$b_aid=1;
  dst_all1.xab$l_alq=8;

  dst_all1.xab$b_bkz=4;
  dst_all1.xab$w_deq=4;
  dst_all1.xab$l_nxt=0;

  infab = cc$rms_fab; 
  infab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_UPD | FAB$M_DEL; 
  infab.fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD | FAB$M_SHRDEL;
  infab.fab$l_fna = (char *) &INPUT_NAME;
  infab.fab$b_fns = sizeof INPUT_NAME - 1; 

  outfab = cc$rms_fab; 
  outfab.fab$b_fac = FAB$M_PUT; 
  outfab.fab$l_fna = (char *) &OUTPUT_NAME;
  outfab.fab$b_fns = sizeof OUTPUT_NAME - 1; 
  outfab.fab$w_mrs = RECORD_SIZE;
  outfab.fab$b_org = FAB$C_IDX;
  outfab.fab$b_rat = FAB$M_CR;
  outfab.fab$b_rfm = FAB$C_VAR;
  outfab.fab$l_xab = &mykey;

  inrab = cc$rms_rab; 
  inrab.rab$l_fab = &infab; 
  inrab.rab$b_rac = RAB$C_KEY;
  inrab.rab$b_krf = 0;
  inrab.rab$l_kbf = key;
  inrab.rab$l_ubf = record;
  inrab.rab$w_usz = RECORD_SIZE;

  outrab = cc$rms_rab; 
  outrab.rab$b_rac = RAB$C_KEY;
  outrab.rab$l_fab = &outfab;           
  outrab.rab$l_rbf = record;             
  outrab.rab$w_rsz = RECORD_SIZE;

#ifdef DO_INPUT
  status = sys$open (&infab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, infab.fab$l_stv); 
  status = sys$connect (&inrab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, inrab.rab$l_stv); 
#endif
#ifdef DO_OUTPUT
  status = sys$create (&outfab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, outfab.fab$l_stv); 
  status = sys$connect (&outrab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, outrab.rab$l_stv); 
#endif
  
  while (1) 
    { 
      printf ("Please input key value: "); 
      gets (key);
      if (feof (stdin))
	break; 
      inrab.rab$b_ksz = strlen (key);
#ifdef DO_INPUT
      status = sys$get (&inrab,0,0); 
      if (! (status & 1)) 
	on_error_exit (status, inrab.rab$l_stv); 
#endif
      record[inrab.rab$w_rsz] = '\0';                   
      printf("Record: {%s}\n", record); 
      printf ("Please choose C, D(elete), or U(pdate):"); 
      gets (choice);
      if (feof (stdin))
	break; 
      switch (choice[0]) 
	{ 
	case 'c': 
	case 'C': 
	  printf("you chose copy\n");
	  outrab.rab$w_rsz = inrab.rab$w_rsz; 
#ifdef DO_OUTPUT
	  status = sys$put (&outrab,0,0); 
	  if (! (status & 1)) 
	    on_error_exit (status,
			outrab.rab$l_stv); 
#endif
	  break; 
	case 'd': 
	case 'D': 
	  printf("you chose delete\n");

#ifdef DO_OUTPUT
	  status = sys$delete (&inrab,0,0); 
	  if (! (status & 1)) 
	    on_error_exit (status,
			inrab.rab$l_stv); 
#endif
	  break; 
	case 'u': 
	case 'U': 
	  printf("you chose update\n");
	  printf ("Please input record value: "); 
	  gets (record);
	  inrab.rab$w_rsz = strlen (record); 

#ifdef DO_OUTPUT
	  status = sys$update (&inrab,0,0); 
	  if (! (status & 1)) 
	    on_error_exit (status, inrab.rab$l_stv); 
#endif
	  break; 
	case 'n': 
	case 'N': 
	  printf("you chose new\n");
	  printf ("Please input record value: "); 
	  gets (record);
#ifdef DO_OUTPUT
	  status = sys$put (&outrab,0,0); 
	  if (! (status & 1)) 
	    on_error_exit (status,
			outrab.rab$l_stv); 
#endif
	  break; 
	default: 
	  printf("unrecognized choice\n");
	  break; 
	} 
    } 
  

#ifdef DO_INPUT
  status = sys$close (&infab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, infab.fab$l_stv); 
#endif
#ifdef DO_OUTPUT
  status = sys$close (&outfab,0,0); 
  if (! (status & 1)) 
    on_error_exit (status, outfab.fab$l_stv); 
#endif
} 
