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
  This function returns -1 when it fails, 0 in all other cases.
================================================================================
*/

int
commands_init(dcl$command **commands)
{
	if ((*commands = command_add_on(*commands,
			"DIRECTORY", "", directory_function, DCL$VERB))
			== NULL)
	{
		return(DCL$FAILURE);
	}

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
		(*new).help = help;
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
