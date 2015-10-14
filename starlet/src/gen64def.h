/*
 * gen64def.h
 *
 *  Created on: Oct 4, 2015
 *      Author: guido
 */

#ifndef GEN64DEF_H_
#define GEN64DEF_H_

#include <vms_types.h>

typedef struct _generic_64
{
    union
    {
        UINT64 gen64$q_quadword;
        UINT32 gen64$l_longword[2];
        UINT16 gen64$w_word[4];
        UINT8 gen64$b_byte[8];
    } gen64$r_quad_overlay;
} GENERIC_64;

#endif /* GEN64DEF_H_ */
