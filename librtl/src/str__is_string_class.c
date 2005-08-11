/*
 * str.c
 *
 *	Code for VAX STR$$IS_STRING_CLASS routine
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
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$$is_string_class
 *
 *	This function is used to determine if the descriptor
 *	passed in is actually a viable string for this set of
 *	functions.
 */
unsigned long str$$is_string_class(const struct dsc$descriptor_s* test_string)
{
	/*
	 * Did we get passed anything?
	 */
	if (test_string == NULL)
	{
		return STR$_FATINTERR;
	}

	/*
	 * Make sure it is a atring class we can handle
	 */
	switch (test_string->dsc$b_class)
	{
	case DSC$K_CLASS_Z:
	case DSC$K_CLASS_S:
	case DSC$K_CLASS_SD:
	case DSC$K_CLASS_VS:
	case DSC$K_CLASS_D:
	case DSC$K_CLASS_A:
	case DSC$K_CLASS_NCA:
		break;

	default:
		return STR$_ILLSTRCLA;
	}

	return STR$_NORMAL;
}

