/*
 *	strright
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
 *	Code for VAX STR$RIGHT routine
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
 *	Jan 02, 1997 - Kevin Handy
 *		Fix starting position by one.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Feb 26, 2004 - Andrew Allison
 * 		Added GNU License
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$right
 *
 */
unsigned long str$right(struct dsc$descriptor_s* destination_string,
	const struct dsc$descriptor_s* source_string,
	const long* start_position)
{
	char* s2_ptr;				/* Pointer to second string */
	unsigned short s2_length;		/* Length of second string */
	int final_length;			/* Signed final length */
	unsigned short real_final_length;	/* Usable final length */
	unsigned long result;			/* Result */
	unsigned long second_result = STR$_NORMAL; /* Another possible result */
	int start_offset = *start_position - 1;	/* Real start character */

	/*
	 * Validate input
	 */
	if (start_offset <= 0)
	{
		start_offset = 0;
		second_result = STR$_STRTOOLON;
	}

	/*
	 * Determine how much we can use
	 */
        str$analyze_sdesc(source_string, &s2_length, &s2_ptr);

//	Is start position past end of source string
	if ( *start_position > s2_length )
	{
		str$free1_dx (destination_string);
		return STR$_ILLSTRPOS;
	}

//	Calculate our offsets
	final_length = s2_length - start_offset;

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
	s2_ptr += start_offset;
	result = str$copy_r(destination_string, &real_final_length, s2_ptr);
	destination_string->dsc$w_length = final_length;
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

