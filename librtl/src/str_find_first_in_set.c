/*
 * str_find_forst_in_set.c
 *
 *      Code for VAX STR$FIND_FIRST_IN_SET routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *      Jan 30, 1997 - Kevin Handy
 *              Preliminary design.
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
 * str$find_first_in_set
 *
 *	This function searches a string, one character at a time, from
 *	the left to right, comparing each character in the string to
 *	every character in a specified set of characters for which it
 *	is searching.
 *
 *	Returns:
 *		0 if no match found, otherwise returns the position
 *		in the source string where the first match is found.
 *
 */
long str$find_first_in_set(
        const struct dsc$descriptor_s* first_source_string,
        const struct dsc$descriptor_s* second_source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	int loop1;			/* Outer loop */
	int loop2;			/* Inner loop */

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * Quick exit when goofy stuff passed
	 */
	if ((s1_length == 0) || (s2_length == 0))
	{
		return 0;
	}

	/*
	 * I'm going to do it using a double loop. If the strings
	 * passed are long, then it might be better to create an
	 * aray[256] of matching characters and we could then loose
	 * the inner loop.
	 *
	 * The tradeoff comes in initializing the test table vs
	 * using an inner loop.
	 */

	/*
	 * Outer loop
	 */
	for (loop1 = 0; loop1 < s1_length; loop1++)
	{
		/*
		 * Inner loop
		 */
		for (loop2 = 0; loop2 < s2_length; loop2++)
		{
			/*
			 * Compare the characters
			 */
			if (s1_ptr[loop1] == s2_ptr[loop2])
			{
				return loop1 + 1;
			}
		}
	}

	/*
	 * If we get here, we don't have a match
	 */
	return 0;
}


