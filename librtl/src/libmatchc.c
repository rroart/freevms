/*
 *	lib$matchc
 *
 *	Copyright (C) 2005 Andrew Allison
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
 *
 *	Code for VAX LIB$MATCHC routine
 *
 * Description:
 *	Search a source string for a specified substring and returns an index
 *	which is the relative position of the first occurrence of a substring
 *	in the source string.
 *
 *	Returns 1,...,n for found string
 *		0 if not found
 *	If the substring has zero length return 1
 *	If the source string has zero length and the substring has a non-zero
 *	length - Zero is returned
 *
 * Bugs:
 *
 * History
 *
 *	Mar 25, 2005 - Andrew Allison
 *		Initial program creation - mainly copied from str$position
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "str$routines.h"
#include "lib$routines.h"

unsigned long lib$matchc(const struct dsc$descriptor_s* search_string,
			 const struct dsc$descriptor_s *source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	long loop;			/* Outer loop */

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(search_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(source_string, &s2_length, &s2_ptr);

//	Specical case search string is 0 and substring is non-zero
	if (( s2_length == 0 ) && ( s1_length != 0 ))
		return 0;
//	Special case if substring is NULL return 1
	if ( s1_length == 0 )
		return 1;

	/*
	 * We loop from the start position to the end of the
	 * last possible match position in the string.
	 */
	for (loop = 0; loop <= s2_length - s1_length; loop++)
	{
		/*
		 * Inner loop does the actual comparison
		 */
		if (memcmp(s1_ptr, s2_ptr + loop, s1_length) == 0)
		{
			return loop + 1;
		}
	}

	/*
	 * Return the answer
	 */
	return 0;
}

