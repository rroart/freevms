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
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
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
	unsigned long result = STR$_NORMAL;	/* Result */

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
        str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	final_length = s1_length + s2_length;

	/*
	 * Extend the destination string
	 * (Assumes that re-sizing will not change the existing data!)
	 */
	result = str$$resize(destination_string, final_length);

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
	}

	/*
	 * Done
	 */
	return result;
}

