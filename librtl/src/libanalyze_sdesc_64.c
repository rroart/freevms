/*
 *	
 * libanalyze_sdesc_64.c
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
 * libanalyze_sdesc_64.c
 *
 *	Code for VAX STR$ANALYSE_SDESC_64 routine
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
 *		Copied code from str$analyze_sdesc_64
 */

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "descrip.h"
#include "strdef.h"
#include "libdef.h"
#include "ssdef.h"
#include "str$routines.h"
#include "lib$routines.h"

/*************************************************************
 * str$analyze_sdesc_64
 *
 *	Analyze String Descriptor extracts the length and
 *	starting address of the data for a variety of
 *	string descriptor classes
 *	type 0 = 32 bit  1 = 64 bit
 */
unsigned long lib$analyze_sdesc_64 (const struct dsc$descriptor_s* descrip,
				unsigned short	*word_integer_length,
				char 		**data_address,
				unsigned long	*descriptor_type )
{


	*descriptor_type = 0;		// 0 = 32 bit     1 = 64 bit

	/*
	 * Only allow for strings
	 */
	if (str$$is_string_class(descrip) != STR$_NORMAL)
	{
	  //DOSIGNAL(STR$_ILLSTRCLA);
		*word_integer_length = 0;
		*data_address = NULL;
		return LIB$_INVSTRDES;
	}

	/*
	 * Nothing stored here right now
	 */
	if (descrip->dsc$a_pointer == NULL)
	{
//		assert(input_descriptor->dsc$w_length == 0);
		*word_integer_length = 0;
		*data_address = NULL;
	}

	/*
	 * Handle weirdness of VS (varying-length) string
	 */
	if (descrip->dsc$b_class == DSC$K_CLASS_VS)
	{
		/*
		 * VS String
		 */
		*word_integer_length = 
			(unsigned short)*(descrip->dsc$a_pointer);
		*data_address = descrip->dsc$a_pointer + 2;
		assert(*word_integer_length <= descrip->dsc$w_length);
	}
	else
	{
		*word_integer_length = descrip->dsc$w_length;
		*data_address = descrip->dsc$a_pointer;
	}

	return SS$_NORMAL;
}

