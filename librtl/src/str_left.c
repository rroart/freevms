/*
 * str_left.c
 *
 *	Code for VAX STR$LEFT routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 * History
 *
 *	Oct 15, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Changed 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$left
 *
 */
unsigned long str$left(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	const long* end_position)
{
	char* s2_ptr;			/* Pointer to second string */
	unsigned short s2_length;	/* Length of second string */
	int final_length;		/* Signed final length */
	unsigned short real_final_length; /* Usable final length */
	unsigned long result;	/* Result */
	unsigned long second_result = STR$_NORMAL;

	/*
	 * Determine how much we can use
	 */
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	final_length = min(s2_length, *end_position);

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
	result = str$copy_r(destination_string, &real_final_length, s2_ptr);

	/*
	 * Done
	 */
	return result;
}

