
/*
 *	libconvert_date_string	
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
 *	The convert date string to quadword routine converts an absolute date
 *	string into an VMS internal format date-time quadword. That is given
 *	an input date/time string of a specified format.lib$convert_date_string
 *	coverts this string toan VMS internal format time.
 *
 * Relative Date since Nov 17 1858 
 * number of 100 nanoseconds units 
 * updated every 10 millisecs
 *
 * Bugs:
 *
 *
 * History
 *
 *	Mar 21, 2004 - Andrew Allison
 * 	 	Initial program creation	
 */

#include "lib$routines.h"
#include "libdef.h"
#include "descrip.h"
#include <stdlib.h>
#include <assert.h>
//prototypes
void load_default_values (unsigned long flag, unsigned char date[8] );
void check_reserved_bits ( unsigned long flag);

/********************************************/

unsigned long lib$convert_date_string (const struct dsc$descriptor_s *time_name,
					long long *date_time,
					unsigned long *user_context,
					unsigned long *field_flags,
					unsigned short *field_defaults,
					unsigned long *default_fields )

{
unsigned long result_code;
unsigned long context, flags, defaults, fields;
unsigned char date[8];


// implement later
if ( user_context == NULL )
	;
else
	context = *user_context;

if ( field_flags == NULL )
	flags = 0x78;
else
	flags = *field_flags;

if ( field_defaults == NULL )
	defaults = 0x78;
else
	defaults = *field_defaults;

if ( default_fields == NULL )
	fields = 0x78;
else
	fields = *default_fields;

check_reserved_bits (defaults);
load_default_values (defaults, date);

return result_code;
}

void load_default_values (unsigned long flag, unsigned char date[8] )
{

// bit 0 	year
if ( flag & 0x0001 )	date[0] = 4;
// bit 1 	month
if ( flag & 0x0002 )	date[1] = 3;
// bit 2	day of month
if ( flag & 0x0004 )	date[2] = 22;
// bit 3	hours
if ( flag & 0x0008 ) 	date[3] = 15;
// bit 4	minutes
if ( flag & 0x0010 )	date[4] = 1;
// bit 5	seconds
if ( flag & 0x0020 )	date[5] = 55;
// bit 6	fractional seconds
if ( flag & 0x0040 ) 	date[6] = 50;

}

void check_reserved_bits ( unsigned long flag)
{
if ( flag & 0xFF80 )
	assert ("Reserved bits \n");

}
