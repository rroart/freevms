#ifndef orbdef_h
#define orbdef_h

/* Author: Roar Thronæs */

/* This is supposed to have lot more. Got this because of syslnm */

struct orb {
unsigned short orb$w_uicmember;
unsigned short orb$w_uicgroup;
unsigned long orb$l_acl_mutex;
unsigned short orb$w_size;
unsigned char orb$b_type;
unsigned char orb$b_flags;
unsigned short reserved;
unsigned short orb$w_refcount;
unsigned long orb$l_mode_protl;
unsigned long orb$l_mode_proth;
unsigned long orb$l_sys_prot;
unsigned long orb$l_own_prot;
unsigned long orb$l_grp_prot;
unsigned long orb$l_wor_prot;
unsigned long orb$l_aclfl;
unsigned long orb$l_aclbl;
unsigned char orb$min_clas[20];
unsigned char orb$max_clas[20];
};

#endif

