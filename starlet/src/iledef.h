#ifndef iledef_h
#define iledef_h
 
/*
** 32 bit item list entry 3 structure
*/
	
struct _ile3
    {
    unsigned short int ile3$w_length;		/* Length of buffer in bytes */
    unsigned short int ile3$w_code;		/* Item code */
    void *ile3$ps_bufaddr;			/* 32 bit buffer address */
    unsigned short int *ile3$ps_retlen_addr;	/* 32 bit address of a word for the returned length */
    };
#define ILE3$K_LENGTH sizeof(struct _ile3)
#define ILE3$C_LENGTH sizeof(struct _ile3)

/*
** 64-Bit Item List Entry B structure.  This is the 64 bit item list 3 structure.
*/
	
struct _ileb_64
    {
    unsigned short int ileb_64$w_mbo;		/* Must be One */
    unsigned short int ileb_64$w_code;		/* Item code */
    int ileb_64$l_mbmo;				/* Must be Minus One */
    unsigned long long ileb_64$q_length;	/* Length of buffer in bytes */
    void *ileb_64$pq_bufaddr;			/* 64 bit Buffer pointer */
    unsigned long long *ileb_64$pq_retlen_addr;	/* 64 bit address of quadword for returned length */
    };
#define ILEB_64$K_LENGTH sizeof(struct _ileb_64)
#define ILEB_64$C_LENGTH sizeof(struct _ileb_64)

/*
** 32-bit Item List Entry 2 structure
*/
	
struct _ile2
    {
    unsigned short int ile2$w_length;		/* Length of buffer in bytes */
    unsigned short int ile2$w_code;		/* Item code value */
    void *ile2$ps_bufaddr;			/* 32 bit buffer address */
    };
#define ILE2$K_LENGTH sizeof(struct _ile2)
#define ILE2$C_LENGTH sizeof(struct _ile2)

/*
** 64-Bit Item List Entry 2 structure.
*/
	
struct _ilea_64
    {
    unsigned short int ilea_64$w_mbo;		/* Must be One */
    unsigned short int ilea_64$w_code;		/* Item code */
    int ilea_64$l_mbmo;				/* Must be Minus One */
    unsigned long long ilea_64$q_length;	/* Length of buffer in bytes */
    void *ilea_64$pq_bufaddr;			/* 64 bit Buffer address */
    };
#define ILEA_64$K_LENGTH sizeof(struct _ilea_64)
#define ILEA_64$C_LENGTH sizeof(struct _ilea_64)

#define $is_itmlst64 _is_itmlst64
inline int _is_itmlst64(const void *item_list)
    {
    return ((const struct _ilea_64 *)item_list)->ilea_64$w_mbo == 1 &&
	   ((const struct _ilea_64 *)item_list)->ilea_64$l_mbmo == -1;
    }
#endif
