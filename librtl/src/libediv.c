/*
 * libediv.c
 *
 *	Code for LIB$EDIV routine
 *
 * Description:
 *	Perform extened precision divison
 *
 * Bugs:
 *
 *
 * History
 *
 *	Apr 25, 2005 - Andrew Allison
 *		Intial program creation	
 */

#include "ssdef.h"
#include "lib$routines.h"

/*************************************************************
 * lib$ediv
 *
 * Extened precision divide
 * lib$ediv long-int-divisor, quad-int-dividend,long-int-quotient,remainder
 *
 *	Returns SS$_NORMAL
 * 		SS$_INTDIV	integer divide by zero. The quotient is replaced
 *				by bits 31:0 of the dividend and the remainder
 *				is replaced by zero
 *		SS$_INTOVF	Integer overflow. The quotient is replaced by
 *				bits 31:0 of the dividend and the remainder is
 *				replaced by zero
 *
 */
unsigned long lib$ediv(	const	signed long int *divisor,
			const	signed long long *dividend,
				signed long int *quotient,
				signed long int *remainder )

{

//	Divide by 0 - a no-no
	if ( *divisor == 0 )
	{	*quotient = (long) *dividend;
		*remainder = 0;
		return SS$_INTDIV;
	}

//	Do the math
	*quotient  = *dividend / *divisor;
	*remainder = *dividend % *divisor;

//	Detect overflow
	if ((*dividend > 0 ) && (*quotient < 0 ))
	{	*quotient = 0x00000000FFFFFFFF & *dividend;
		*remainder = 0;
		return SS$_INTOVF;
	}
	if ((*dividend < 0 ) && (*quotient > 0 ))
	{	*quotient = 0x00000000FFFFFFFF & *dividend;
		*remainder = 0;
		return SS$_INTOVF;
	}
	
	return SS$_NORMAL;
}

