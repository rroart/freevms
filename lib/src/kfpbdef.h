#ifndef kfpbdef_h
#define kfpbdef_h

#define KFPB$K_LENGTH 16
#define KFPB$C_LENGTH 16
#define KFPB$S_KFPBDEF 16

struct _kfpb
{
    struct _kfd *kfpb$l_kfdlst;
    void *kfpb$l_kfehshtab;
    unsigned short int kfpb$w_size;
    unsigned char kfpb$b_type;
    unsigned char kfpb$b_spare;
    unsigned short int kfpb$w_kfdlstcnt;
    unsigned short int kfpb$w_hshtablen;
};

#endif

