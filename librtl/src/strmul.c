
/*
 *	strmul.c
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
 *	Should we abort if input string c is not null?
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
 *	Oct 1, 2003 - Andrew Allison
 *              Wrote str$mul code
 *
 *	Feb 19, 2004 - Andrew Allison
 * 		Changed malloc to calloc to initialize memory
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
 * str$mul 
 *
 *	Multiply two decimal strings of digits
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


/***********************************************/

unsigned long str$mul (const unsigned long *asign, 
                      const          long *aexp, 
	              const struct dsc$descriptor_s *adigits,
                      const unsigned long *bsign,
                      const          long *bexp, 
                      const struct dsc$descriptor_s *bdigits,
	              unsigned       long *csign,
                                     long *cexp,
                            struct dsc$descriptor_s *cdigits)
{
unsigned short	s1_len,  s2_len,  s3_len, temp_len;
char		*s1_ptr, *s2_ptr, *s3_ptr;
unsigned long  	index, max_len, min_len;
int		i,j,k;
unsigned long	status;
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

	if (( *asign == 0 ) && ( *bsign == 0 )) *csign = 0;
	if (( *asign == 0 ) && ( *bsign == 1 )) *csign = 1;
	if (( *asign == 1 ) && ( *bsign == 0 )) *csign = 1;
	if (( *asign == 1 ) && ( *bsign == 1 )) *csign = 0;

//	Get the length of the input strings and how much room for the output
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
        str$analyze_sdesc (bdigits, &s2_len, &s2_ptr);
        str$analyze_sdesc (cdigits, &s3_len, &s3_ptr);
	strcpy (s3_ptr,"0");

// 	Quick abort
	if (status != STR$_NORMAL)
	{	return status;
	}

//	zero out the accumulator
	for (i=0; i < MAXSTR; i++ )
	{	a[i] = '0';
		b[i] = '0';
		c[i] = 0;
	}

//	Move in the largest number - we need to keep the alignment correct
//	char string is "right to left" alignment
//	start at location specified by the exponent
	max_len = ( s1_len > s2_len ) ? s1_len : s2_len;
	min_len = ( s1_len > s2_len) ? s2_len : s1_len;

//	Copy input strings to working storage
	for (i = 0; i < s1_len; i++ )
	{	a[i] = s1_ptr[i];
	}
	for (i = 0; i < s2_len; i++ )
	{	b[i] = s2_ptr[i];
	}

//	Set the output exponent
	*cexp = *aexp + *bexp;

	max_len = s1_len + s2_len;
	sum = 0;
	carry = 0;
	k = max_len;
	for (j = s2_len; j > 0; j--)
	{ 
		k = max_len - s2_len + j;
		for ( i = s1_len; i > 0; i-- )
		{
			sum = ( b[j-1] - '0' ) * ( a[i-1] - '0');
			sum += carry;
			carry = 0;
			c[k]  += sum % 10;
			if (c[k] > 9 )
			{
				c[k]   -= 10;
				c[k-1] += 1;
			}
			sum   -= sum % 10;
			carry  = sum / 10;
			sum = 0;
			k--;
		}
	}
	c[k] = carry;

//	Truncate output sum string to 65536 MAXUINT16
	if ( max_len > MAXUINT16 )
	{	status = STR$_TRU;
		max_len = MAXUINT16;
	}

//	Free any memory that is passed into us.
	str$free1_dx (cdigits);
	temp_len = (unsigned short) max_len + 1;
	str$get1_dx(&temp_len, cdigits);
	str$analyze_sdesc (cdigits,&s3_len, &s3_ptr);

	for (i = 0; i <= max_len; i++)
	{	
		s3_ptr[i] = (c[i] + '0');
	}

	free (a);
	free (b);
	free (c);

	str$$lzerotrim (cdigits);
	str$$rzerotrim (cdigits,cexp);
	str$$iszerotrim(cdigits,cexp);

	return status;
}


/*************************************************************/
