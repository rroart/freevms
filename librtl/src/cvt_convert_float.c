/* From David Wijnants */

/*
 * This is an implementation of the OpenVMS cvt$convert_float routine
 * which converts single and double precision IEEE floats to and from
 * VAX f and d floats.
 *
 * It does not handle Cray and IBM types, and does not implement all
 * the rounding options in full. If you throw it lots of duff PC data,
 * you'll also find it doesn't do denormalised floats, but then it does
 * work reasonably well on normal everyday IEEE and VAX floats, which 
 * is what I'm really after.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cvtdef.h>
#include <cvt$routines.h>

/*
 * The VAX f and d-floats, and the IEEE s-float, have an eight-bit
 * exponent. Converting between these types just means rearranging the
 * bits. The following bitfield structures remove the need for masking
 * and shifting.
 */

typedef struct
{
  unsigned f1   :  7;
  unsigned exp  :  8;
  unsigned sign :  1;
  unsigned f2   : 16;
} _VAXF;

typedef struct
{
  unsigned f1   :  7;
  unsigned exp  :  8;
  unsigned sign :  1;
  unsigned f2   : 16;
  unsigned f3   : 16;
  unsigned f4   : 16;
} _VAXD;

typedef struct
{
  unsigned f2   : 16;
  unsigned f1   :  7;
  unsigned exp  :  8;
  unsigned sign :  1;
} _IEES;

/*
 * The IEEE t-float has an eleven-bit exponent. Converting to or from
 * this type requires rounding or truncation in relation to the extra
 * three bits of precision. The _FRACT bitfield union makes it easier 
 * to add and remove three extra bits.
 */

typedef struct
{
  unsigned f4   : 16;
  unsigned f3   : 16;
  unsigned f2   : 16;
  unsigned f1   :  4;
  unsigned exp  : 11;
  unsigned sign :  1;
} _IEET;

typedef union
{
  struct
  {
    unsigned five  :  9;
    unsigned four  : 16;
    unsigned three : 16;
    unsigned two   : 16;
    unsigned one   :  7;
  } exp8;

  struct
  {
    unsigned five  : 12;
    unsigned four  : 16;
    unsigned three : 16;
    unsigned two   : 16;
    unsigned one   :  4;
  } exp11;

} _FRAC;

/*
 * CVT$CONVERT_FLOAT is a general-purpose OpenVMS floating-point 
 * conversion routine that converts VAX, IEEE, Cray and IBM 
 * floating-point types to and from each other.
 *
 * ~iThis~i implementation provides only the 'useful' subset that
 * allows IEEE and VAX types to be converted on platforms other
 * than OpenVMS.
 */

