#ifndef __CVTDEF_H
#define __CVTDEF_H

/* From David Wijnants */

  /*
   * Float types.
   */

  #define CVT$K_VAX_F      0
  #define CVT$K_VAX_D      1
  #define CVT$K_VAX_G      2
  #define CVT$K_VAX_H      3
  #define CVT$K_IEEE_S     4
  #define CVT$K_IEEE_T     5
  #define CVT$K_IBM_LONG   6
  #define CVT$K_IBM_SHORT  7
  #define CVT$K_CRAY       8
  #define CVT$K_IEEE_X     9

  /*
   * Conversion options.
   */

  #define CVT$M_ROUND_TO_NEAREST  0x00000001
  #define CVT$M_ROUND_TO_ZERO     0x00000002
  #define CVT$M_ROUND_TO_POS      0x00000004
  #define CVT$M_ROUND_TO_NEG      0x00000008
  #define CVT$M_VAX_ROUNDING      0x00000010
  #define CVT$M_BIG_ENDIAN        0x00000020
  #define CVT$M_ERR_UNDERFLOW     0x00000040
  #define CVT$M_TRUNCATE          CVT$M_ROUND_TO_ZERO

  /*
   * Status codes.
   * See OpenVMS Programming Concepts Manual fig. 13.3
   */

  #define CVT$_NORMAL     0x05fa8009
  #define CVT$_INPCONERR  0x05fa8014
  #define CVT$_INVINPTYP  0x05fa801c
  #define CVT$_INVOPT     0x05fa8024
  #define CVT$_INVOUTTYP  0x05fa802c
  #define CVT$_INVVAL     0X05fa8034
  #define CVT$_NEGINF     0x05fa803c
  #define CVT$_OUTCONERR  0x05fa8044
  #define CVT$_OVERFLOW   0x05fa804c
  #define CVT$_POSINF     0x05fa8054
  #define CVT$_UNDERFLOW  0x05fa805c

#endif
