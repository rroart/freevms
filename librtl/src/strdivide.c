/*
 *	strdivide.c
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
 *
 *	Oct 3, 2003 - Andrew Allison
 *              Wrote str$divide code
 *
 *	Feb 19, 2004 - Andrew Allison
 *		Changed malloc to calloc to initalize memory
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
 * str$divide A / B = C
 *
 *	Divide two decimal strings of digits
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
 *		STR$_FATINTERR  Internal Error
 * 		STR$_ILLSTRCLA	Illegal string Class
 *		STR$_INSVIRMEM	Insufficient virtual memory
 *		STR$_WRONUMARG	Wrong number of arguments	
 */
#define MAXSTR 		132000
#define MAXUINT16	65536
#define TRUE		1
#define FALSE		0


unsigned long str$divide (	const unsigned	long *asign, 
                      		const         	long *aexp, 
	              		const struct 	dsc$descriptor_s *adigits,
                      		const unsigned	long *bsign,
                      		const		long *bexp, 
                      		const struct 	dsc$descriptor_s *bdigits,
		      		const 		long *tdigits,
		      		const unsigned	long *rti,
	              		      unsigned	long *csign,
                                	      	long *cexp,
                            	      struct	dsc$descriptor_s *cdigits)
{
int		c_not_zero;
int		i,j,k, leading_zero_count;
unsigned int	temp_len, outlen;
char		*s1_ptr, *s2_ptr, *s3_ptr;
char		*qa_ptr, *qb_ptr, *result_ptr;
signed  long	dexp, eexp, lpos, precision, temp_exp;
//signed	long	outlenlong;
unsigned long  	dsign,esign, max_len, min_len,status, temp_rti;
unsigned short	s1_len,  s2_len,  s3_len, result_len;
unsigned short  uspos, qa_len, qb_len, result_temp_len;
struct dsc$descriptor_s quotient,divisor,result,temp,quotient_a,quotient_b;
struct dsc$descriptor_s zero, nine, result_temp;

        status = STR$_NORMAL;
	outlen = 0;
	precision = *tdigits;

//	Check the sign field is 1 or 0 
      	if ( *asign == 1 || *asign == 0 ) 
		;
	else
            status = LIB$_INVARG;

        if ( *bsign == 1  || *bsign == 0)
		;
	else
            status = LIB$_INVARG;

	if (( *asign == 0 ) && ( *bsign == 0 )) *csign = 0;
	if (( *asign == 0 ) && ( *bsign == 1 )) *csign = 1;
	if (( *asign == 1 ) && ( *bsign == 0 )) *csign = 1;
	if (( *asign == 1 ) && ( *bsign == 1 )) *csign = 0;

//	Get the length of the input strings and how much room for the output
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);
        str$analyze_sdesc (cdigits, &s3_len, &s3_ptr);
	strcpy (s3_ptr,"0");

//	Check that we are not dividing by zero
	c_not_zero = FALSE;
	for (i=0; i < s2_len; i++)
		if ( s2_ptr[i] != '0')
			c_not_zero = TRUE;

	if ( c_not_zero == FALSE )
	{	status = STR$_DIVBY_ZER;
	}

// 	Quick abort
	if (status != STR$_NORMAL)
	{	return status;
	}

//	Check that we don't have a zero numerator 
	c_not_zero = FALSE;
	for (i=0; i < s1_len; i++)
		if ( s1_ptr[i] != '0')
			c_not_zero = TRUE;

	if ( c_not_zero == FALSE )
	{	status = STR$_NORMAL;
		return status;
	}

//	Move in the largest number - we need to keep the alignment correct
//	char string is "right to left" alignment
//	start at location specified by the exponent
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len;
	min_len = ( s1_len > s2_len ) ? s2_len : s1_len;


