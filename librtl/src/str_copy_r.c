/*
 * str.c
 *
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
	const void* source_string_address)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	unsigned long result = STR$_NORMAL;	/* Working result */

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
	result = str$$resize(destination_string,
		*word_integer_source_length);

	if (result == STR$_NORMAL)
	{
		/*
		 * Analyze the string we now have
		 */
		str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);

		/*
		 * Jam in the text
		 */
		result = str$$copy_fill(s1_ptr, s1_length,
			source_string_address, *word_integer_source_length,
			' ');
	}

	/*
	 * If we get here, it must have worked
	 */
	return result;
}

