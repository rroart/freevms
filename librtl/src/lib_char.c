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
 *	Should this function change the resulting string
 *	length to 1 if possible?
 *
 * History
 *
 *	March 30, 1997 - Kevin Handy
 *		Original version based upon  str$dupl_char
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"
#include "libdef.h"
#include "lib$routines.h"

/*************************************************************
 * lib$char
 *
 *	Transform byte to first character of string converts
 *	a single 8-bit ascii character to an ascii string
 *	consisting of a single character followed by trailing
 *	spaces, if needed.
 */
unsigned long lib$char(struct dsc$descriptor_s* destination_string,
	const char* ascii_character)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	unsigned long result = STR$_NORMAL;
	char UseChar = ' ';		/* Character to use for duplication */
	unsigned long alt_result;

	/*
	 * Check out the source string. It better have one
	 * single character in it.
	 */
	if (ascii_character != NULL)
	{
		UseChar = *ascii_character;
	}

	/*
	 * Resize the destination string
	 */
	alt_result = str$$resize(destination_string, 1);
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
		*s1_ptr = UseChar;
		memset(s1_ptr + 1, ' ', (size_t)(s1_length - 1));

		/*
		 * Did we truncate?
		 */
		if (s1_length < 1)
		{
			result = STR$_TRU;
		}
	}

	/*
	 * Done!
	 */
	return result;
}

