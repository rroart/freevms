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
directory_function(unsigned char *argument, dcl$env *env)
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
*/
{
	return(DCL$SUCCESS);
}
