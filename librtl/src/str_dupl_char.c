/*
 * str.c
 *
 *	Code for VAX STR$DUPL_CHAR routine
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
 *	Feb 4, 1997 - Kevin Handy
 *		Lose variables 's2_length', 's2_ptr' which were
 *		defined but never used (found with '-Wall')
 *
 *	Feb 7, 1997 - Kevin Handy
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 11, 1997 - Kevin Handy
 *		Changed UseCount from 'long' to 'unsigned short'
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$dupl_char
 *
 */
unsigned long str$dupl_char(struct dsc$descriptor_s* destination_string,
	const long* repetition_count,
	const char* ascii_character)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	unsigned long result = STR$_NORMAL;
	char UseChar = ' ';		/* Character to use for duplication */
	unsigned short UseCount;
	unsigned long alt_result;

	/*
	 * Get the repitition count
	 */
	if (repetition_count == NULL)
	{
		UseCount = 1;
	}
	else
	{
		UseCount = *repetition_count;
	}

	/*
	 * Check out the source string. It better have one
	 * single character in it.
	 */
	if (ascii_character != NULL)
	{
		UseChar = *ascii_character;
	} else {
		UseChar = ' ';
	}

	/*
	 * Validate the repitition count
	 */
#if 0	/* Not possible with unsigned */
	if (UseCount < 0)
	{
		UseCount = 0;
		result = STR$_NEGSTRLEN;
	}
#endif

	if (UseCount > 65535L)
	{
		DOSIGNAL(STR$_STRTOOLON);
		return STR$_STRTOOLON;
	}

	/*
	 * Resize the destination string
	 */
	alt_result = str$$resize(destination_string, UseCount);
	if (alt_result != STR$_NORMAL)
	{
		result = alt_result;
	}
	else
	{
		/*
	 	 * Now, see what we've got for a destination
	 	 */
		str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);
		memset(s1_ptr, UseChar, (size_t)min(s1_length, UseCount));

		/*
		 * Did we truncate?
		 */
		if (UseCount > s1_length)
		{
			result = STR$_TRU;
		}
	}

	/*
	 * Done!
	 */
	return result;
}

