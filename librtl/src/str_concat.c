/*
 * str.c
 *
 *	Code for VAX STR$CONACT routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *	List MUST be terminated will a null item, because there's
 *	no standard way to determine where the list ends.
 *
 * History
 *
 *	Oct 18, 1996 - Kevin Handy
 *		Preliminary design.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$concat
 *
 */
#undef str$concat
unsigned long str$concat(struct dsc$descriptor_s* destination_string,
	...)
{
	va_list vlist;
	unsigned long result;
	struct dsc$descriptor_s* Item;

	/*
	 * Blank out current string
	 */
	result = str$free1_dx(destination_string);

	/*
	 * Set up for varargs
	 */
	va_start(vlist, destination_string);

	/*
	 * Loop until we get a null (BUG)
	 */
	while((Item = va_arg(vlist, struct dsc$descriptor_s*), Item != NULL) &&
		(result == STR$_NORMAL))
	{
		result = str$append(destination_string, Item);
	}

	/*
	 * Terminate varargs
	 */
	va_end(vlist);

	/*
	 * Done
	 */
	return result;
}


