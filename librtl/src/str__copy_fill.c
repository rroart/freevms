/*
 * str.c
 *
 *	Code for VAX STR$$COPY_FILL routine
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
 * str$$copy_fill
 *
 *	Copies over text from source to dest.
 */
unsigned long str$$copy_fill_not(char* dest_ptr, unsigned short dest_length,
	const char* source_ptr, unsigned short source_length, char fill)
{
	unsigned short max_copy;
	int fill_loop;

	/*
	 * Copy over however much can.
	 */
	max_copy = min(dest_length, source_length);
	memcpy(dest_ptr, source_ptr, max_copy);

	/*
	 * Fill in the rest
	 */
	for (fill_loop = max_copy; fill_loop < dest_length; fill_loop++)
	{
		dest_ptr[fill_loop] = fill;
	}

	/*
	 * Figure out what to return
	 */
	if (source_length > dest_length)
	{
		return STR$_TRU;
	}
	else
	{
		return STR$_NORMAL;
	}

}

