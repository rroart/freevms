
/*
 *	LIB$COMPARE_NODENAME
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
 *	The compare two node names routine compares two node name to see
 *	if they resolve to the same full name.
 *
 * Bugs:
 *
 *
 * History
 *
 *	Mar 20, 2004 - Andrew Allison
 * 	 	Initial program creation	
 */
#include "libdef.h"
#include "descrip.h"
#include "str$routines.h"
#include "lib$routines.h"

//Prototype
int	not_valid_node_name (const struct dsc$descriptor_s *nodename);

// LIB$COMPARE_NODENAME
unsigned long lib$compare_nodename (const struct dsc$descriptor_s *nodename1,
				    const struct dsc$descriptor_s *nodename2,
				    	 unsigned long *result)
{
unsigned short	name1_len, name2_len, expandlen;
unsigned long	result_code;
char		*name1_ptr, *name2_ptr;
struct dsc$descriptor_s node1, node2;

str$analyze_sdesc (nodename1,&name1_len, &name1_ptr);

if ( name1_len > 1024 )
	return LIB$_INVSTRDES;
if ( not_valid_node_name (nodename1) )
	return LIB$_INVARG;

str$analyze_sdesc (nodename2,&name2_len, &name2_ptr);

if ( name2_len > 1024 )
	return LIB$_INVSTRDES;
if ( not_valid_node_name (nodename2) )
	return LIB$_INVARG;

lib$expand_nodename (nodename1, &node1, &expandlen);
lib$expand_nodename (nodename2, &node2, &expandlen);

*result = (unsigned long) str$compare (&node1, &node2 );

return result_code;
}

// *******************************

int	not_valid_node_name (const struct dsc$descriptor_s *nodename)
{
//	Put code to test node name her
	return 0;
}
