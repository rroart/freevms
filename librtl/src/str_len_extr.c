/*
 * str_len_extr.c
 *
 *	Code for VAX STR$POS_EXTR routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *	Feb 5, 1997 - Kevin Handy
 *		Preliminary design.
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$len_extr
 *
 */
unsigned long str$len_extr(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	const long* start_position,
	const long* longword_integer_length)
{
        char* s2_ptr;	 	 		/* Pointer to second string */
        unsigned short s2_length;	 	/* Length of second string */
	int final_length;			/* Signed final length */
	unsigned short real_final_length;	/* Usable final length */
	unsigned long result;			/* Result */
	unsigned long second_result = STR$_NORMAL; /* Another possible result */
	int start_offset = *start_position;	/* Real start character */

	/*
	 * Validate input
	 */
	if (start_offset <= 0)
	{
		start_offset = 1;
		second_result = STR$_STRTOOLON;
	}

	/*
	 * Determine how much we can use
	 */
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	if (*longword_integer_length < s2_length - (start_offset - 1))
	{
		final_length = *longword_integer_length;
	}
	else
	{
		final_length = s2_length - (start_offset - 1);
	}

	/*
	 * Now validate the final; length
	 */
	if (final_length < 0)
	{
		real_final_length = 0;
		second_result = STR$_STRTOOLON;
	}
	else
	{
		real_final_length = (unsigned short) final_length;
	}

	/*
	 * Move over the left part of the string
	 */
	s2_ptr += start_offset - 1;
	result = str$copy_r(destination_string,
		&real_final_length, s2_ptr);

	/*
	 * Done
	 */
	if (result == STR$_NORMAL)
	{
		return second_result;
	}
	else
	{
		return result;
	}
}

