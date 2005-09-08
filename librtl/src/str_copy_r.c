/*
 * strcopy_r.c
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
 *	Code for VAX STR$COPY_R routine
 *
 * Description:
 *
 *	This file contains various 'str$' functions equivalent
 *	to those available in Vax/VMS string library.
 *
 * Bugs:
 *
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Change '=' to '==' in if statement. (Found with '-Wall');
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 26, 2004 - Andrew Allison
 * 		Added GNU License
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"


/*************************************************************
 * str$copy_r
 *
 */
unsigned long str$copy_r(struct dsc$descriptor_s* destination_string,
	const unsigned short* word_integer_source_length,
	const void *source_string_address)
{
unsigned short s1_length;
int	s2_length;
unsigned long length;	/* Length of first string */
unsigned long result = STR$_NORMAL;	/* Working result */
char* s1_ptr;			/* Pointer to first string */


if (source_string_address != NULL )
	s2_length = * word_integer_source_length;
else
	s2_length = 0;
	/*
	 * Do the normal sort of tests
	 */
	if ((result = str$$is_string_class(destination_string)) != STR$_NORMAL)
	{
		DOSIGNAL(result);
		return result;
	}

	/*
	 * Try to resize the destination
	 */

	length = *word_integer_source_length;
	result = str$$resize(destination_string, *word_integer_source_length);

	if (result == STR$_NORMAL)
	{
		/*
		 * Analyze the string we now have
		 */
		str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);
		/*
		 * Jam in the text
		 */
		result = str$$copy_fill(s1_ptr,(unsigned int) s1_length,
 source_string_address, (unsigned int) s2_length,' ' );
	}

	/*
	 * If we get here, it must have worked
	 */
	return result;
}

