/*
 * str.c
 *
 *	Code for VAX STR$FREE1_DX routine
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
 *	Feb 4, 1997 - Kevin Handy
 *		Include 'stdlib.h' to lose warning with '-Wall'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$free1_dx
 *
 *	Deallocates the described string space and flags
 *	the descriptor as no string at all (dsc$a_pointer = 0,
 *	dsc$w_length = 0).
 */
unsigned long str$free1_dx(struct dsc$descriptor_s* string_descriptor)
{
	/*
	 * Validate class
	 */
	if (str$$is_string_class(string_descriptor) != STR$_NORMAL)
	{
		DOSIGNAL(STR$_ILLSTRCLA);
		return STR$_ILLSTRCLA;
	}

	/*
	 * Validate address
	 */
	if (string_descriptor->dsc$a_pointer == NULL)
	{
#if 1
		/*
		 * Already free
		 */
		return STR$_NORMAL;
#else
		DOSIGNAL(STR$_FATINTERR);
		return STR$_FATINTERR;
#endif
	}

	free(string_descriptor->dsc$a_pointer);
	string_descriptor->dsc$a_pointer = NULL;
	string_descriptor->dsc$w_length = 0;

	/*
	 * Succussful operation
	 */
	return STR$_NORMAL;
}


