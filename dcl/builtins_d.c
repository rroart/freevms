/*
================================================================================
  DCL/2 version 1.00
  	Digital Command Language

    Date de création    : 28 novembre 2001

    Tous droits réservés à l'auteur, Joël BERTRAND
	All rights reserved worldwide
================================================================================
*/


/*
================================================================================
  Copyright (C) 2001 BERTRAND Joël

  This file is part of DCL/2.

  DCL/2 is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.
            
  DCL/2 is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.
              
  You should have received a copy of the GNU General Public License
  along with Octave; see the file COPYING.  If not, write to the Free
  Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
================================================================================
*/


#include "dcl.h"

int
directory_function(unsigned char *argument, dcl$command *self,
		dcl$command *commands, dcl$env *env)
/*
DIRECTORY

     Provides a list of files or information about a file or group of
     files.

     Requires execute (E) access to look up files you know the names
     of, read (R) access to read or list a file or to use a file
     name with the asterisk (*)  and the percent sign (%) wildcard
     characters to look up files.

     Format

       DIRECTORY  [filespec[,...]]

       DIRECTORY/FTP  directorySpec
   



  Additional information available:

  Parameter  Qualifiers
  /ACL       /BACKUP    /BEFORE    /BRIEF     /BY_OWNER  /COLUMNS   /CREATED
  /DATE      /EXACT     /EXCLUDE   /EXPIRED   /FILE_ID   /FTP       /FULL
  /GRAND_TOTAL          /HEADING   /HIGHLIGHT /MODIFIED  /OUTPUT    /OWNER
  /PAGE      /PRINTER   /PROTECTION           /SEARCH    /SECURITY  /SELECT
  /SHELVED_STATE        /SINCE     /SIZE      /STYLE     /TIME      /TOTAL
  /TRAILING  /VERSIONS  /WIDTH     /WRAP

Examples:
$ dir /full

Directory SYS$SYSDEVICE:[JKB]

ESSAI.F;1                     File ID:  (3183,6,0)            
Size:            1/9          Owner:    [GUESTS,JKB]
Created:   15-JUN-2001 16:51:23.47
Revised:   15-JUN-2001 16:51:23.68 (1)
Expires:   <None specified>
Backup:    <No backup recorded>
Effective: <None specified>
Recording: <None specified>
File organization:  Sequential
Shelved state:      Online 
Caching attribute:  Writethrough
File attributes:    Allocation: 9, Extend: 0, Global buffer count: 0
                    No version limit
Record format:      Variable length, maximum 0 bytes, longest 21 bytes
Record attributes:  Carriage return carriage control
RMS attributes:     None
Journaling enabled: None
File protection:    System:RWED, Owner:RWED, Group:RE, World:
Access Cntrl List:  None
Client attributes:  None

MAIL.MAI;1                    File ID:  (3298,4,0)            
Size:           45/45         Owner:    [GUESTS,JKB]
Created:   18-JUN-2001 10:25:00.08
Revised:    7-NOV-2001 16:58:50.87 (21)
Expires:   <None specified>
Backup:    <No backup recorded>
Effective: <None specified>
Recording: <None specified>
File organization:  Indexed, Prolog: 3, Using 2 keys
                             In 3 areas
Shelved state:      Online 
Caching attribute:  Writethrough
File attributes:    Allocation: 45, Extend: 15, Maximum bucket size: 5
                    Global buffer count: 0, No version limit
                    Contiguous best try
Record format:      Variable length, maximum 2048 bytes, longest 0 bytes
Record attributes:  None
RMS attributes:     None
Journaling enabled: None
File protection:    System:RW, Owner:RW, Group:, World:
Access Cntrl List:  None
Client attributes:  None

Total of 2 files, 46/54 blocks.
$ dir /protection

Directory SYS$SYSDEVICE:[JKB]

ESSAI.F;1            (RWED,RWED,RE,)
MAIL.MAI;1           (RW,RW,,)

Total of 2 files.
$ dir /owner

Directory SYS$SYSDEVICE:[JKB]

ESSAI.F;1            [GUESTS,JKB]                   
MAIL.MAI;1           [GUESTS,JKB]                   

Total of 2 files.
$ 
*/
{
	int					status;

	argument = next_argument(argument);

	if (strlen(argument) == 0)
	{
		/*
		 * To few arguments
		 */

		return(DCL$SUCCESS);
	}

	status = parsing(argument, commands, env, DCL$QUALIFIER, self);

	return(status);
}
