/*
 * str.c
 *
 *	Code for VAX STR$UPCASE routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *
 * History
 *
 *	Oct 16, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$upcase
 *
 */
unsigned long str$upcase(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;				/* Pointer to string */
	unsigned short s1_length;		/* Length of string */
	unsigned long result;			/* Working result */

	/*
	 * Copy over the string, retaining original case
	 */
	result = str$copy_dx(destination_string, source_string);

	/*
	 * Analyze destination string
	 */
	if ((result & 1) == 1)
	{
		/*
		 * Analyse the copies string
		 */
		str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);

		/*
		 * Transform it to upper case
		 */
		while(s1_length > 0)
		{
			*s1_ptr = toupper(*s1_ptr);
			s1_ptr++;
			s1_length--;
		}

	}

	/*
	 * Done
	 */
	return result;
}

