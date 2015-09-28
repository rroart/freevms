#ifndef MMG_FUNCTIONS_H
#define MMG_FUNCTIONS_H

#include <far_pointers.h>
#include <phddef.h>
#include <rdedef.h>
#include <vadef.h>

inline struct _rde * mmg$lookup_rde_id(UINT64 region_id, struct _phd * const phd, int ipl);

inline struct _rde * mmg$search_rde_va(void * va, struct _rde *head, struct _rde **prev, struct _rde **next);

inline struct _rde * mmg$lookup_rde_va(void * va, struct _phd * const phd, int function, int ipl);

#endif
