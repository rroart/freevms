/*
 * str.c
 *
 *	Code for VAX STR$COMPARE routine
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
 *
 *	Feb 19, 1997 - Kevin Handy
 *		Fix so that a shorter string is assumed to be padded
 *		with spaces.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$compare
 *
 *	Compares two strings for the same contents. This
 *	routine distinguishes between upper and lower case
 *	strings.
 *
 *	returns:
 *		-1 if first < second
 *		0 if equal
 *		1 if first > second
 */
long str$compare(
	const struct dsc$descriptor_s* first_source_string,
	const struct dsc$descriptor_s* second_source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	unsigned short min_length;	/* length of shortest string */
	long result;			/* Result of comparison */

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * Calculate length to compare
	 */
	min_length = (s1_length < s2_length) ? s1_length : s2_length;

	/*
	 * Compare the two strings.
	 * Use 'memcmp' instead of 'strncmp' because we may have NULL's
	 * in our strings.
	 */
	result = memcmp(s1_ptr, s2_ptr, min_length);

	/*
	 * Work on the result in case of equal in first part, but
	 * different total lengths.
	 */
	if (result == 0)
	{
		min_length++;
		if (s1_length < s2_length)
		{
			while ((min_length < s2_length) && (result == 0))
			{
				if (s2_ptr[min_length++] != ' ')
				{
					result = -1;
				}
			}
		}
		else
		{
			if (s1_length > s2_length)
			{
				while ((min_length < s1_length) && (result == 0))
				{
					if (s1_ptr[min_length++] != ' ')
					result = 1;
				}
			}
		}
	}

	/*
	 * Normalize the result
	 */
	if (result < -1)
	{
		result = -1;
	}

	if (result > 1)
	{
		result = 1;
	}

	/*
	 * Return the answer
	 */
	return result;
}

