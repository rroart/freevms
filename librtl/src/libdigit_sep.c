
/*
 *	LIB$DIGIT_SEP
 *
 *	Copyright (C) 2004 Andrew Allison
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
 *The author(s) may be contacted at:
 *
 *	Andrew Allison		freevms@sympatico.ca
 *
 *				Andrew Allison
 *				50 Denlaw Road
 *				London, Ont
 *				Canada 
 *				N6G 3L4
 *
 *
 *
 * Description:
 *	Returns the system's digit separator symbol
 *
 * Bugs:
 *
 *
 * History
 *
 *	Apr 12, 2004 - Andrew Allison
 * 	 	Initial program creation	
 */

#include "lib$routines.h"
#include "str$routines.h"
#include <stdlib.h>

unsigned long lib$digit_sep (struct dsc$descriptor_s *digit_sep,
				unsigned short *result_length)
{
char	*s1_ptr;
unsigned long result;
unsigned short s1_len;

result = 1;

str$analyze_sdesc (digit_sep, &s1_len, &s1_ptr);

s1_len = 1;
str$copy_r (digit_sep,&s1_len,",");

if ( result_length != NULL )
	*result_length = s1_len;

return result;
}
