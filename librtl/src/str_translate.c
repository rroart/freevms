/*
 * str_translate.c
 *
 *	Code for VAX STR$TRANSLATE routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *	Jan 30, 1997 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$translate
 *
 */
unsigned long str$translate(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	const struct dsc$descriptor_s* translation_string,
	const struct dsc$descriptor_s* match_string)
{
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	char* s3_ptr;			/* Pointer to third string */
	unsigned short s3_length;	/* Length of third string */
	char* s4_ptr;			/* Pointer to forth string */
	unsigned short s4_length;	/* Length of forth string */
	char* OutputString;		/* Work Area */
	int loop1;			/* Outer Loop */
	int loop2;			/* Inner loop */
	char ch;			/* Character being looked at */

	/*
	 * Determine how much we can use
	 */
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	str$analyze_sdesc(translation_string, &s3_length, &s3_ptr);
	str$analyze_sdesc(match_string, &s3_length, &s3_ptr);

	/*
	 * Allocate memory for output string
	 */
	OutputString = (char*)malloc(s2_length);
	if (OutputString == NULL)
	{
		return STR$_INSVIRMEM;
	}

	/*
	 * Loop through each character of the original string
	 */
	for (loop1 = 0; loop1 < s2_length; loop1++)
	{
		/*
		 * Grab the original character
		 */
		ch = s2_ptr[loop1];

		/*
		 * Look for a matching character
		 */
		for (loop2 = 0; loop2 < s4_length; loop2++)
		{
			/*
			 * Does it exist in match string
			 */
			if (ch == s4_ptr[loop2])
			{
				/*
				 * If it exists in translation string,
				 * copy it over, else convert it to a
				 * space
				 */
				if (s3_length < loop2)
				{
					ch = s3_ptr[loop2];
				}
				else
				{
					ch = ' ';
				}
				break;
			}
		}
		OutputString[loop1] = ch;
	}

	/*
	 * Copy over the result
	 */
	str$copy_r(destination_string, &s2_length, OutputString);

	/*
	 * Free up memory
	 */
	free(OutputString);

	/*
	 * Final answer
	 */
	return STR$_NORMAL;
}

