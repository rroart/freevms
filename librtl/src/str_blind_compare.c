/*
 * str.c
 *
 *	Code for VAX STR$BLIND_COMPARE routine
 *
 * Description:
 *
 * Bugs:
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design. Spelling errors are
 *		not my fault! Someone must have snuck them in
 *		there when I wasn't looking.
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Include 'ctype.h' to lose warnings when compiling
 *		with '-Wall'
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*
 * Local prototypes
 */
static int str__memicmp(const char* str1, const char* str2, size_t length);

/*************************************************************
 * str$case_blind_compare
 *
 *	Compares two strings without regard to case.
 *	The contents of the strings are not modified.
 *
 * Return:
 *
 *	-1 = str1 < str2
 *	0 = str1 == str2
 *	1 = str1 > str2
 */
long str$case_blind_compare(
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
	result = str__memicmp(s1_ptr, s2_ptr, min_length);

	/*
	 * Work on the result in case of equal in first part, but
	 * different total lengths.
	 */
	if (result == 0)
	{
		if (s1_length < s2_length)
		{
			result = -1;
		}
		else
		{
			if (s1_length > s2_length)
			{
				result = 1;
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

/*************************************************************
 * str__memicmp
 *
 *	This function does a memory-memory comparison (like
 *	memcmp), ignoring cases. It returns an integer less than,
 *	equal to, or greater than zero if str1 is found, respectively
 *	to be less than, to match, or be greater than str2.
 *
 *	stricmp, strcasecmp won't work, because they stop at
 *	the first null character, and we need to compare all
 *	characters.
 */

static int str__memicmp(const char* str1, const char* str2, size_t length)
{
	int result = 0;

	/*
	 * Do the comparison
	 */
	while((length != 0) && (result == 0))
	{
		result = tolower(*str1++) - tolower(*str2++);
		length--;
	}

	/*
	 * Return the result
	 */
	return result;
}


