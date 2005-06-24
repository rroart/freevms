
/*
 *	LIB$DATE_TIME	
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
 *	The date and time returned as a string routine returns the FreeVMS
 *	system date and time in the semantics of a user provided string.
 *
 * Bugs:
 *
 *
 * History
 *
 *	Apr 07, 2004 - Andrew Allison
 * 	 	Initial program creation	
 */
#include "lib$routines.h"
#include "str$routines.h"
#include <stdio.h>
#include <time.h>
#include <string.h>


unsigned long lib$date_time (struct dsc$descriptor_s *date_time_string )
{
unsigned long result;
unsigned short s1_len;
char	*s1_ptr;
time_t	time_ptr;
struct tm *local_time;
char month[36] = "JANFEBMARAPRJUNJULAUGSEPOCTNOVDEC";

str$analyze_sdesc (date_time_string, &s1_len, &s1_ptr);

// get date and time to extract day light saving time flag
time (&time_ptr);
local_time = localtime (&time_ptr);

// determine if daylight saving time is in effect
if (local_time->tm_isdst > 0 )			// DST in effect
	time_ptr += 3600;
else if ( local_time->tm_isdst == 0 )		// DST not in effect
	;
else if ( local_time->tm_isdst < 0 )		// Flag not used
	;

// convert time_t to struct tm
local_time = localtime (&time_ptr);

sprintf (&s1_ptr[ 0],"%.2d",local_time->tm_mday);
          s1_ptr[ 2] = '-';
strcpy  (&s1_ptr[ 3],&month[local_time->tm_mon * 3]);
          s1_ptr[ 6] = '-';
sprintf (&s1_ptr[ 7],"%.4d",local_time->tm_year + 1900);
          s1_ptr[11] = ' ';
sprintf (&s1_ptr[12],"%.2d",local_time->tm_hour);
          s1_ptr[14] = ':';
sprintf (&s1_ptr[15],"%.2d",local_time->tm_min);
          s1_ptr[17] = ':';
sprintf (&s1_ptr[18],"%.2d",local_time->tm_sec);
          s1_ptr[20] = '.';
          s1_ptr[21] = '0';
          s1_ptr[22] = '0';


return result;
}