int cvt$convert_float ( void * ival,
                        int    itype,
                        void * oval,
                        int    otype,
                        int    opt )
{
  int             inf=0,          // input float is infinity
                  zero=0,         // set output float to zero
                  over=0,         // overflow detected
                  nan=0,          // input float is not a number
                  roprand=0,      // set output to 'reserved operand'
                  denorm=0,       // input float is denormalised
                  exp;            // (signed) zero'ed exponent

  unsigned int   *l1,             // lowest longword of float
                 *l2;             // next longword

  unsigned short  sign;           // 0=positive 1=negative

  _FRAC           f;              // fraction bits

  /*
   * I don't quite understand the ins and outs of the options
   * parameter, but it's probably mostly to do with what happens to
   * denormalised IEEEs - which aren't supported anyway - so I'll
   * just ignore it for now and truncate extraneous bits. Just for 
   * consistency, I ~iwill~i check for an invalid parameter though.
   */

  if ( opt & ~( CVT$M_ROUND_TO_NEAREST |
                CVT$M_ROUND_TO_ZERO |
                CVT$M_ROUND_TO_POS |
                CVT$M_ROUND_TO_NEG |
                CVT$M_VAX_ROUNDING |
                CVT$M_BIG_ENDIAN |
                CVT$M_ERR_UNDERFLOW ) ) return (CVT$_INVOPT);
  opt = CVT$M_ROUND_TO_ZERO;

  /*
   * Extract the bits from the input float, de-bias the exponent, and
   * recognise known bit patterns for zero, NaN and infinity.
   */

  l2 = (l1=ival) + 1;
  switch ( itype )
  {
    case CVT$K_VAX_F:
      {
      _VAXF      *p = ival;
      sign          = p->sign;
      exp           = p->exp;
      f.exp8.one    = p->f1;
      f.exp8.two    = p->f2;
      f.exp8.three  = 0;
      f.exp8.four   = 0;
      f.exp8.five   = 0;
      nan           = ( *l1==0x00008000 );
      zero          = ( exp == 0 || exp >= 254 ) && !nan;
      exp          -= 128;
      }
      break;
    case CVT$K_VAX_D:
      {
      _VAXD      *p = ival;
      sign          = p->sign;
      exp           = p->exp;
      f.exp8.one    = p->f1;
      f.exp8.two    = p->f2;
      f.exp8.three  = p->f3;
      f.exp8.four   = p->f4;
      f.exp8.five   = 0;
      nan           = ( *l1==0x00008000 && *l2==0x00000000 );
      zero          = ( exp == 0 || exp >= 254 ) && !nan;
      exp          -= 128;
      }
      break;
    case CVT$K_IEEE_S:
      {
      _IEES      *p = ival;
      sign          = p->sign;
      exp           = p->exp;
      f.exp8.one    = p->f1;
      f.exp8.two    = p->f2;
      f.exp8.three  = 0;
      f.exp8.four   = 0;
      f.exp8.five   = 0;
      zero          = ( *l1==0x00000000 || *l1==0x80000000 );
      inf           = ( *l1==0x7f800000 || *l1==0xff800000 );
      denorm        = ( !exp && !zero );
      over          = ( exp == 0xfe );
      nan           = ( exp == 0xff );
      roprand       = ( over || nan );
      exp          -= 126;
      }
      break;
    case CVT$K_IEEE_T:
      {
      _IEET      *p = ival;
      sign          = p->sign;
      exp           = p->exp;
      f.exp11.one   = p->f1;
      f.exp11.two   = p->f2;
      f.exp11.three = p->f3;
      f.exp11.four  = p->f4;
      f.exp11.five  = 0;
      zero          = ( *l2==0x00000000 || *l2==0x80000000 ) && !*l1;
      inf           = ( *l2==0x7ff00000 || *l2==0xfff00000 ) && !*l1;
      denorm        = ( !exp && !zero );
      over          = ( exp == 0x7fe );
      nan           = ( exp == 0x7ff );
      roprand       = ( over || nan );
      exp          -= 1022;
      }
      break;
    default:
      return (CVT$_INVINPTYP);
  }

  /*
   * Poke the bits into the output float after re-biasing the exponent,
   * removal or addition of extra bits for t-floats is taken care of by
   * the _FRACT bit-field union. Zero and NaN are handled separately.
   */

  l2 = (l1=oval) + 1;
  switch ( otype )
  {
    case CVT$K_VAX_F:
           if ( zero )    *l1 = 0x00000000;
      else if ( roprand ) *l1 = 0x00008000;
      else
      {
        _VAXF     *p = oval;
        f.exp8.three = 0;
        p->sign      = sign;
        p->exp       = exp + 128;
        p->f1        = f.exp8.one;
        p->f2        = f.exp8.two;
      }
      break;
    case CVT$K_VAX_D:
           if ( zero )    *l2 = 0x00000000, *l1 = 0x00000000;
      else if ( roprand ) *l2 = 0x00000000, *l1 = 0x00008000;
      else
      {
        _VAXD    *p = oval;
        f.exp8.five = 0;
        p->sign     = sign;
        p->exp      = exp + 128;
        p->f1       = f.exp8.one;
        p->f2       = f.exp8.two;
        p->f3       = f.exp8.three;
        p->f4       = f.exp8.four;
      }
      break;
    case CVT$K_IEEE_S:
           if ( nan )  *l1 = 0x7fbfffff;
      else if ( zero ) *l1 = 0x00000000;
      else
      {
        _IEES     *p = oval;
        f.exp8.three = 0;
        p->sign      = sign;
        p->exp       = exp + 126;
        p->f1        = f.exp8.one;
        p->f2        = f.exp8.two;
      }
      break;
    case CVT$K_IEEE_T:
           if ( nan )  *l2 = 0x7ff7ffff, *l1 = 0xffffffff;
      else if ( zero ) *l2 = 0x00000000, *l1 = 0x00000000;
      else
      {
        _IEET     *p = oval;
        f.exp11.five = 0;
        p->sign      = sign;
        p->exp       = exp + 1022;
        p->f1        = f.exp11.one;
        p->f2        = f.exp11.two;
        p->f3        = f.exp11.three;
        p->f4        = f.exp11.four;
      }
      break;
    default:
      return (CVT$_INVOUTTYP);
  }

  /*
   * Return status codes for negative infinity, positive infinity,
   * invalid number or NaN, and overflow detected. In the absence of
   * any strange conditions, indicate normal successful completion.
   */

  if ( inf && sign )  return (CVT$_NEGINF);
  if ( inf && !sign ) return (CVT$_POSINF);
  if ( nan )          return (CVT$_INVVAL);
  if ( over )         return (CVT$_OVERFLOW);
                      return (CVT$_NORMAL);
}
