#ifndef orbdef_h
#define orbdef_h

/* Author: Roar Thronæs */

/* This is supposed to have lot more. Got this because of syslnm */

struct orb {
unsigned short ORB$W_UICMEMBER;
unsigned short ORB$W_UICGROUP;
unsigned long ORB$L_ACL_MUTEX;
unsigned short ORB$W_SIZE;
unsigned char ORB$B_TYPE;
unsigned char ORB$B_FLAGS;
unsigned short reserved;
unsigned short ORB$W_REFCOUNT;
unsigned long ORB$L_MODE_PROTL;
unsigned long ORB$L_MODE_PROTH;
unsigned long ORB$L_SYS_PROT;
unsigned long ORB$L_OWN_PROT;
unsigned long ORB$L_GRP_PROT;
unsigned long ORB$L_WOR_PROT;
unsigned long ORB$L_ACLFL;
unsigned long ORB$L_ACLBL;
unsigned char ORB$MIN_CLAS[20];
unsigned char ORB$MAX_CLAS[20];
};

#endif orb_h
