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
 *
 *	Mar 23, 2005 - Andrew Allison
 *		Added lib$routines.h for lib$stop prototype
 *
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"
#include "lib$routines.h"

/*************************************************************
 * str$analyze_sdesc
 *
 *	Analyze String Descriptor extracts the length and
 *	starting address of the data for a variety of
 *	string descriptor classes
 */
short int str$analyze_sdesc(const struct dsc$descriptor_s* input_descriptor,
	unsigned short* word_integer_length, char** data_address)
{
	/*
	 * Only allow for strings
	 */
	if (str$$is_string_class(input_descriptor) != STR$_NORMAL)
	{
		DOSIGNAL(STR$_ILLSTRCLA);
		*word_integer_length = 0;
		*data_address = NULL;
		lib$stop(STR$_ILLSTRCLA);
	}

	/*
	 * Nothing stored here right now
	 */
	if (input_descriptor->dsc$a_pointer == NULL)
	{
		assert(input_descriptor->dsc$w_length == 0);
		*word_integer_length = 0;
		*data_address = NULL;
	}

	/*
	 * Handle weirdness of VS (varying-length) string
	 */
	if (input_descriptor->dsc$b_class == DSC$K_CLASS_VS)
	{
		/*
		 * VS String
		 */
		*word_integer_length = 
			(unsigned short)*(input_descriptor->dsc$a_pointer);
		*data_address = input_descriptor->dsc$a_pointer + 2;
		assert(*word_integer_length <= input_descriptor->dsc$w_length);
	}
	else
	{
		*word_integer_length = input_descriptor->dsc$w_length;
		*data_address = input_descriptor->dsc$a_pointer;
	}
	return *word_integer_length;
}

