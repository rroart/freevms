/*
 * str.c
 *
 *	Code for VAX STR$TRIM routine
 *
 * Description:
 *
 *	Removes trailing spaces and tabs from the end of a
 *	string.
 *
 * Bugs:
 *
 *
 * History
 *
 *	Oct 16, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Lose variable 'i', which was defined but not used
 *		(found with '-Wall').
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$trim
 *
 */
unsigned long str$trim(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	unsigned short* resultant_length)
{
	char* s2_ptr;				/* Pointer to string */
	unsigned short s2_length;		/* Length of string */
	unsigned long result;			/* Result */

	/*
	 * Look at the original string
	 */
	str$analyze_sdesc(source_string, &s2_length, &s2_ptr);

	/*
	 * Determine the end of the string
	 */
	while ((s2_length > 0) &&
		((s2_ptr[s2_length-1] == ' ') || (s2_ptr[s2_length-1] == '\t')))
	{
		s2_length--;
	}

	/*
	 * Now, copy that much to the destination
	 */
	result = str$copy_r(destination_string, &s2_length, s2_ptr);
	*resultant_length = s2_length;

	/*
	 * Done
	 */
	return result;
}


