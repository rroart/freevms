
/*
 *	libutil.c
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

#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <values.h>
#include "descrip.h"
#include <stdlib.h>
#include "strdef.h"
#include "libdef.h"
#include "str$routines.h"
//#include "stdint.h"

#define MAXSTR 		132000
#define MAXUINT16	65536
#define TRUE		1
#define FALSE		0

int lib$$ncompare (char *s1,char *s2);


/*************************************************************/


int lib$$ncompare (char *s1,char *s2)

{
unsigned short	s1_len,  s2_len;
int	 	min_len, max_len, i;

	s1_len = strlen (s1);
	s2_len = strlen (s2);

	min_len = ( s1_len < s2_len) ? s1_len : s2_len;
	max_len = ( s1_len > s2_len) ? s1_len : s2_len;

	if ( s1_len > s2_len )
		return  1;
	if ( s1_len < s2_len )
		return -1;

//	The string are of equal length
	for (i = 0; i < max_len; i++)
	{	if ( s1[i] > s2[i] )
			return  1;
		if ( s1[i] < s2[i] )
			return -1;
	}

	return 0;
}

/*************************************************************/
