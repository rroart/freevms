
/*
 *	stradd.c
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
 *              Wrote str$add code
 *
 *	Feb 19  2004 - Andrew Allison
 * 		Changed malloc to calloc to initialize memory
 *
 *	Mar 28, 2004 - Andrew Allison
 *		Compatability testing
 *		Added code to cause "stack dump" if c descriptor
 *		is not NULL
 */


/*************************************************************
 * str$add 
 *
 *	Add two decimal strings of digits
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
#define MAXSTR 		132000
#define MAXUINT16	65536


/*****************************/

unsigned long str$add ( const	unsigned	long *asign,
			const 			long *aexp,
			const struct dsc$descriptor_s *adigits,
			const	unsigned	long *bsign,
			const 			long *bexp,
			const struct dsc$descriptor_s *bdigits,
				unsigned  	long *csign,
				 		long *cexp,
			struct dsc$descriptor_s *cdigits)
{
unsigned short	s1_len,  s2_len,  s3_len, temp_len;
char		*s1_ptr, *s2_ptr, *s3_ptr;
unsigned long  	index,max_len,min_len;
int		i,j,k;
unsigned long	status;
signed long	min_exp,max_exp, a_size, b_size, max_size, min_size;
char 		ctemp;
int		sum,carry;
char		*a,*b,*c;

        status = STR$_NORMAL;
	index = 0;

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

//	If we have a negative sign then call str$subtract
//	c = -a + b
	if (( *asign == 1 ) && (*bsign == 0 ))
	{	
	     status = str$sub(asign,aexp,adigits,bsign,bexp,bdigits,csign,cexp,cdigits);	
  	     return status;
	}
//	c = a - b
	if (( *asign == 0  ) && (*bsign == 1 ))
	{	
	     status = str$sub(asign,aexp,adigits,bsign,bexp,bdigits,csign,cexp,cdigits);	
	     return status;
	}
//	c = -a + -b
	*csign = 0;
	if (( *asign == 1 ) && ( *bsign == 1))
	{	*csign = 1;
	}

//	Get the length of the input strings and how much room for the output
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);
        str$analyze_sdesc (cdigits, &s3_len, &s3_ptr);
	if ( s3_ptr != NULL )
	{	str$free1_dx (cdigits);
		printf ("Destination must be NULL\n");
		return STR$_FATINTERR;
	}
// 	Quick abort
	if (status != STR$_NORMAL)
	{	return status;
	}

//	Move in the largest number - we need to keep the alignment correct
//	char string is "right to left" alignment
//	start at location specified by the exponent
	max_exp = ( *aexp > *bexp ) ? *aexp : *bexp;	// get largest exp
	min_exp = ( *aexp > *bexp ) ? *bexp : *aexp;
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len;
	min_len = ( s1_len > s2_len) ? s2_len : s1_len;
	a_size  = ( *aexp + s1_len );
	b_size  = ( *bexp + s2_len );
	max_size= ( a_size > b_size ) ? a_size : b_size;
	min_size= ( a_size > b_size ) ? b_size : a_size;

// 	The strings don't overlap just return the largest
	if ( max_size - min_size > UINT16_MAX )
	{	

//Don't Overlap returning largest 
		if ( *aexp > *bexp )
		{	*cexp = *aexp;
			str$copy_dx (cdigits,adigits);
		}
		else
		{	*cexp = *bexp;
			str$copy_dx(cdigits,bdigits);
		}
	return STR$_TRU;
	}

//	Copy input strings to working storage
	for (i = 0; i < s1_len; i++ )
	{	a[i] = s1_ptr[i];
	}
	for (j = 0; j < s2_len; j++ )
	{	b[j] = s2_ptr[j];
	}

//	Set the output exponent
	*cexp = min_exp;

//	Add zero's to the end of the number for remaining exponent
	if ( *aexp > *bexp )
	{
		for ( i = s1_len; i < s1_len + max_exp - min_exp; i++)
			a[i] = '0';
		s1_len += max_exp - min_exp;
	}
	if ( *aexp < *bexp )
	{
		for ( i = s2_len; i < s2_len + max_exp - min_exp; i++)
			b[i] = '0';
		s2_len += max_exp - min_exp;
	}

	sum = 0;
	carry = 0;
	ctemp = '0';
	i = s1_len;
	j = s2_len;
// 	New max string length
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len ;

	for (k =(int) max_len; k > 0; k-- )
	{
		if ( i > 0 )
		{	sum += a[i-1] - '0';
		}
		if ( j > 0 )
		{	sum += b[j-1] - '0';
		}
		sum += carry;
		carry = 0;
		if ( sum > 9 )
		{	carry = 1;
			sum -= 10;
		}
		ctemp = sum + '0';
		sum = 0;
		c[k-1] = ctemp;
		i--;
		j--;
	}
	if ( carry == 1 )
	{
		for (i = max_len-1; i >= 0; i-- )
		{	c[i+1] = c[i];
		}
		c[0] = (char) (carry + '0');
		max_len++;
	}


//	Truncate output sum string to 65536 MAXUINT16
	if ( max_len > MAXUINT16 )
	{	status = STR$_TRU;
		max_len = MAXUINT16;
	}

//	Free any memory that is passed into us.
	temp_len = max_len;
	str$free1_dx(cdigits);

	str$get1_dx(&temp_len,cdigits);
	str$analyze_sdesc (cdigits,&s3_len,&s3_ptr);

	for (i = 0; i < max_len; i++)
	{	
		s3_ptr[i] = c[i];
	}

	free (a);
	free (b);
	free (c);
	str$$lzerotrim (&*cdigits);
	str$$rzerotrim (&*cdigits,&*cexp);
	str$$iszerotrim (&*cdigits,&*cexp);

	return status;
}


/*************************************************************/
