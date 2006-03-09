#ifndef MMG_FUNCTIONS_H
#define MMG_FUNCTIONS_H

//#include <builtins.h>
#include <far_pointers.h>
#if 0
#include <gsddef.h>
#include <ints.h>
#include <ipldef.h>
#include <jibdef.h>
#include <lib_bigpage.h>
#include <pcbdef.h>
#include <pfndef.h>
#include <pfn_macros.h>
#include <phddef.h>
#include <psldef.h>
#include <ptedef.h>
#include <pte_functions.h>
#include <rdedef.h>
#include <secdef.h>
#include <shm_descdef.h>
#include <shm_iddef.h>
#include <ssdef.h>
#include <starlet_bigpage.h>
#include <sys_functions.h>
#include <vadef.h>
#include <vms_macros.h>
#endif

#if 0
typedef unsigned long long uint64;
#endif
#define uint64 unsigned long long

inline struct _rde * mmg$lookup_rde_id (uint64 region_id, struct _phd * const phd, int ipl);

inline struct _rde * mmg$search_rde_va (void * va, struct _rde *head, struct _rde **prev, struct _rde **next);

inline struct _rde * mmg$lookup_rde_va (void * va, struct _phd * const phd, int function, int ipl);

#undef uint64

#endif
