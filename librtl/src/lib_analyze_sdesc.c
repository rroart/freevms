/*
 * str.c
 *
 *	Code for VAX STR$ANALYSE_SDESC routine
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
#include <assert.h>
#include "descrip.h"
#include "str$routines.h"
#include "lib$routines.h"
#include "ssdef.h"

/*************************************************************
 * lib$analyze_sdesc
 *
 *	Analyze String Descriptor extracts the length and
 *	starting address of the data for a variety of
 *	string descriptor classes
 */
unsigned long lib$analyze_sdesc(const struct dsc$descriptor_s* input_descriptor,
	unsigned short* word_integer_length, char** data_address)
{
	/*
	 * Since I've already written the STR$ version, lets
	 * re-use it.
	 */
	str$analyze_sdesc(input_descriptor, word_integer_length,
		data_address);

	return SS$_NORMAL;
}

