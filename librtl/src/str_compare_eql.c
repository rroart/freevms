/*
 * str.c
 *
 *	Code for VAX STR$OMPARE_EQL routine
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
 *		Lose 'result' variable definition, which was never
 *		used. (Warning from '-Wall')
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/************************************************************
 * str$compare_eql
 *
 *	Compares two strings for equality, in both length
 *	and contents.
 */
long str$compare_eql(
	const struct dsc$descriptor_s* first_source_string,
	const struct dsc$descriptor_s* second_source_string)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */

	/*
	 * Analyze source strings
	 */
	str$analyze_sdesc(first_source_string, &s1_length, &s1_ptr);
	str$analyze_sdesc(second_source_string, &s2_length, &s2_ptr);

	/*
	 * Failure if not same length
	 */
	if (s1_length != s2_length)
	{
		return 1;
	}

	/*
	 * Failure if not identical contents
	 */
	if (memcmp(s1_ptr, s2_ptr, s2_length) != 0)
	{
		return 1;
	}

	/*
	 * Must be equal
	 */
	return 0;
}


