#ifndef diobmdef_h
#define diobmdef_h

#include <ptedef.h>

#define DIOBM$M_REL_DEALLOC 0x1
#define DIOBM$M_PTE_WINDOW 0x2
#define DIOBM$M_AUX_INUSE 0x4
#define DIOBM$M_INUSE 0x8
#define DIOBM$M_S0PTE_WINDOW 0x10
#define DIOBM$K_HDRLEN 16
#define DIOBM$K_PTECNT_FIX 9

#define DIOBM$K_PTECNT_MAX_UNI 94
#define DIOBM$K_PTECNT_MAX_SMP 430

#define DIOBM$K_LENGTH 88
#define DIOBM$M_NORESWAIT 1
 
struct _diobm {
  struct _diobm *diobm$ps_aux_diobm;
  unsigned int diobm$l_pte_count;
  unsigned short int diobm$w_size;
  unsigned char diobm$b_type;
  unsigned char diobm$b_subtype;
  union  {
    unsigned int diobm$l_flags;
    struct  {
      unsigned diobm$v_rel_dealloc : 1;
      unsigned diobm$v_pte_window : 1;
      unsigned diobm$v_aux_inuse : 1;
      unsigned diobm$v_inuse : 1;
      unsigned diobm$v_s0pte_window : 1;
      unsigned diobm$v_fill_2_ : 3;
    };
  };
  union  {
    struct _pte diobm$q_pte_vector [9];
    struct  {
      struct _pte *diobm$pq_ptew_va_spte;
      void *diobm$ps_ptew_s0va;
    };
  };
};
 
#endif
 
