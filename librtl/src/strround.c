/*
 *	strround.c
 *
 *	Copyright (C) 2004 Andrew Allison
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
 *
 *	Feb 23, 2004 - Andrew Allison
 *              Wrote str$round code
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
 * str$round 
 *
 *	Either round or truncate a string to a desired number of digits
 *
 *
 *       Format
 *       places, flags, insign,inexp, indigits, outsign,outexp,outdigits 
 *       
 * 	Input
 *
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

unsigned long str$round (	const 		long *tdigits,
		      		const unsigned	long *rti,
				const unsigned	long *asign, 
                      		const         	long *aexp, 
	              		const struct 	dsc$descriptor_s *adigits,
                      		      unsigned	long *csign,
                      				long *cexp, 
                      		      struct 	dsc$descriptor_s *cdigits)

{
char	*s1_ptr, *one_ptr, *temp_ptr, rounding_char;
unsigned short	s1_len, one_len, new_length, temp_len;
unsigned long status;
struct dsc$descriptor_s one, temp_sd;
	
	status = STR$_NORMAL;
	new_length = *tdigits;

	if ( ( *asign != 0 ) && ( *asign != 1) )
		return LIB$_INVARG;

	if ( ( *csign != 0 ) && ( *csign != 1) )
		return LIB$_INVARG;

	if ( ( *rti != 0 ) && ( *rti != 1) )
		return LIB$_INVARG;

//	Create a descriptor with the string value of 1 so we can round up
	str$$malloc_sd (&one,"1"); 
	str$analyze_sdesc (&one, &one_len, &one_ptr);

// 	Get the length of the input string
	str$analyze_sdesc (adigits, &s1_len, &s1_ptr);
//	Is there even enough digits to do a rounding or truncation
	if ( s1_len <= *tdigits )
	{
		str$copy_dx (cdigits, adigits);	// Nope just copy to output
	}
	else
	{
		if ( ( *rti == 0 ) && ( s1_ptr[*tdigits] >= 0 ) )
		{
			str$analyze_sdesc (adigits,&temp_len,&temp_ptr);

// 			correct the multiplier we are changing the string length
			if ( (*cexp) <= 0 )
			{	(*cexp) += temp_len - *tdigits;
			}
			else 
			{	(*cexp) -= temp_len - *tdigits;
			}
			
			rounding_char = temp_ptr[*tdigits];
			str$get1_dx ( &s1_len, cdigits );	//make same size
			str$copy_dx ( cdigits, adigits);	// copy
			str$get1_dx ( &new_length, cdigits );	// resize
			str$$malloc_sd (&temp_sd, s1_ptr);
			str$copy_dx (&temp_sd, cdigits);
			str$analyze_sdesc (&temp_sd,&temp_len,&temp_ptr);

			if ( rounding_char >= '5' )
			{	str$add	(asign, aexp, &temp_sd,	// round up
					 asign, aexp, &one,
					 csign, cexp, cdigits );
			}
		}
		else		// Just truncate
		{	
			str$copy_dx ( cdigits,  adigits);
			str$get1_dx ( &new_length, cdigits );
			if ( (*cexp) <= 0 )
				(*cexp) += s1_len - *tdigits;
			else 
				(*cexp) -= s1_len - *tdigits;
		
		}
	}

	return status;
}

/*************************************************************/
