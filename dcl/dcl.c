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
main(int argc, char *argv[])
{
	dcl$command		*commands;
	dcl$env			env;

	unsigned char	*command_line;

	/*
	 * dcl$env struct initialization
	 */

	if ((env.prompt = malloc(3 * sizeof(unsigned char))) == NULL)
	{
		return(DCL$FAILURE);
	}

	strcpy(env.prompt, "$ ");
	env.end_flag = 0;

	/*
	 * dcl$command struct initialization
	 */

	commands = NULL;
	if (commands_init(&commands) != 0)
	{
		return(DCL$FAILURE);
	}

	/*
	 * Loop
	 */

	while(env.end_flag == 0)
	{
		command_line = read_command(&env);

		if (parsing() != 0)
		{
		}
	}

	/*
	 * dcl$command list is freed
	 */

	commands_freeing(commands);
	return(DCL$SUCCESS);
}
