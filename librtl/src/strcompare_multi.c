/*
 * strcompare_multi.c
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
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 19, 1997 - Kevin Handy
 *		Fix so that a shorter string is assumed to be padded
 *		with spaces.
 *
 *	Feb 24, 2004 - Andrew Allison
 *		Inserted GNU License
 */

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"
#include "libdef.h"
#include <string.h>

/*************************************************************
 * str$compare_multi
 *
 *	Compares two strings for the same contents. This
 *	routine distinguishes between upper and lower case
 *	strings.
 *
 *	returns:
 *		-1 if first < second
 *		0 if equal
 *		1 if first > second
 */
long str$compare_multi( const struct dsc$descriptor_s* first_source_string,
			const struct dsc$descriptor_s *second_source_string,
			const unsigned long *flags,
			const unsigned long *language)
{
unsigned long 	case_sensitive,  character_set;
char 		*s1_ptr, *s2_ptr;
unsigned short 	s1_length, s2_length;
unsigned short 	min_length;
long 		result;	


	case_sensitive = 1; 		// default case sensitive comparision
	if ( (unsigned long) flags != 0 )
	{	switch ( *flags)
		{
		case 0:	case_sensitive = 1;	// comparision is case sensitive
			break;
		case 1: case_sensitive = 0;	// ignore case
			break;
		default:
			result = LIB$_INVARG;
			return result;
		}
	}

	character_set = 1;
	if ( (unsigned long) language != 0 )
	{	switch ( *language )
		{
		case 1:	character_set = 1;	// multinational
			break;
		case 2:	character_set = 2;	// Danish
			break;
		case 3:	character_set = 3;	// Finish Swedish
			break;
		case 4:	character_set = 4;	// German
			break;
		case 5:	character_set = 5; 	// Norwegian
			break;
		case 6:	character_set = 6;	// Spanish
			break;
		default:
			result = LIB$_INVARG;
			return result;
		}
	}
	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * Calculate length to compare
	 */
	min_length = (s1_length < s2_length) ? s1_length : s2_length;

	/*
	 * Compare the two strings.
	 * Use 'memcmp' instead of 'strncmp' because we may have NULL's
	 * in our strings.
	 */
	if ( min_length != 0 ) 
		result = memcmp(s1_ptr, s2_ptr, min_length);

	/*
	 * Work on the result in case of equal in first part, but
	 * different total lengths.
	 */
	if (result == 0)
	{
		if (s1_length < s2_length)
		{
			while ((min_length < s2_length) && (result == 0))
			{
				if (s2_ptr[min_length++] != ' ')
				{
					result = -1;
				}
			}
		}
		else
		{
			if (s1_length > s2_length)
			{
				while ((min_length < s1_length) &&(result == 0))
				{
					if (s1_ptr[min_length++] != ' ')
					result = 1;
				}
			}
		}
	}

	/*
	 * Normalize the result
	 */
	if (result < -1)
	{
		result = -1;
	}

	if (result > 1)
	{
		result = 1;
	}

	/*
	 * Return the answer
	 */
	return result;
}

