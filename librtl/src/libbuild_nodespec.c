
/*
 *	LIB$BUILD_NODESPEC
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
 *	The build a node-name specification routine builds a node-name
 *	specification from the primary node name. The output node-name
 *	specification can be used fotr other node-name parsing operations
 *
 * Bugs:
 *
 *
 * History
 *
 *	Mar 20, 2004 - Andrew Allison
 * 	 	Initial program creation	
 */

#include "ssdef.h"
#include "libdef.h"
#include "descrip.h"
#include <stdio.h>
#include "lib$routines.h"
#include "str$routines.h"

unsigned long lib$build_nodespec(const 	struct	dsc$descriptor_s *primary,
				       	struct	dsc$descriptor_s *nodespec,
 				 const 	struct	dsc$descriptor_s *acs,
				 const 	struct	dsc$descriptor_s *secondary,
				 unsigned short	*nodespec_length )
{
unsigned long	result_code, max_length;
unsigned short	primary_length;
signed long	quote_pos, start_pos;
char		*primary_ptr;
struct dsc$descriptor_s	 temp, quote, double_quote;

result_code = SS$_NORMAL;
max_length  = 1024;
start_pos   = 0;

str$analyze_sdesc (primary, &primary_length, &primary_ptr);

if ( primary_ptr == NULL )
	return LIB$_INVARG;

//if ( primary_length > 1024 )
//	return LIB$_NODTOOLNG;

//create descriptors containing 1 and 2 double quotes
str$$malloc_sd (&temp,"NULL");
str$$malloc_sd (&quote,"\"");
str$$malloc_sd (&double_quote, "\"\"");

//copy primary node name to output - nodespec
str$copy_dx (nodespec,primary);

//duplicate quotation marks
quote_pos = str$position (nodespec,&quote,&start_pos);
if ( quote_pos != 0 )
	str$replace (&temp,nodespec,&quote_pos,&quote_pos,&double_quote);

// add 2 -- to get past the one's we put in
quote_pos += 2;
quote_pos = str$position (&temp,&quote,&quote_pos);

if ( quote_pos != 0 )
	str$replace(nodespec,&temp,&quote_pos,&quote_pos,&double_quote);
	
//	Append access control string to primary node name
if ( (unsigned int) acs != (unsigned int) NULL )
	str$append (nodespec,acs);

//	Append secondary node name
if ( (unsigned int) secondary != (unsigned int) NULL )
	str$append (nodespec,secondary);

// Truncate node name to 1024 characters
str$analyze_sdesc (primary, &primary_length, &primary_ptr);
if ( primary_length > 1024 )
{	result_code = LIB$_STRTRU;
	str$left (nodespec, primary,&max_length);
}

//	Set nodespec length
if ( nodespec_length != NULL )
{	str$analyze_sdesc (nodespec, &primary_length, &primary_ptr);
	*nodespec_length = primary_length;
}

str$free1_dx (&quote);
str$free1_dx (&double_quote);

return result_code;
}

