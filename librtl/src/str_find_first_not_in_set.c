/*
 *	strfind_first_not_in_set
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

/* str_find_first_not_in_set.c
 *
 *      Code for VAX STR$FIND_FIRST_NOT_IN_SET routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *      Jan 30, 1997 - Kevin Handy
 *              Preliminary design.
 *
 *	Feb 7, 1997 - Christof Ziele
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 26, 2004 - Andrew Allison
 *		Added GNU License
 *
 *	Mar 12, 2004 - Andrew Allison
 *		Return 1 if set-of-characters is empty
 */ 

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$find_first_not_in_set
 *
 *	This function searches a string, one character at a time, from
 *	the left to right, comparing each character in the string to
 *	every character in a specified set of characters for which it
 *	is searching, and return when no match is found.
 *
 *	Returns:
 *		0 if no match found, otherwise returns the position
 *		in the source string where the first match is found.
 *
 */
long str$find_first_not_in_set(
        const struct dsc$descriptor_s* first_source_string,
        const struct dsc$descriptor_s* second_source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	int loop1;			/* Outer loop */
	int loop2;			/* Inner loop */
	int result;			/* Partial result */

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * Quick exit when goofy stuff passed
	 */
	if (s1_length == 0 )
	{
		return 1;
	}
	if (s2_length == 0)
	{
		return 0;
	}

	/*
	 * I'm going to do it using a double loop. If the strings
	 * passed are long, then it might be better to create an
	 * aray[256] of matching characters and we could then loose
	 * the inner loop.
	 *
	 * The tradeoff comes in initializing the test table vs
	 * using an inner loop.
	 */

	/*
	 * Outer loop
	 */
	for (loop1 = 0; loop1 < s1_length; loop1++)
	{
		result = 0;

		/*
		 * Inner loop
		 */
		for (loop2 = 0; loop2 < s2_length; loop2++)
		{
			/*
			 * Compare the characters
			 */
			if (s1_ptr[loop1] == s2_ptr[loop2])
			{
				result = 1;
			}
		}

		/*
		 * Was no match found?
		 */
		if (result == 0)
		{
			return loop1 + 1;
		}
	}

	/*
	 * If we get here, we don't have a match
	 */
	return 0;
}


