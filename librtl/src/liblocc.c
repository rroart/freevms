/*
 * liblocc
 *
 *	Code for VAX LIB$LOCC routine
 *
 * Description:
 *	Locate a character in a string by comparing successive bytes
 *	in the string whith the character specified. The search continues
 *	until the character is found or the string has no more characters.
 *
 *	If no character matches or if the string has a length of zero,
 *	then zero is returned
 *
 * Bugs:
 *
 * History
 *
 *	March 25, 2005 - Andrew Allison
 *		Procedure creation
 */

//#include <stdio.h>
//#include <string.h>
//#include <assert.h>
//#include "descrip.h"
//#include "strdef.h"
//#include "libdef.h"
#include "lib$routines.h"

unsigned long lib$locc( const struct dsc$descriptor_s* char_string,
			const struct dsc$descriptor_s* source_string)
{

char		*s1_ptr, *s2_ptr;
unsigned short	s1_len, s2_len;
unsigned long	i;

	lib$analyze_sdesc(char_string,   &s1_len, &s1_ptr);
	lib$analyze_sdesc(source_string, &s2_len, &s2_ptr);

	if (s1_len > 0 )
	{
		for (i = 0; i < s2_len ; i++)
		{
			if ( s1_ptr[0] == s2_ptr[i])
			  return i+1;
		}
	}
	return 0;
}

