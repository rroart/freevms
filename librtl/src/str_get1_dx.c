/*
 * str.c
 *
 *	Code for VAX STR$GET1_DX routine
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
 *	Feb 4, 1997 - Kevin Handy
 *		Include "stdlib.h" to lose warnings with '-Wall'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$get1_dx
 *
 *	Allocates a specified number of bytes of virtual memory
 *	to a specified string descriptor. The descriptor must
 *	be dynamic.
 *
 *	If the string descriptor already has dynamic memory
 *	allocated to it, but the amount is less than what is
 *	asked for, the space will be deallocated before it
 *	allocates new space.
 */
unsigned long str$get1_dx(unsigned short* word_integer_length,
	struct dsc$descriptor_s* character_string)
{
	char* new_memory;

	/*
	 * Validate string class
	 */
	if (character_string->dsc$b_class != DSC$K_CLASS_D)
	{
		DOSIGNAL(STR$_ILLSTRCLA);
		return(STR$_ILLSTRCLA);
	}

	/*
	 * If memory is already allocated, redo the allocation
	 */
	if (character_string->dsc$a_pointer != NULL)
	{
		/*
		 * Do we really need to change anything?
		 */
		if (character_string->dsc$w_length != *word_integer_length)
		{
			/*
			 * Reallocate old memory
			 */
			new_memory = (char*)realloc(character_string->dsc$a_pointer,
				*word_integer_length);
			if (new_memory == NULL)
			{
				DOSIGNAL(STR$_INSVIRMEM);
				return STR$_INSVIRMEM;
			}
			character_string->dsc$a_pointer = new_memory;
			character_string->dsc$w_length = *word_integer_length;
		}
	}
	else
	{
		/*
		 * Allocate some new memory
		 */
		character_string->dsc$a_pointer =
			(char*)malloc(*word_integer_length);
		if (character_string->dsc$a_pointer == NULL)
		{
			DOSIGNAL(STR$_INSVIRMEM);
			return STR$_INSVIRMEM;
		}
		character_string->dsc$w_length = *word_integer_length;
	}

	/*
	 * Successful
	 */
	return STR$_NORMAL;
}


