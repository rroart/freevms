/*
 *	strpos_extr
 *
 *	Copyright (C) 2003 Andrew Allison
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *The authors may be contacted at:
 *
 *	Andrew Allison		freevms@sympatico.ca
 *
 *				Andrew Allison
 *				50 Denlaw Road
 *				London, Ont
 *				Canada 
 *				N6G 3L4
 *
 */

 /* str_right.c
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
 *	Oct 15, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places
 *
 *	Feb 26, 2004 - Andrew Allison
 *		Added GNU License
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$pos_extr
 *
 */
unsigned long str$pos_extr(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	const long* start_position,
	const long* end_position)
{
	char* s2_ptr;				/* Pointer to second string */
	unsigned short s2_length;		/* Length of second string */
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
		second_result = STR$_ILLSTRPOS;
	}

	/*
	 * Determine how much we can use
	 */
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);
	final_length = s2_length - start_offset + 1;

	/*
	 * Handle the end position business
	 */
	if (final_length > *end_position - *start_position + 1)
	{
		final_length =
			(*end_position - *start_position + 1);
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

