/*
 *	strposition
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

 /* str.c
 *
 *	Code for VAX STR$COMPARE routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *	Jan 30, 1997 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in severa; places.
 *
 *	Feb 26, 2004 - Andrew Allison
 *		Added GNU License
 *
 *	Mar 20, 2004 - Andrew Allison
 *		Inserted test if start position < 0 make it 0
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$position
 *
 *	Searches for the first position of a substring within
 *	a source string. If STR$POSITION finds the substring,
 *	it returns the relative position of the substring.
 *
 *	returns:
 *		0 if not found, otherwise returns the
 *		relative position.
 */
unsigned long str$position(
			const struct dsc$descriptor_s* first_source_string,
			const struct dsc$descriptor_s *second_source_string,
			const long *start_position)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	long loop;			/* Outer loopp */
	long start_pos;

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

//	Check that start position make sense
	start_pos = *start_position;
	if ( start_pos < 0 )
		start_pos = 0;
	if ( start_pos > s1_length )
		start_pos = s1_length;

//	Special case if substring is NULL return source + 1
	if ( s2_length == 0 )
	{
		return s1_length + 1;
	}
	/*
	 * We loop from the start position to the end of the
	 * last possible match position in the string.
	 */
	for (loop = start_pos - 1;
		loop <= s1_length - s2_length; loop++)
	{
		/*
		 * Inner loop does the actual comparison
		 */
		if (memcmp(s1_ptr + loop, s2_ptr, s2_length) == 0)
		{
			return loop + 1;
		}
	}

	/*
	 * Return the answer
	 */
	return 0;
}

