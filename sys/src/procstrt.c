// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/mm.h>
#include<descrip.h>
#include<dyndef.h>
#include<lnmdef.h>
#include<lnmstrdef.h>
#include<misc.h>
#include<system_service_setup.h>

struct _lnmth lnm_prc_dir_table_header;

struct _lnmx lnm_prc_dir_xlat = {
  lnmx$l_flags : LNMX$M_TERMINAL,
  lnmx$l_index : LNMX$C_TABLE,
  lnmx$l_next : 0
};

struct _lnmb lnm_prc_dir = {
  lnmb$l_flink: 0,
  lnmb$l_blink: 0,
  lnmb$w_size: sizeof(struct _lnmb),
  lnmb$b_type: DYN$C_LNM,
  lnmb$b_acmode: MODE_K_KERNEL,
  lnmb$l_lnmx : 0,
  lnmb$l_table : 0,
  lnmb$b_flags : LNM$M_NO_ALIAS|LNM$M_TABLE|LNM$M_NO_DELETE,
  lnmb$b_count : 22,
  lnmb$t_name : "LNM$PROCESS_DIRECTORY",
};

struct _lnmth lnm_prc_dir_table_header={
  lnmth$l_flags: LNMTH$M_DIRECTORY,
  lnmth$l_hash : 0,
  lnmth$l_orb  : 0,
  lnmth$l_name : 0,
  lnmth$l_parent : 0,
  lnmth$l_child : 0,
  lnmth$l_sibling : 0,
  lnmth$l_qtable : 0,
  lnmth$l_byteslm : 0x7fffffff,
  lnmth$l_bytes : 0x7fffffff
};

struct _lnmth lnm_prc_table_header;

struct _lnmx lnm_prc_xlat = {
  lnmx$l_flags : LNMX$M_TERMINAL,
  lnmx$l_index : LNMX$C_TABLE,
  lnmx$l_next : 0
};

struct _lnmb lnm_prc = {
  lnmb$l_flink: 0,
  lnmb$l_blink: 0,
  lnmb$w_size: sizeof(struct _lnmb),
  lnmb$b_type: DYN$C_LNM,
  lnmb$b_acmode: MODE_K_KERNEL,
  lnmb$l_lnmx : 0,
  lnmb$l_table : 0,
  lnmb$b_flags : LNM$M_NO_ALIAS|LNM$M_TABLE,
  lnmb$b_count : 18,
  lnmb$t_name : "LNM$PROCESS_TABLE",
};

struct _lnmth lnm_prc_table_header={
  lnmth$l_flags: 0,
  lnmth$l_hash : 0,
  lnmth$l_orb  : 0,
  lnmth$l_name : 0,
  lnmth$l_parent : 0,
  lnmth$l_child : 0,
  lnmth$l_sibling : 0,
  lnmth$l_qtable : 0,
  lnmth$l_byteslm : 0x7fffffff,
  lnmth$l_bytes : 0x7fffffff
};

