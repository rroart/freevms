
/*
 *	strsub.c
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
 * str.c
 *
 *	Code for VAX STR$ routines
 *
 * Description:
 *
 *	This file contains various 'str$' functions equivalent
 *	to those available in Vax/VMS string library.
 *
 * Bugs:
 *
 *	Not compatible at the binary level.
 *
 *	No seperate "string zone" to allocate memory from,
 *	uses malloc/free instead.
 *
 *	Some versions of realloc are broken! Some don't like to be
 *	called hundreds of times.  The program may
 *	crash with a segmentation fault in such a case.
 *
 *	I probibly should be using dsc$descriptor instead of
 *	dsc$descriptor_s, but I prefer to have the type 'char*'
 *	instead of 'void*' which is the only difference.
 *
 *	Not worrying about the dsc$b_dtype field yet. Assumes it
 *	will always be type DSC$K_DTYPE_T (character coded text).
 *	Type V should mean size is in bits, and P means size is in
 *	digits (4 bit nibbles).
 *
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design. Spelling errors are
 *		not my fault! Someone must have snuck them in
 *		there when I wasn't looking.
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Added a 'return STR$_ILLSTRCLA' so that compiling
 *		with '-Wall' won't display errors.
 *
 *      Sep 10, 2003 - Andrew Allison
 *              Wrote str$sub code
 *
 *	Feb 19, 2004 - Andrew Allison
 *		Changed malloc to calloc to initialize memory
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <values.h>
#include "descrip.h"
#include <stdlib.h>
#include "strdef.h"
#include "libdef.h"
#include "str$routines.h"
#include "lib$routines.h"
//#include "stdint.h"

/*************************************************************
 * str$sub
 *
 *	Subtract two decimal strings of digits
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
 *	Bugs
 *		You could create a much more elegant solution seeing if 
 *		the numbers actually overlap befor going down the brute
 *		force road
 */
#define MAXSTR 132000
#define MAXUINT16	65536
#define	POS 0
#define	NEG 1
#define REV 0
#define NORM 1
#define FALSE 0
#define TRUE 1
#define MALLOC_CHECK_ 0


unsigned long str$sub (const unsigned long *asign, 
                      const          long *aexp, 
	              const struct dsc$descriptor_s *adigits,
                      const unsigned long *bsign,
                      const          long *bexp, 
                      const struct dsc$descriptor_s *bdigits,
	              unsigned       long *csign,
                                     long *cexp,
                            struct dsc$descriptor_s *cdigits)
{
unsigned short	s1_len,  s2_len,  s3_len, c_len;
char		*s1_ptr, *s2_ptr, *s3_ptr;
unsigned long  	index, max_len, min_len;
int		i, j, k, l;
unsigned long	status;
signed long	min_exp, max_exp, a_size, b_size, max_size, min_size;
char 		ctemp;
int		sum, borrow, order, result;
int		a_not_zero,b_not_zero,c_not_zero;
unsigned long 	plus_sign;
char            *a, *b, *c;

        status = STR$_NORMAL;
	index = 0;
	a_not_zero = FALSE;
	b_not_zero = FALSE;
	c_not_zero = FALSE;

	a = (char *) calloc(MAXSTR,1);
	b = (char *) calloc(MAXSTR,1);
	c = (char *) calloc(MAXSTR,1);

	if ( a == NULL )
	{	status = STR$_INSVIRMEM;
	}
	if ( b == NULL )
	{	status = STR$_INSVIRMEM;
	}
	if ( c == NULL )
	{	status = STR$_INSVIRMEM;
	}

//	Check the sign field is 1 or 0 
      	if ( *asign == 1 || *asign == 0 ) 
		;
	else
            status = LIB$_INVARG;

        if ( *bsign == 1  || *bsign == 0)
		;
	else
            status = LIB$_INVARG;

//	We need to find which is larger a or b to set up formula
//	Get the length of the input strings and how much room for the output
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);
        str$analyze_sdesc (cdigits, &s3_len, &s3_ptr);
//	strcpy (s3_ptr,"0");
	str$free1_dx (cdigits);

// 	Quick abort
	if (status != STR$_NORMAL)
	{	free(a);
		free(b);
		free(c);
		return status;
	}

	max_exp = ( *aexp  >  *bexp ) ? *aexp : *bexp;
	min_exp = ( *aexp  >  *bexp ) ? *bexp : *aexp;
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len;
	min_len = ( s1_len > s2_len ) ? s2_len : s1_len;
	a_size  = ( *aexp + s1_len );
	b_size  = ( *bexp + s2_len );
	max_size= ( a_size > b_size ) ? a_size : b_size;
	min_size= ( a_size > b_size ) ? b_size : a_size;

	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);

//	Copy input strings to working storage
	for (i = 0; i < s1_len; i++ )
	{	a[i] = s1_ptr[i];
		if (a[i] != '0')
		{	a_not_zero = TRUE;
		}
	}
	for (i = 0; i < s2_len; i++ )
	{	b[i] = s2_ptr[i];
		if (b[i] != '0')
		{	b_not_zero = TRUE;
		}
	}
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len;

//	Set the output exponent
	*cexp = min_exp;

//	Add zero's to the end of the number for remaining exponent
	for ( i = *aexp; i > (int) (min_exp); i--)
	{	a[s1_len + *aexp - 1 ] = '0';
	}
	for ( i = *bexp; i > (int) (min_exp); i--)
	{	b[s2_len + *bexp - 1] = '0';
	}
	
