
/*
 *	str$find_first_substring	
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

/*	Description
 *
 *	Str$find_first_substring takes as input a string to be searched
 *	and an unspecified number of substrings ffor which tosearch. It
 *	searches the specified string and returnbs the position of the
 *	substring that is found earliest in the string.This is not necessarilly
 *	the position of the first substring specified. That is,
 *	str$find_first_substring returns the position of the leftmost matching
 *	substring. The order in which the substrings are searched for is
 *	irrelevant.
 *
 *	Unlike many of the compare and search routines, 
 *	str$find_first_substring does not return the position in a returned
 *	value. The position of the substring which is found earlies in the
 *	string is retuurned in the the index argument. If none of the specified
 *	substring is found in the string, the value of index is 0.
 *
 *	Zero lenght string, of null arguments produced unexpected rresults.
 *	Any time the routine is called with a null substring as an argument.
 *	str$find_first_substring always returns the position of the null
 *	substring as the first substring found. All other substring are
 *	interprtoted as appearing in the string after the null string.
 *
*/
/* 	str$find_first_substring source-string,index,substring-index,substring
 *	[,substring]
 *
 * 
 * source-string
 *	source-string	char_string
 * 	type		character string
 * 	access		read only
 * 	mechanism	by descriptor
 * String that STR$FIND_FIRST_SUBSTRING searches. The source-string
 * argument is the address of a descriptor pointing to the string
 *
 * index
 * Earliest position within source-string at which STR$FIND_FIRST_SUBSTRING
 * found a matching substring; zero if no matching substring was found. The
 * index argument is the address of a signed longword containing this position
 * 
 * substring-index
 * Ordinal number of the substring that matched (1 for the first,2 for the
 * second and so on) or zero if str$find_first_substring found no substrings
 * that matched.The substring-index argument is the address of a signed longword
 * containing this ordinal number
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "descrip.h"
#include "str$routines.h"
/************************************/

#undef str$find_first_substring
unsigned long str$find_first_substring (const struct dsc$descriptor_s *s1,
				long *index, long *subindex,
				struct dsc$descriptor_s *sub, ...)

{
int	i, status, result;
long	j;
char	*s1_ptr,*s2_ptr;
struct dsc$descriptor_s *sd_ptr, temp_sd, temp2_sd;
unsigned short	s1_len, s2_len,temp_len;
va_list ap;

	*index = 0;
	sd_ptr = 0;
	*subindex = 0;

	str$analyze_sdesc (s1,&s1_len,&s1_ptr);
	str$analyze_sdesc (sub,&s2_len,&s2_ptr);
	va_start(ap,sub);		// make ap point to first unnamed arg
	sd_ptr = sub;
	do
	{	
		++*subindex;

		str$analyze_sdesc (sd_ptr,&s2_len,&s2_ptr);
		if ( (s1_len >= s2_len ) && (s2_len != 0 ))
		{
			for (i = 1; i < (s1_len - s2_len + 2); i++ )
			{
				j = i;
				temp_len = s2_len;
				temp_sd.dsc$w_length  = 0;
				temp_sd.dsc$b_class   = DSC$K_CLASS_D;
				temp_sd.dsc$b_dtype   = DSC$K_DTYPE_D;
				temp_sd.dsc$a_pointer = NULL;

				temp2_sd.dsc$w_length  = 0;
				temp2_sd.dsc$b_class   = DSC$K_CLASS_D;
				temp2_sd.dsc$b_dtype   = DSC$K_DTYPE_D;
				temp2_sd.dsc$a_pointer = NULL;

				str$get1_dx (&temp_len,&temp_sd);
				str$get1_dx (&temp_len,&temp2_sd);
				str$right   (&temp_sd,s1,&j);
				j = s2_len;
				str$left(&temp2_sd,&temp_sd,&j);
				result = str$compare(&temp2_sd,sd_ptr);

				str$free1_dx (&temp_sd);
				str$free1_dx (&temp2_sd);

				if (result == 0)
				{
					*index = i;
					i = s1_len - s2_len + 2;
					va_end(ap);
					return 1;
				}
			}
		}
		else
		{
			status = 0;
		}

		sd_ptr = va_arg(ap,struct dsc$descriptor_s *);

	}

	while (  sd_ptr != NULL  );

	va_end(ap);			// clean up argument pointer
	*subindex = 0;			// not found set back to zero
	return 0;
}

