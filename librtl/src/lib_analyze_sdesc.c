
/*
 *	
 * libanaalyze_sdesc.c
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
 * libanalyze_sdesc.c
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
 *	Mar 19, 2004 - Andrew Allison
 *		Copied code from str$analyze
 */

#include <stdio.h>
#include <assert.h>
#include "descrip.h"
#include "libdef.h"
#include "strdef.h"
#include "ssdef.h"
#include "str$routines.h"
#include "lib$routines.h"

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
	 * Only allow for strings
	 */
	if (str$$is_string_class(input_descriptor) != STR$_NORMAL)
	{
	  //DOSIGNAL(STR$_ILLSTRCLA);
		*word_integer_length = 0;
		*data_address = NULL;
		return LIB$_INVSTRDES;
	}

	/*
	 * Nothing stored here right now
	 */
	if (input_descriptor->dsc$a_pointer == NULL)
	{
//		assert(input_descriptor->dsc$w_length == 0);
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
	return SS$_NORMAL;
}

