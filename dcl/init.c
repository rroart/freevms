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

/*
================================================================================
  This function returns DCL$FAILURE when it fails, DCL$SUCCESS
  in all other cases.
================================================================================
*/

int
commands_init(dcl$command **commands)
{
	/*
	 * VERBS
	 */

	if ((*commands = command_add_on(*commands,
			"COPY", "", NULL, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"DIRECTORY", "", directory_function, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"LOGOUT", "", logout_function, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SET", "", set_function, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SEARCH", "", NULL, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SHOW", "", NULL, DCL$VERB))
			== NULL) return(DCL$FAILURE);

	/*
	 * KEYWORDS
	 */

	if ((*commands = command_add_on(*commands,
			"DEFAULT", "", NULL, DCL$KEYWORD))
			== NULL) return(DCL$FAILURE);

	/*
	 * QUALIFIERS
	 */

	if ((*commands = command_add_on(*commands,
			"/ACL", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BACKUP", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BEFORE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BRIEF", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BY_OWNER", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/COLUMNS", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/CREATED", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/DATE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXACT", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXCLUDE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXPIRE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FILE_ID", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FTP", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FULL", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/GRAND_TOTAL", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/HEADING", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/HIGHLIGHT", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/MODIFIED", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/OUTPUT", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/OWNER", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PAGE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PRINTER", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PROTECTION", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SEARCH", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SECURITY", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SELECT", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SHELVED_STATE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SINCE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SIZE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/STYLE", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TIME", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TOTAL", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TRAILING", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/VERSIONS", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/WIDTH", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/WRAP", "", NULL, DCL$QUALIFIER))
			== NULL) return(DCL$FAILURE);

	return(DCL$SUCCESS);
}


/*
================================================================================
  This function adds a new DCL command to the current list. It returns the
  new base list or a NULL pointer if the internal malloc() fails.
  When the add on fails, the command list is freed.
================================================================================
*/

dcl$command *
command_add_on(dcl$command *commands, unsigned char *name,
		unsigned char *help, int (*function)(), int type)
{
	dcl$command			*new;

	if ((new = malloc(sizeof(dcl$command))) != NULL)
	{
		(*new).next = commands;
		(*new).name = name;
		(*new).length = strlen(name);
		(*new).help = help;
		(*new).type = type;
		(*new).function = function;
	}
	else
	{
		commands_freeing(commands);
	}

	return(new);
}


void
commands_freeing(dcl$command *commands)
{
	dcl$command		*current;
	dcl$command		*next;

	current = commands;

	while(current != NULL)
	{
		next = (*current).next;
		free(current);
		current = next;
	}

	return;
}
