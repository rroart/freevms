/*
 *	strmatch_wild
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

 /* strmatch_wild
 *
 *	Code for VAX STR$MATCH_WILD routine
 *
 * Description:
 *	Compares a pattern string that includes wildcard charaters
 *	with a candidate string.
 *
 *	Translates wildcard characters and searches the candidate string to
 *	determine if it matches the pattern string. The pattern string may
 *	contain either one or both of the wildcard characters asterisk (*) and
 *	percent (%)
 *
 * Bugs:
 *
 * History
 *
 *	Created Andrew Allison Feb 18,2004
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$match_wild
 *
 */

unsigned long str$match_wild ( 	const struct dsc$descriptor_s *candidate_string,
				const struct dsc$descriptor_s *pattern_string)
{
	int		s1_pos, s2_pos;
	char 		*s1_ptr, *s2_ptr, *s1_copy, *s2_copy;
	unsigned short 	s1_len, s2_len;	
	unsigned long 	result;

	result = 0;

	str$analyze_sdesc ( candidate_string, &s1_len, &s1_ptr);
	str$analyze_sdesc ( pattern_string,   &s2_len, &s2_ptr);
	
	s1_copy = calloc (s1_len,1);
	s2_copy = calloc (s2_len,1);

	strcpy (s1_copy,s1_ptr);
	strcpy (s2_copy,s2_ptr);

	s1_pos = 0;
	s2_pos = 0;
	result = STR$_MATCH;

	for ( s1_pos = 0; s1_pos < s1_len; s1_pos++  )
	{
		if ( s1_copy[s1_pos] == s2_copy[s2_pos] )
		{
			result = STR$_MATCH;
			s2_pos++;
		}
		else if ( s2_copy[s2_pos] == '%' )	
		{
			result = STR$_MATCH;
			s2_pos++;
		}
		else if ( s2_copy[s2_pos] == '*' )
		{
			if (s2_pos+1 >= s2_len )	// wild card last char 
			{
				s1_pos = s1_len;
				s2_pos++;
				result = STR$_MATCH;
			}
			else	// wild card not last character
			{	if ( s1_copy[s1_pos] == s2_copy[s2_pos+1] )
				{
					s2_pos += 2;
				} 
			}
		}
		else
		{
			result = STR$_NOMATCH;
			s1_pos = s1_len;
		}

	} // end for


// Special case last char is * which can represent nothing
	if (( s2_copy[s2_pos] == '*' ) && ( s2_pos+1 == s2_len ))
		s2_pos++;

// we left uncheck characters in the candidate string
	if ( s2_pos != s2_len )
	{
		result = STR$_NOMATCH;
	}

	free (s1_copy);
	free (s2_copy);
	
	return result;
}

