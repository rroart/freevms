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
	dcl$command				*commands;
	dcl$env					env;

	int						flag;
	int						i;
	int						length;
	int						nb_options = 3;
	int						options[nb_options];

	struct sigaction		action;

	unsigned char			*ptr;
	unsigned char			*qualifier;

	/*
	 * SIGINT management
	 */

	action.sa_handler = SIG_IGN;
	action.sa_flags = SA_NOMASK;

	if (sigaction(SIGINT, &action, NULL) != 0)
	{
		return(DCL$FAILURE);
	}
	
	/*
	 * dcl$env struct initialization
	 */

	if ((env.prompt = malloc(3 * sizeof(unsigned char))) == NULL)
	{
		return(DCL$FAILURE);
	}

	strcpy(env.prompt, "$ ");

	env.end_flag = 0;
	env.historic_length = 256;

	/*
	 * dcl$command struct initialization
	 */

	commands = NULL;
	if (commands_init(&commands) != 0)
	{
		return(DCL$FAILURE);
	}

	/*
	 * Command line
	 */

	for(i = 0; i < nb_options; options[i++] = 0);

	if (argc > 1)
	{
		while((--argc) > 0)
		{
			if ((*(++argv))[0] == '/')
			{
				ptr = (*argv)++;
				flag = 0;

				while((*ptr) != 0)
				{
					if (((*ptr) >= 'a') && ((*ptr) <= 'z')) (*ptr) -= 'a' - 'A';
					ptr++;
				}

				qualifier = "VERSION";
				length = (strlen(*argv) < strlen(qualifier))
						? strlen(*argv) : strlen(qualifier);

				if (strncmp(*argv, qualifier, length) == 0)
				{
					if (strlen(*argv) <= strlen(qualifier))
					{
						options[0] = 1;
						flag = 1;
					}
				}

				qualifier = "COPYRIGHT";
				length = (strlen(*argv) < strlen(qualifier))
						? strlen(*argv) : strlen(qualifier);

				if (strncmp(*argv, qualifier, length) == 0)
				{
					if (strlen(*argv) <= strlen(qualifier))
					{
						options[1] = 1;
						flag = 1;
					}
				}

				qualifier = "HELP";
				length = (strlen(*argv) < strlen(qualifier))
						? strlen(*argv) : strlen(qualifier);

				if (strncmp(*argv, qualifier, length) == 0)
				{
					if (strlen(*argv) <= strlen(qualifier))
					{
						options[2] = 1;
						flag = 1;
					}
				}

				if (flag == 0)
				{
					return(DCL$FAILURE);
				}
			}
			else
			{
				return(DCL$FAILURE);
			}
		}
	}

	for(i = 0; i < nb_options; i++)
	{
		if (options[i])
		{
			switch(i)
			{
				case 0 :
				if (fprintf(stdout, "DCL/2 Version %s (%s)\n",
						DCL$VERSION, DCL$DATE) < 0)
					return(DCL$FAILURE);
				break;

				case 1 :
				if (fprintf(stdout, "Copyright (C) 2001 BERTRAND Joël\n") < 0)
					return(DCL$FAILURE);
				break;

				case 2 :
				if (fprintf(stdout, "Avalaible options for DCL/2\n") < 0)
					return(DCL$FAILURE);
				if (fprintf(stdout, "  /COPYRIGHT\n") < 0)
					return(DCL$FAILURE);
				if (fprintf(stdout, "  /HELP\n") < 0)
					return(DCL$FAILURE);
				if (fprintf(stdout, "  /VERSION\n") < 0)
					return(DCL$FAILURE);

				return(DCL$SUCCESS);
				break;
			}
		}
	}

	/*
	 * Main loop
	 */

	if (loop(commands, &env) != DCL$SUCCESS)
	{
		return(DCL$FAILURE);
	}

	/*
	 * dcl$command list is freed
	 */

	commands_freeing(commands);
	return(DCL$SUCCESS);
}
