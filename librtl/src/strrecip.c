/*
 *	strrecip.c
 *
 *	Copyright (C) 2003 Andrew Allison
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *The author may be contacted at freevms@sympatico.ca
 *
 *	Andrew Allison
 *	50 Denlaw Road
 *	London, Ont
 *	Canada 
 *	N6G 3L4
 *
 */

/*
 *
 * History
 *
 *	Feb 20, 2004 - Andrew Allison
 *              Wrote str$recip code
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <values.h>
#include "descrip.h"
#include <stdlib.h>
#include "strdef.h"
#include "libdef.h"
#include "str$routines.h"
#include "stdint.h"

/*************************************************************
 * str$recip 
 *
 *	calculate the reciprocal of a number to a specified precsion
 *
 *      Fixed length output string results are blank padded or truncated
 *      Varying length output length is set or truncated
 *      Dynamic length output length is set
 *
 *       Format
 *       sign,exp, digits, sign,exp,digits, sign,exp,digits 
 *       
 *       1,   23, +12345,  0,   -34,5432112, out,out,out
 * 
 * 	Input
 *		digits	       65,536 	string portion of number
 *		exp	2,147,483,648	power of 10 to obtain value of number
 *		sign			sign of number 0 pos 1 neg
 *		Total   2,147,549,184   
 *
 *		value = sign digits * 10 ^^ exp
 *	Returns	
 * 		STR$_NORMAL
 *		STR_TRU		Truncation
 *	Signal
 *		LIB$_INVARG	Invalid Argument
 *		LIB$_DIVBY_ZER	Divison by zero
 *		STR$_FATINTERR  Internal Error
 * 		STR$_ILLSTRCLA	Illegal string Class
 *		STR$_INSVIRMEM	Insufficient virtual memory
 *		STR$_WRONUMARG	Wrong number of arguments	
 */
#define TRUE		1
#define FALSE		0


unsigned long str$recip (	const unsigned	long *asign, 
                      		const         	long *aexp, 
	              		const struct 	dsc$descriptor_s *adigits,
                      		const unsigned	long *bsign,
                      		const		long *bexp, 
                      		const struct 	dsc$descriptor_s *bdigits,
	              		      unsigned	long *csign,
                                	      	long *cexp,
                            	      struct	dsc$descriptor_s *cdigits)
{
int		c_not_zero;
int		i, outlen;
char		*s1_ptr, *s2_ptr, *s3_ptr;
signed   long	oneexp, rti, td;
unsigned long  	onesign, status;
unsigned short	s1_len,  s2_len,  s3_len;
struct dsc$descriptor_s one;

        status = STR$_NORMAL;
	outlen = 0;
	rti = 0;	// round truncate indicator  1 = round 0 = truncate

//	Check the sign field is 1 or 0 
      	if ( *asign == 1 || *asign == 0 ) 
		;
	else
            status = LIB$_INVARG;

        if ( *bsign == 1  || *bsign == 0)
		;
	else
            status = LIB$_INVARG;

//	Copy the sign of the input number
	*csign = *asign;

//	Get the length of the input strings and how much room for the output
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);
        str$analyze_sdesc (cdigits, &s3_len, &s3_ptr);
	strcpy (s3_ptr,"0");

	td = atol (s2_ptr);

//	Check that we are not dividing by zero
	c_not_zero = FALSE;
	for (i=0; i < s1_len; i++)
		if ( s1_ptr[i] != '0')
			c_not_zero = TRUE;

	if ( c_not_zero == FALSE )
	{	status = STR$_DIVBY_ZER;
	}

// 	Quick abort
	if (status != STR$_NORMAL)
	{	return status;
	}

//	Check that the precision is not zero. 
	c_not_zero = FALSE;
	for (i=0; i < s2_len; i++)
		if ( s2_ptr[i] != '0')
			c_not_zero = TRUE;

	if ( c_not_zero == FALSE )
	{	status = STR$_NORMAL;
		return status;
	}

//	Create the numeric 1 numerator
	one.dsc$w_length  = 1;
	one.dsc$b_class   = DSC$K_CLASS_D;
	one.dsc$b_dtype   = DSC$K_DTYPE_T;
	one.dsc$a_pointer = calloc (1,1);
	*one.dsc$a_pointer = '1';

//	set the sign and exponent for the one
	oneexp = 0;
	onesign = 0;

//divide asign,aexp,adigit,bsign,bexp,bdigit,td,rti,csign,cexp,cdigit

	str$divide (&onesign,&oneexp,&one, asign,aexp,adigits, &td,&rti, csign, cexp, cdigits);



	return status;
}

