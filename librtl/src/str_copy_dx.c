/*
 * str.c
 *
 *	Code for VAX STR$COPY_DX routine
 *
 * Description:
 *
 *
 * Bugs:
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
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$copy_dx
 *
 */
unsigned long str$copy_dx(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	unsigned long result = STR$_NORMAL;	/* Working result */

	/*
	 * Get information about source string
	 */
	str$analyze_sdesc(source_string, &s1_length, &s1_ptr);

	/*
	 * Now, we can copy things over using str$copy_r
	 */
	result = str$copy_r(destination_string,
		&s1_length, s1_ptr);

	/*
	 * If we get here, it must have worked
	 */
	return result;
}

