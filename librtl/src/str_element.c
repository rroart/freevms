/*
 *	strelement.c
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
 *The authors may be contacted at:
 *
 *	Andrew Allison		freevms@sympatico.ca
 *
 *				Andrew Allison
 *				50 Denlaw Road
 *				London, Ont
 *				Canada 
 *				N6G 3L4
 *
 */

/*
 *	Code for VAX STR$ELEMENT routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *
 * History
 *
 *	Feb 3, 1997 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 26, 2004 - Andrew Allison
 * 		Added GNU License
 *
 *	Mar 12, 2004 - Andrew Allison
 *		If text not found return NULL in destination
 *		return entire substring if token not found
 *		if no text present between delimiters return NULL
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "ssdef.h"
#include "str$routines.h"

/*************************************************************
 * str$element
 *
 */
unsigned long str$element(struct dsc$descriptor_s* destination_string,
	const long* find_element_number,
	const struct dsc$descriptor_s* delimiter_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;			/*Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	char* s3_ptr	;		/* Pointer to third string */
	unsigned short s3_length;	/* Length of third string */
	short res_ptr;			/* Index into source*/
	unsigned short res_length;	/* Index to end  of element */
	char delimiter;			/* Delimiter character */
	unsigned long result1;
	unsigned long result = STR$_NORMAL;	/* Result */
	int element_number;

	element_number = (int) *find_element_number;

//	Did we get a numeric value
	if ( isdigit((char) element_number + '0') == 0 )
		return STR$_ILLSTRSPE;

	/*
	 * Negitive numbers start at first element
	 */
	if (element_number < 0)
	{
		element_number = 0;
	}


        /*
         * Analyze strings
         */
        str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	str$analyze_sdesc(delimiter_string, &s3_length, &s3_ptr);

//	If no delimiter's where found return entire string
	if ( (s2_ptr != NULL) && (s3_ptr != NULL) )
	{
		if ( strstr (s2_ptr, s3_ptr) == NULL) 
		{
//			str$copy_dx (destination_string,source_string);
			return SS$_NORMAL;
		}
	}

	/*
	 * Check out delimiter. Assume '\0' if none given.
	 */
	if (s3_length != 1)
	{
		return STR$_INVDELIM;
	}
	delimiter = *s3_ptr;
	/*
	 * Look for a sterting position
	 */
	res_ptr = 0;
	while ((element_number > 0) && (res_ptr < s2_length))
	{
		if (s2_ptr[res_ptr] == delimiter)
		{
			element_number--;
		}
		res_ptr++;
	}

	/*
	 * Should now point to result string, or not have a
	 * any element available
	 */
	res_length = 0;
	if (element_number != 0)
	{
		result = STR$_NOELEM;
	}
	else
	{
		/*
		 * Look for ending delimiter
		 */
		while (((res_ptr + res_length) < s2_length) &&
			(s2_ptr[res_ptr + res_length] != delimiter))
		{
			res_length++;
		}
	}

	/*
	 * Copy over the result
	 */
	if ( res_ptr < s2_length )
	{	s1_ptr = s2_ptr + res_ptr;
		result1 = str$copy_r(destination_string, &res_length, s1_ptr);
	}
/*
	else
	{	str$free1_dx (destination_string);
		result1 = STR$_NOELEM;
		result  = STR$_NOELEM;
	}
*/
	if (result1 == STR$_NORMAL)
	{
		result = SS$_NORMAL;
	}

	/*
	 * Done
	 */
	return result;
}

