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
copy_function(unsigned char *argument, dcl$env *env)
/*
COPY

     Creates a new file from one or more existing files. The COPY
     command can do the following:

     o  Copy an input file to an output file.

     o  Concatenate two or more input files into a single output file.

     o  Copy a group of input files to a group of output files.

     Format

       COPY  input-filespec[,...] output-filespec
   



  Additional information available:

  Parameters Qualifiers
  /ALLOCATION           /BACKUP    /BEFORE    /BY_OWNER  /CONCATENATE
  /CONFIRM   /CONTIGUOUS           /CREATED   /EXCLUDE   /EXPIRED
  /EXTENSION /LOG       /MODIFIED  /OVERLAY   /PROTECTION
  /READ_CHECK           /REPLACE   /SINCE     /STYLE     /TRUNCATE  /VOLUME
  /WRITE_CHECK
  /FTP       /RCP
*/
{
	return(DCL$SUCCESS);
}
