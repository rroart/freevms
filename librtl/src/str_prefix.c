/*
 * str_prefix.c
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
 *	Feb 5, 1997 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 11, 1997 - Christof Ziele
 *		Change 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$prefix
 *
 */
unsigned long str$prefix(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	unsigned short final_length;	/* Final sstring length */
	char* work;			/* Working area */
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
	 * Create a sufficiently large work area
	 */
	work = (char*)malloc((unsigned)final_length);
	if (work == NULL)
	{
		return STR$_INSVIRMEM;
	}

	/*
	 * Concatinate the strings
	 */
	memcpy(work, s1_ptr, (size_t)s1_length);
	memcpy(work + s1_length, s2_ptr, (size_t)s2_length);

	/*
	 * Save the result
	 */
	result = str$copy_r(destination_string, &final_length, work);

	/*
	 * Release memory
	 */
	free(work);

	/*
	 * Done
	 */
	return result;
}

