/*
 * lib_len.c
 *
 *	Code for VAX LIB$LEN routine
 *
 * Description:
 *
 *	Return length of a string as a longword
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
 * lib$len
 *
 */
unsigned short lib$len(const struct dsc$descriptor_s* input_descriptor)
{
	short word_integer_length;	/* Length of source string */
	char* data_address;		/* Address of actual data */

	/*
	 * use STR$ANALYSE_SDESC instead of LIB$ version, because
	 * the LIB$ version calls the STR$ version.
	 */
	lib$analyze_sdesc(input_descriptor, &word_integer_length,
		&data_address);

	return word_integer_length;
}

