
/*
 *	str$replace
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
 *The author may be contacted at freevms@sympatico.ca
 *
 *	Andrew Allison
 *	50 Denlaw Road
 *	London, Ont
 *	Canada 
 *	N6G 3L4
 *
 */
/*
 * History
 *	Feb 26, 2004 - Andrew Allison
 *		Wrote str$replace code
 *
 * Description
 *
 *
 * 	str$replace (destination-string, source-string, start, end, substring)
 *
 * Bugs 
 *
 * 
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ssdef.h>
#include "strdef.h"
#include "descrip.h"
#include "str$routines.h"

/************************************/

unsigned long str$replace(	struct dsc$descriptor_s *destin,
			const	struct dsc$descriptor_s *source,
			const	long *begin, const long *finish,
			const	struct dsc$descriptor_s *sub)

{
int	status;
char	*s1_ptr;
unsigned short	s1_len, temp_len;
long	start,end,newstart,newend;
struct dsc$descriptor_s temp;


	start = *begin;
	end   = *finish;
	temp_len = 0;
	status = SS$_NORMAL;

	str$analyze_sdesc (source,&s1_len,&s1_ptr);
	if ( start < 1 )
		start = 1;

	if ( end > s1_len ) 
		end = s1_len;

	if ( start <= end )
	{
		str$$malloc_sd(&temp,"NULL");
		start--;
		str$left   (destin,source,&start);	// move over 1'st part
		str$append (destin,sub);		// copy in replacement
		end++;
		str$right  (&temp, source,&end);	// append rest of source
		str$append (destin,&temp);
		str$free1_dx (&temp);
	}
	else
	{	// start is after end 
		status = STR$_ILLSTRPOS;
		newstart = end;
		newend   = start;
		newstart--;
		str$left   (destin,source,&newstart);	// move over 1'st part
		str$$malloc_sd(&temp,"NULL");
		newstart++;
		str$pos_extr (&temp,source,&newstart,&newend); //extract overlap
		str$append   (destin,&temp);		// insert overlap twice
		str$append   (destin,&temp);
		newend++;
		str$right  (&temp, source,&newend);	// append rest of source
		str$append (destin,&temp);
		str$free1_dx (&temp);
	}

	return status;
}

