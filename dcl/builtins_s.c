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
set_function(unsigned char *argument, dcl$env *env)
/*
SET

     Defines or changes, for the current terminal session or batch
     job, characteristics associated with files and devices owned by
     the process.

     Format

       SET  option
   



  Additional information available:

  Description           ACCOUNTING ACL        AUDIT      BROADCAST
  CARD_READER           CLUSTER    COMMAND    CONTROL    CPU        DAY
  DEFAULT    DEVICE     DIRECTORY  DISPLAY    ENTRY      FILE       HOST
  KEY        LOGINS     MAGTAPE    MESSAGE    NETWORK    ON
  OUTPUT_RATE           PASSWORD   PREFERRED_PATH        PREFIX     PRINTER
  PROCESS    PROMPT     PROTECTION QUEUE      RESTART_VALUE
  RIGHTS_LIST           RMS_DEFAULT           SECURITY   SERVER     SYMBOL
  TERMINAL   TIME       UIC        VERIFY     VOLUME     WORKING_SET
*/
{
	return(DCL$SUCCESS);
}
