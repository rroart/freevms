/*
 * lib_index.c
 *
 *	Code for VAX LIB$INDEX routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *	March 30, 1997 - Kevin Handy
 *		Preliminary design based on STR$POSITION.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "libdef.h"
#include "lib$routines.h"

/*************************************************************
 * lib$index
 *
 *	Searches for the first position of a substring within
 *	a source string. If LIB$INDEX finds the substring,
 *	it returns the relative position of the substring.
 *
 *	returns:
 *		0 if not found, otherwise returns the
 *		relative position.
 */
unsigned long lib$index(
	const struct dsc$descriptor_s* first_source_string,
	const struct dsc$descriptor_s* second_source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	unsigned long loop;			/* Outer loopp */

	/*
	 * Analyze source strings
	 */
	lib$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	lib$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * We loop from the start position to the end of the
	 * last possible match position in the string.
	 */
	for (loop = 0;
		loop <= s1_length - s2_length; loop++)
	{
		/*
		 * Inner loop does the actual comparison
		 */
		if (memcmp(s1_ptr + loop, s2_ptr, s2_length) == 0)
		{
			return loop + 1;
		}
	}

	/*
	 * Return the answer
	 */
	return 0;
}

