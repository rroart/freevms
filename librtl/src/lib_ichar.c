/*
 * lib_ichar.c
 *
 *	Code for VAX LIB$ICHAR routine
 *
 * Description:
 *
 *	Convert first character of a string to an integer.
 *
 * Bugs:
 *
 *
 * History
 *
 *	March 30, 1997 - Kevin Handy
 *		Preliminary design.
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "libdef.h"
#include "lib$routines.h"

/*************************************************************
 * lib$ichar
 *
 */
int lib$ichar(const struct dsc$descriptor_s* input_descriptor)
{
	short word_integer_length;	/* Length of source string */
	char* data_address;		/* Address of actual data */
	long result = 0;		/* Result to return */
	unsigned long status;

	/*
	 * use STR$ANALYSE_SDESC instead of LIB$ version, because
	 * the LIB$ version calls the STR$ version.
	 */
	status = lib$analyze_sdesc(input_descriptor, &word_integer_length,
		&data_address);

	if (word_integer_length >= 1)
	{
		result = *data_address - '0';
	}

	return result;
}