//	Set the output exponent
	*cexp = *aexp - *bexp;
	temp_exp = 0;

	temp_len = s1_len;
	quotient.dsc$w_length  = s1_len;
	quotient.dsc$b_class   = DSC$K_CLASS_D;
	quotient.dsc$b_dtype   = DSC$K_DTYPE_T;
	quotient.dsc$a_pointer = calloc (1,temp_len);

	quotient_a.dsc$w_length  = 0;
	quotient_a.dsc$b_class   = DSC$K_CLASS_D;
	quotient_a.dsc$b_dtype   = DSC$K_DTYPE_T;
	quotient_a.dsc$a_pointer = NULL;

	quotient_b.dsc$w_length  = 0;
	quotient_b.dsc$b_class   = DSC$K_CLASS_D;
	quotient_b.dsc$b_dtype   = DSC$K_DTYPE_T;
	quotient_b.dsc$a_pointer = NULL;

	temp_len = s2_len;
	divisor.dsc$w_length  = s2_len;
	divisor.dsc$b_class   = DSC$K_CLASS_D;
	divisor.dsc$b_dtype   = DSC$K_DTYPE_T;
	divisor.dsc$a_pointer = calloc (1,temp_len);

	temp_len = precision;
	result.dsc$w_length  = 0;
	result.dsc$b_dtype   = DSC$K_DTYPE_T;
	result.dsc$b_class   = DSC$K_CLASS_D;
	result.dsc$a_pointer = NULL;

	temp.dsc$w_length  = 0;
	temp.dsc$b_class   = DSC$K_CLASS_D;
	temp.dsc$b_dtype   = DSC$K_DTYPE_T;
	temp.dsc$a_pointer = NULL;

	result_temp.dsc$w_length  = 0;
	result_temp.dsc$b_class   = DSC$K_CLASS_D;
	result_temp.dsc$b_dtype   = DSC$K_DTYPE_T;
	result_temp.dsc$a_pointer = NULL;

	zero.dsc$w_length  = 1;
	zero.dsc$b_class   = DSC$K_CLASS_D;
	zero.dsc$b_dtype   = DSC$K_DTYPE_T;
	zero.dsc$a_pointer = calloc (1,1);
	*zero.dsc$a_pointer = '0';

	nine.dsc$w_length  = 1;
	nine.dsc$b_class   = DSC$K_CLASS_D;
	nine.dsc$b_dtype   = DSC$K_DTYPE_T;
	nine.dsc$a_pointer = calloc (1,1);
	*nine.dsc$a_pointer = '9';

	dsign 	= 0;
	dexp	= 0;
	esign  	= 0;
	eexp 	= 0;

//	Copy input strings to working storage
	str$copy_dx(&quotient,  adigits);
	str$copy_dx(&quotient_b,adigits);
	str$copy_dx(&divisor,   bdigits);

//	Flag set to round result
	if ( *rti == 1 )
		precision++;

//	Get ready for the first digit
	result_temp_len = 1;
	str$get1_dx (&result_temp_len, &result);

