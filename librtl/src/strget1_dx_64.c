/*
 *	strget1_dx_64
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

 /*
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
 *
 *	Feb 26, 2004 - Andrew Allison
 *		Added GNU License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$get1_dx_64
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
unsigned long str$get1_dx_64 (unsigned short* word_integer_length,
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
		int	i;
			/*
			 * Reallocate old memory
			 */
//			new_memory = (char*)realloc(character_string->dsc$a_pointer, *word_integer_length);
			*word_integer_length++;
			new_memory = (char*)calloc(1,*word_integer_length);
			*word_integer_length--;

			if (new_memory == NULL)
			{
				DOSIGNAL(STR$_INSVIRMEM);
				return STR$_INSVIRMEM;
			}
			for (i=0; i < *word_integer_length; i++)
			    new_memory[i] = character_string->dsc$a_pointer[i];

			free(character_string->dsc$a_pointer);
			character_string->dsc$a_pointer = new_memory;
			character_string->dsc$w_length  = *word_integer_length;
		}
	}
	else
	{
		/*
		 * Allocate some new memory
		 */
		character_string->dsc$a_pointer =
			(char*)calloc(1,*word_integer_length);
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