// 	New adjusted string length with zero's
	i = (int) s1_len + (int) *aexp - (int) min_exp;
	j = (int) s2_len + (int) *bexp - (int) min_exp;

	order = NORM;
	result = lib$$ncompare(a,b);
	if ( result <= -1 ) result = -1;
	if ( result >=  1 ) result = 1;


	switch  (result )
	{
// 	absolute (a) less than absolute (b)
	case -1:
//		(-1) - (-2) = 1 = +(b-a)
		if ((*asign == NEG) && (*bsign == NEG))
		{	*csign = POS;
			order  = REV;
		}
//	 	1 - 2 = (-1) = -(b-a)
		if ((*asign == POS) && (*bsign == POS))
		{	*csign = NEG;
			order  = REV;	
		}
//	 	1  - (-2) =  1 = +(b-a)
		if ((*asign == POS) && (*bsign == NEG))
		{	*csign = POS;
			order  = REV;
		}
//		(-1) - 2 = -3 =	-(a+b)
		if ((*asign == NEG) && (*bsign == POS))
		{	plus_sign = 0;
			status = str$add (&plus_sign,aexp,adigits,
				 bsign,bexp,bdigits,
				 csign,cexp,cdigits);
			*csign = NEG;
			free(a);
			free(b);
			free(c);
			return status;
		}
		break;
// 	absolute (a) equals absolute (b)
	case  0:
//		2 - 2 = 0	zero
		if ((*asign == POS) && (*bsign == POS))
		{	*csign = POS;
			order  = NORM;
		}
//		2 - (-2) = 4	+(a+b)
		if ((*asign == POS) && (*bsign == NEG))
		{
			plus_sign = 0;
			status = str$add (asign,aexp,adigits,
				 &plus_sign,bexp,bdigits,
				 csign,cexp,cdigits);
			*csign = POS;
			free(a);
			free(b);
			free(c);
			return status;
		}
//		-2 - 2 = -4	-(a+b)
		if ((*asign == NEG) && (*bsign == POS))
		{	plus_sign = 0;
			status = str$add (&plus_sign,aexp,adigits,
				 bsign,bexp,bdigits,
				 csign,cexp,cdigits);
			*csign = NEG;
			free(a);
			free(b);
			free(c);
			return status;
		}
//		-2 - -2 = 0	zero
	
	if ((*asign == NEG) && (*bsign == NEG))
		{	*csign = POS;
			order  = NORM;
		}
		break;
//	absolute (a) greater than absolute (b)
	case  1:
//		-2 - -1	= -1	-(a-b)
		if ((*asign == NEG) && (*bsign == NEG))
		{	*csign = NEG;
			order = NORM;
		}
//		2 - 1 = 1	+(a-b)
		if ((*asign == POS) && (*bsign == POS))
		{	*csign = POS;
			order = NORM;
		}
//		-2 - 1	-3	-(a+b)
		if ((*asign == NEG) && (*bsign == POS))
		{	plus_sign = 0;
			status = str$add(&plus_sign,aexp,adigits,
				 bsign,bexp,bdigits,
				 csign,cexp,cdigits);
			*csign = NEG;
			free(a);
			free(b);
			free(c);
			return status;
		}
//		2 - -1 = 3	+(a+b)
		if ((*asign == POS) && (*bsign == NEG))
		{	plus_sign = 0;
			status = str$add (asign,aexp,adigits,
				 &plus_sign,bexp,bdigits,
				 csign,cexp,cdigits);
			*csign = POS;
			free(a);
			free(b);
			free(c);
			return status;
		}
		break;
	default:
		free(a);
		free(b);
		free(c);
 		return STR$_FATINTERR;
	}
	
	sum    =  0;
	borrow =  0;
	ctemp  = '0';

// 	New max string length
	max_len = ( i > j ) ? i : j ;
//	Start Subtracting
	for (k =(int) max_len; k > 0; k-- )
	{
	    if ( order == NORM )
	    {
		if ( i > 0 )
		{
			if ( a[i-1] > '0' )
				sum  = a[i-1] - '0';
			else
				sum = 0;
		}
		if ( j > 0 )
		{	
			if ( b[j-1] > a[i-1] )
			{
//	Borrowing
				for ( l = i; l > 1; l-- )
					if (( a[l-2] ) > '0' )
					{	(a[l - 2])--;
						sum += 10;
						l = 0;
					}
					else
					{	a[l-2] = '9';
					}
				
				sum -= (b[j-1] - '0');
			}
			else
				sum -= b[j-1] - '0';
			
		}
		ctemp = sum + '0';
		sum = 0;
		c[k-1] = ctemp;
		i--;
		j--;
	    }
	    else
	    { 	
		if ( j > 0 )
		{
			if ( b[j-1] > '0' )
				sum  = b[j-1] - '0';
			else
				sum = 0;
		}
		if ( i > 0 )
		{	if ( a[i-1] > b[j-1] )
			{
				sum += 10;
				(b[j-2])--;
				sum -= (a[i-1] - '0');
			}
			else
			{	sum -= a[i-1] - '0';
			}
		}
		ctemp = sum + '0';
		sum = 0;
		c[k-1] = ctemp;
		i--;
		j--;
	    }
	}

//	Truncate output sum string to 65536 MAXUINT16
	if ( max_len > MAXUINT16 )
	{	status = STR$_TRU;
		max_len = MAXUINT16;
	}

	c_len = max_len;
	str$get1_dx (&c_len,&*cdigits);
	str$copy_r (&*cdigits,&c_len,c);
	str$$lzerotrim  (&*cdigits);
	str$$rzerotrim  (&*cdigits,&*cexp);
	str$$iszerotrim (&*cdigits,&*cexp);
	free (a);
	free (b);
	free (c);
	return status;
}


/*************************************************************/