void lnm_init_prc(struct _pcb * p) {

  /* this has to be done after malloc has been initialized */
  /* can possibly done with mallocs */

  unsigned long ahash;
  unsigned long * myhash;
  int status;

  struct lnmhshp * hash = kmalloc (sizeof(struct lnmhshp),GFP_KERNEL);
  bzero(hash,sizeof(struct lnmhshp));

  $DESCRIPTOR(mypartab,"LNM$PROCESS_DIRECTORY");

  $DESCRIPTOR(mytabnam,"LNM$PROCESS_TABLE");
  $DESCRIPTOR(sysinput,"SYS$INPUT");
  $DESCRIPTOR(sysoutput,"SYS$OUTPUT");

  struct item_list_3 itm[2];

  struct _lnmb * lnm$process_directory=&lnm_prc_dir;
  struct _lnmth * lnm$process_directory_table_header=&lnm_prc_dir_table_header;

  struct _lnmb * dir_lnmb = kmalloc(sizeof(struct _lnmb),GFP_KERNEL);
  struct _lnmx * dir_lnmx = kmalloc(sizeof(struct _lnmx),GFP_KERNEL);
  struct _lnmth * dir_lnmth = kmalloc(sizeof(struct _lnmth),GFP_KERNEL);
  struct _lnmb * tab_lnmb = kmalloc(sizeof(struct _lnmb),GFP_KERNEL);
  struct _lnmx * tab_lnmx = kmalloc(sizeof(struct _lnmx),GFP_KERNEL);
  struct _lnmth * tab_lnmth = kmalloc(sizeof(struct _lnmth),GFP_KERNEL);
  
  memcpy(dir_lnmb, &lnm_prc_dir, sizeof(struct _lnmb));
  memcpy(dir_lnmx, &lnm_prc_dir, sizeof(struct _lnmx));
  memcpy(dir_lnmth, &lnm_prc_dir, sizeof(struct _lnmth));
  memcpy(tab_lnmb, &lnm_prc_dir, sizeof(struct _lnmb));
  memcpy(tab_lnmx, &lnm_prc_dir, sizeof(struct _lnmx));
  memcpy(tab_lnmth, &lnm_prc_dir, sizeof(struct _lnmth));

  p->pcb$l_ns_reserved_q1 = dir_lnmb; // instead of ctl$gl_lnmdirect
  p->pcb$l_affinity_callback = hash;

  // not yet ready for this?
  //lnm$inslogtab(ret_lnm, &sys_table);
  // then do it manually?

  dir_lnmth->lnmth$l_child = tab_lnmth;
  tab_lnmth->lnmth$l_parent = dir_lnmth;
  dir_lnmth->lnmth$l_hash = hash;
  tab_lnmth->lnmth$l_hash = hash;
  tab_lnmb->lnmb$l_table=tab_lnmth; // beware this and over

  itm[0].item_code=1;
  itm[0].buflen=6;
  itm[0].bufaddr="opa0:";
  bzero(&itm[1],sizeof(struct item_list_3));

  exe$crelnm(0,&mytabnam,&sysinput,0,itm);
  exe$crelnm(0,&mytabnam,&sysoutput,0,itm);

  //lnm$al_dirtbl[1]=&lnm_prc_dir;

  /*ctl$gl_lnmdirect=LNM$PROCESS_DIRECTORY;
    lnm$al_dirtbl[0]=LNM$SYSTEM_DIRECTORY;
    lnm$al_dirtbl[1]=ctl$gl_lnmdirect;*/
  myhash=&ahash; //lnmmalloc(sizeof(unsigned long));
  status=lnm$hash(mypartab.dsc$w_length,mypartab.dsc$a_pointer,0xffff,myhash);
#ifdef LNM_DEBUG 
  lnmprintf("here %x %x\n",myhash,*myhash);
#endif

  hash->entry[2*(*myhash)]=dir_lnmb;
  hash->entry[2*(*myhash)+1]=dir_lnmb;

  status=lnm$hash(mytabnam.dsc$w_length,mytabnam.dsc$a_pointer,0xffff,myhash);
#ifdef LNM_DEBUG 
  lnmprintf("here %x %x\n",myhash,*myhash);
#endif

  hash->entry[2*(*myhash)]=tab_lnmb;
  hash->entry[2*(*myhash)+1]=tab_lnmb;

  dir_lnmb->lnmb$l_flink=dir_lnmb;
  dir_lnmb->lnmb$l_blink=dir_lnmb;
  tab_lnmb->lnmb$l_flink=tab_lnmb;
  tab_lnmb->lnmb$l_blink=tab_lnmb;
}

int exe$procstrt(struct _pcb * p) {
  // get pcb and copy pqb
  // store rms dispatcher address
  // initialize dispatch vector for system services
  // init msg vect
  // propagate image dump flag
  // init krp
  // set cpu and ast limits
  // init wslist
  // copy b_pri to b_authpri
  // copy priv mask
  // copy msg flags to p1
  // save logintime in ctl$gq_login
  // copy security clearance
  // init getjpi queue
  // init image activator listheads
  // init cwps queue
  // init process_scan queue
  // create p1
  // init logical hash table
  // init lnm$process_directory
  // create lnm$job lnm$group lnm$process and insert them
  // create sys$input etc
  // create job and group tables
  // create space for process-private logicals cache
  // copy from pqb
  // copy more from pqb
  // copy uaf
  // copy jib
  // dealloc pqb
  // mmg$imgreset
  // $imgfix
  // merge f11xqp into p1
  // change access mode
  // request to image activator
  // exe$rmsexh
  // dummy cli
  // change access mode again
  // clear FP
  // initial call frame
  // exe$catch_all
  // $imgfix
  // arg list
  // check hib stsflg
  // call inital image transfer address
}
