/*
 * str.c
 *
 *	Code for VAX STR$ELEMENT routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *
 * History
 *
 *	Feb 3, 1997 - Kevin Handy
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
unsigned long str$element(struct dsc$descriptor_s* destination_string,
	const long* find_element_number,
	const struct dsc$descriptor_s* delimiter_string,
	const struct dsc$descriptor_s* source_string)
{
	char* s1_ptr;			/*Pointer to first string */
	unsigned short s1_length;	/* Length of first string */
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	char* s3_ptr	;		/* Pointer to third string */
	unsigned short s3_length;	/* Length of third string */
	short res_ptr;			/* Index into source*/
	unsigned short res_length;	/* Index to end  of element */
	char delimiter;			/* Delimiter character */
	unsigned long result1;
	unsigned long result = STR$_NORMAL;	/* Result */
	int element_number = *find_element_number;

	/*
	 * Negitive numbers start at first element
	 */
	if (element_number < 0)
	{
		element_number = 0;
	}


        /*
         * Analyze strings
         */
        str$analyze_sdesc(destination_string, &s1_length, &s1_ptr);
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	str$analyze_sdesc(delimiter_string, &s3_length, &s3_ptr);

	/*
	 * Check out delimiter. Assume '\0' if none given.
	 */
	if (s3_length != 1)
	{
		return STR$_INVDELIM;
	}
	delimiter = *s3_ptr;

	/*
	 * Look for a sterting position
	 */
	res_ptr = 0;
	while ((element_number > 0) && (res_ptr < s2_length))
	{
		if (s2_ptr[res_ptr] == delimiter)
		{
			element_number--;
		}
		res_ptr++;
	}


	/*
	 * Should now point to result string, or not have a
	 * any element available
	 */
	res_length = 0;
	if (element_number != 0)
	{
		result = STR$_NOELEM;
	}
	else
	{
		/*
		 * Look for ending delimiter
		 */
		while (((res_ptr + res_length) < s2_length) &&
			(s2_ptr[res_ptr + res_length] != delimiter))
		{
			res_length++;
		}
	}

	/*
	 * Copy over the result
	 */
	s1_ptr = s2_ptr + res_ptr;
	result1 = str$copy_r(destination_string, &res_length, s1_ptr);

	if (result1 != STR$_NORMAL)
	{
		result = result1;
	}

	/*
	 * Done
	 */
	return result;
}

