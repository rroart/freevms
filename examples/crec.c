/* 
   RECORD OPERATIONS 
   
   This program looks up records by key and then 
   
   *  copies the record to an output file, 
   *  deletes the record, or 
   *  updates the record 
   */ 

#include        <rms>                /* defines for rabs and fabs    */ 
#include        <stdio>              /* defins printf...             */ 
#include        <string>     /* defines strlen               */ 
#include        <stdlib>     /* defines exit                 */ 
#include        <starlet>    /* defines sys$open et al       */ 

#include <xaballdef>
#include <xabkeydef>

#define INPUT_NAME      "INFILE.DAT" 
#define OUTPUT_NAME     "OUTFILE.DAT" 
#define RECORD_SIZE     132 
#define TYPING_SIZE     100 

struct  FAB     infab, outfab; 
struct  RAB     inrab, outrab; 

struct XABKEY mykey;
struct XABALL dst_all0, dst_all1;

error_exit (code, value) 
     long    code; 
     long    value; 
{            void lib$signal(); 
	     lib$signal (code, value); 
	     exit (0); 
	   } 

main () 
{ 
  char    record [RECORD_SIZE];           /* record buffer */ 
  char    key [RECORD_SIZE];              /* key buffer */ 
  char    choice [TYPING_SIZE];           /* typing buffer */ 
  long    status; 
  
  {
    bzero(&mykey,sizeof(struct XABKEY));
    mykey.xab$b_cod=XAB$C_KEY;
    mykey.xab$b_bln=XAB$C_KEYLEN;

    mykey.xab$b_ref=0;
    mykey.xab$b_dan=0;
    mykey.xab$w_dfl=1536;
    mykey.xab$v_dup=1;
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

    bzero(&dst_all0,sizeof(struct XABALL));
    dst_all0.xab$b_cod=XAB$C_ALL;
    dst_all0.xab$b_bln=XAB$C_ALLLEN;
    dst_all0.xab$b_aid=0;
    dst_all0.xab$l_alq=328;
    /* dst_all0.xab$b_aop=XAB$C_CBT;*/
    dst_all0.xab$b_bkz=4;
    dst_all0.xab$w_deq=112;
    dst_all0.xab$l_nxt=&dst_all1;

    bzero(&dst_all1,sizeof(struct XABALL));
    dst_all1.xab$b_cod=XAB$C_ALL;
    dst_all1.xab$b_bln=XAB$C_ALLLEN;
    dst_all1.xab$b_aid=1;
    dst_all1.xab$l_alq=8;
    /* dst_all0.xab$b_aop=XAB$C_CBT;*/
    dst_all1.xab$b_bkz=4;
    dst_all1.xab$w_deq=4;
    dst_all1.xab$l_nxt=0;

#if 0
int i;
unsigned char * c=&cc$rms_xabkey;
  for(i=0; i<sizeof(struct XABKEY); i++) {
    printf("%x ",c[i]);
  }
#endif
}

  /* set up input fab */ 
  infab = cc$rms_fab; 
  infab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_UPD | FAB$M_DEL; 
  infab.fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD 
    | FAB$M_SHRDEL;       /* read/write sharing */ 
  infab.fab$l_fna = (char *) &INPUT_NAME; /* logical name INFILE */ 
  infab.fab$b_fns = sizeof INPUT_NAME - 1; 
  
  
  /* set up output fab */ 
  outfab = cc$rms_fab; 
  outfab.fab$b_fac = FAB$M_PUT; 
  outfab.fab$l_fna = (char *) &OUTPUT_NAME; /* logical name OUTFILE */ 
  outfab.fab$b_fns = sizeof OUTPUT_NAME - 1; 
  outfab.fab$w_mrs = RECORD_SIZE;         /* record size */ 
  outfab.fab$b_org = FAB$C_IDX;           /* relative file was: REL */ 
  outfab.fab$b_rat = FAB$M_CR;            /* implied carriage eturn */ 
  outfab.fab$b_rfm = FAB$C_VAR;
  outfab.fab$l_xab = &mykey;
  
  /* set up input rab */ 
  inrab = cc$rms_rab; 
  inrab.rab$l_fab = &infab; 
  inrab.rab$b_rac = RAB$C_KEY;            /* key access */ 
  inrab.rab$b_krf = 0;                    /* access by primary key */ 
  inrab.rab$l_kbf = key;                  /* key buffer */ 
  inrab.rab$l_ubf = record;               /* record buffer */ 
  inrab.rab$w_usz = RECORD_SIZE;          /* maximum record size */ 
  
  /* set up output rab */ 
  outrab = cc$rms_rab; 
  /* outrab.rab$w_mbf = 3; */
  outrab.rab$b_rac = RAB$C_KEY;
  outrab.rab$l_fab = &outfab;           
  outrab.rab$l_rbf = record;             
  outrab.rab$w_rsz = RECORD_SIZE;          /* maximum record size */ 
  /* outrab.rab$b_rop = RAB$C_LOA; */                   /* access by primary key */ 
  /* record buffer */     
  
  
  /* open files and connect streams */ 
#if 1
  status = sys$open (&infab); 
  if (! (status & 1)) 
    error_exit (status, infab.fab$l_stv); 
  status = sys$connect (&inrab); 
  if (! (status & 1)) 
    error_exit (status, inrab.rab$l_stv); 
#endif
  status = sys$create (&outfab); 
  if (! (status & 1)) 
    error_exit (status, outfab.fab$l_stv); 
  status = sys$connect (&outrab); 
  if (! (status & 1)) 
    error_exit (status, outrab.rab$l_stv); 
  
  while (1) 
    { 
      
      /* get a key and a record */ 
      printf ("Please input key value: "); 
      gets (key);                     /* get key from user */ 
      if (feof (stdin))               /* stop on ctrl-Z */ 
	break; 
      inrab.rab$b_ksz = strlen (key); /* set key length */ 
#if 1
      status = sys$get (&inrab); 
      if (! (status & 1)) 
	error_exit (status, inrab.rab$l_stv); 
#endif
      
      /* display the record */ 
      record[inrab.rab$w_rsz] = '\0';                   
      printf("Record: {%s}\n", record); 
      
      /* choose what to do */ 
      printf ("Please choose C, D(elete), or U(pdate):"); 
      gets (choice);            /* get choice from user */ 
      if (feof (stdin))               /* stop on ctrl-Z */ 
	break; 
      switch (choice[0]) 
	{ 
	case 'c': 
	case 'C': 
	  printf("you chose copy\n");
	  /* copy the record */ 
	  outrab.rab$w_rsz = inrab.rab$w_rsz; 
	  /* out length = in length */ 
	  status = sys$put (&outrab); 
	  if (! (status & 1)) 
	    error_exit (status,
			outrab.rab$l_stv); 
	  break; 
	case 'd': 
	case 'D': 
    
	  printf("you chose delete\n");
	  /* delete */ 
	  status = sys$delete (&inrab); 
	  if (! (status & 1)) 
	    error_exit (status,
			inrab.rab$l_stv); 
	  break; 
	case 'u': 
	case 'U': 
	  /* get a new record */ 
	  printf("you chose update\n");
	  printf ("Please input record value: "); 
	  gets (record);	/* get record from user */ 
	  inrab.rab$w_rsz = strlen (record); 
	  /* set record length */ 
	  status = sys$update (&inrab); 
	  if (! (status & 1)) 
	    error_exit (status, inrab.rab$l_stv); 
	  break; 
	case 'n': 
	case 'N': 
	  printf("you chose new\n");
	  /* copy the record */ 
	  /*outrab.rab$w_rsz = inrab.rab$w_rsz; */
	  printf ("Please input record value: "); 
	  gets (record);	/* get record from user */ 
	  /*outrab.rab$w_rsz = strlen (record); */
	  /* out length = in length */ 
	  status = sys$put (&outrab); 
	  if (! (status & 1)) 
	    error_exit (status,
			outrab.rab$l_stv); 
	  break; 
	default: 
	  /* do nothing */                                  
	  break; 
	} 
    } 
  
  /* close files */ 
#if 1
  status = sys$close (&infab); 
  if (! (status & 1)) 
    error_exit (status, infab.fab$l_stv); 
#endif
  status = sys$close (&outfab); 
  if (! (status & 1)) 
    error_exit (status, outfab.fab$l_stv); 
} 