//	Start Dividing by repeated subtraction
//	Need 1 digit more of precision so we know which way to round
	for ( k = 0; k <= precision; k++ )
	{
//		Increase the size of the result descriptor by 1
		result_temp_len = k + 1;
		str$get1_dx	(&result_temp_len, &result);

		outlen++;
	        str$analyze_sdesc (&quotient_b, &qb_len, &qb_ptr);
		if ( qb_len > 0 )
		{
//			Split quotient into 2 parts a left b right
//			move over 1 digit from quotient_b to quotient_a
			uspos = 1;
			lpos  = 1;
			str$free1_dx(&temp);
			str$get1_dx (&uspos,&temp);
			str$left    (&temp,&quotient_b,&lpos);
			str$append  (&quotient_a,&temp);
	
//			Remove that digit from quotient b	
		        str$analyze_sdesc (&quotient_b, &qb_len, &qb_ptr);
			str$get1_dx (&qb_len,&temp);	// create temp var
			str$copy_dx (&temp,&quotient_b); //copy b to temp
			str$free1_dx(&quotient_b);	// free b
			qb_len--;			// decrement size of b
			str$get1_dx (&qb_len,&quotient_b);	// create b
			lpos++;
			str$right(&quotient_b,&temp,&lpos);	// right temp
			lpos--;
			str$free1_dx(&temp);
		}
		else 
		{
//			Add a borrow zero
			str$analyze_sdesc (&quotient_a, &qa_len, &qa_ptr);
			str$get1_dx (&qa_len,&temp);
			str$copy_dx (&temp,&quotient_a);
			qa_len++;
			str$free1_dx(&quotient_a);
			str$get1_dx (&qa_len,&quotient_a);
			str$copy_dx (&quotient_a,&temp);
			str$append  (&quotient_a,&zero);
			(*cexp)--;
//			if ( *csign == 0 )
//			{
//				if (*cexp < 0)
//					(*cexp)++;
//				else
//					*csign = 1;
//			}
//			if ( *csign == 1 )
//			{	(*cexp)--;
//			}
		}

// 		Let's start at 0
		str$analyze_sdesc (&quotient_a, &qa_len, &qa_ptr);
		str$get1_dx (&qa_len,&temp);
		str$analyze_sdesc (&result, &result_len, &result_ptr);
		result_ptr[k] = '0';

//printf ("quotient a "); str$$print_sd (&quotient_a); printf ("\n");
//printf ("divisor    "); str$$print_sd (&divisor); printf ("\n");

//		while quotient_a > divisor  i.e. can still subtract
		while ( str$$ncompare(&quotient_a,&divisor) >= 0 )
		{

//printf ("while quotient a "); str$$print_sd (&quotient_a); printf ("\n");
//printf ("while divisor    "); str$$print_sd (&divisor); printf ("\n");
//pause();

			status = str$sub(&dsign,&dexp ,&quotient_a,
//					&*bsign,&*bexp,&divisor,
					&*bsign,&temp_exp,&divisor,
					&esign ,&eexp ,&temp);

//			Increment  result / loop counter
			str$analyze_sdesc (&result, &result_len, &result_ptr);
			result_ptr[k] += 1;

//			Move remainder of subtraction to quotient_a
			str$copy_dx(&quotient_a,&temp);

//			Add zero's to normalize result exponent
			if ( eexp > 0 )
			{
				for (j = 0; j < eexp; j++)
				{
				str$analyze_sdesc(&quotient_a,&qa_len,&qa_ptr);
					str$get1_dx (&qa_len,&temp);
					str$copy_dx (&temp,&quotient_a);
					qa_len++;
					str$free1_dx(&quotient_a);
					str$get1_dx (&qa_len,&quotient_a);
					str$copy_dx (&quotient_a,&temp);
					str$append  (&quotient_a,&zero);
				}
			}
		}
		str$free1_dx(&temp);

//		Are we done and out of digits
		if ((str$$ncompare (&quotient_a,&zero) == 0 ) &&
		    (quotient_b.dsc$w_length == 0 ))
		{
			k = precision + 1;
		}

		if ( result.dsc$a_pointer[k] == '0' )
		{	leading_zero_count++;
			precision++;
		}
//printf ("%c",result.dsc$a_pointer[k]  );
	}	// end for

//	Free any memory that is passed into us.
	str$free1_dx (cdigits);

//	Truncate output sum string to 65536 MAXUINT16
	if ( max_len > MAXUINT16 )
	{	status = STR$_TRU;
		max_len = MAXUINT16;
	}

//	Allocate space for result
	result_len = outlen;

	str$get1_dx (&result_len,cdigits);

//	trim leading zero's
	str$$lzerotrim (&result);

//	outlenlong = outlen;
//	str$left (cdigits,&result,&outlenlong);

//	Who designed this - 2 routines flags are opposite
//	str$divide 1 == round, 	  0 == truncate 
//	str$round  1 ==truncate,  0 == round
	if ( (*rti) == 0 )
		temp_rti = 1;
	else
		temp_rti = 0;

	str$round (tdigits,&temp_rti,csign,cexp,&result,csign,cexp,cdigits);

	str$free1_dx (&quotient);
	str$free1_dx (&quotient_a);
	str$free1_dx (&quotient_b);
	str$free1_dx (&result);
	str$free1_dx (&divisor);
	str$free1_dx (&temp);
	str$free1_dx (&nine);
	str$free1_dx (&zero);

	return status;
}

/*************************************************************/
