
/*
 * str_append.c
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
 * str_append.c
 *
 *	Code for VAX STR$APPEND routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 24, 2004 - Andrew Allison
 * 		Inserted GNU Licence
 *
 *	Mar 1, 2004 - Andrew Allison
 *		Changed return status from STR$_NORMAL to SS$_NORMAL
 *		Added code to handle destination string being NULL
 *
 *
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "ssdef.h"
#include "str$routines.h"

/*************************************************************
 * str$append
 *
 */
unsigned long str$append(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;			/* Pointer to first string */
        unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	unsigned short final_length;	/* Final string length */
	unsigned long result = SS$_NORMAL;	/* Result */
	unsigned long	size_check;	// check that haven't gone over

	/*
	 * Destination MUST be a dynamic string
	 */
	if (destination_string->dsc$b_class != DSC$K_CLASS_D)
	{
		DOSIGNAL(STR$_ILLSTRCLA);
		return STR$_ILLSTRCLA;
	}

        /*
         * Analyze strings
         */
        str$analyze_sdesc ( destination_string, &s1_length, &s1_ptr);
        str$analyze_sdesc ( source_string,      &s2_length, &s2_ptr);

	size_check   = s1_length + s2_length;

#define MAXCSTR16     65535
	if ( size_check > MAXCSTR16 )
	{	final_length = MAXCSTR16;

		result = str$$resize ( destination_string, final_length);

// reduce the length so the combined strings will fit
		s2_length = MAXCSTR16 - s1_length;
		if (result == STR$_NORMAL)
		{
			memcpy(destination_string->dsc$a_pointer + s1_length,
				s2_ptr, s2_length);
			destination_string->dsc$w_length = final_length;
			result = STR$_TRU;
		}
		else
			result = STR$_FATINTERR;

	}
	else
	{	final_length = s1_length + s2_length;

		/*
		 * Extend the destination string
		 * (Assumes that re-sizing will not change the existing data!)
		 */
		result = str$$resize ( destination_string, final_length);
		/*
		 * Append the source onto the destination.
		 * Must assume that everything is ok for this.
		 */
		if (result == STR$_NORMAL)
		{
			/*
			 * We can directly use the pointer here, because we
			 * know that we have a dynamic string, and not one
			 * of the weird ones. Don't need to analyze again.
			 */
			memcpy(destination_string->dsc$a_pointer + s1_length,
				s2_ptr, s2_length);
			destination_string->dsc$w_length = final_length;
			result = SS$_NORMAL;
		}
		else
		{
			result = STR$_FATINTERR;
		}
	}
	/*
	 * Done
	 */
	return result;
}

