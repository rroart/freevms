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
			"COPY", copy_function, DCL$VERB, DCL$NONE,
			DCL$Q_ALLOCATION, DCL$Q_BACKUP,
			DCL$Q_BEFORE, DCL$Q_BY_OWNER,
			DCL$Q_CONCATENATE, DCL$Q_CONFIRM,
			DCL$Q_CONTIGUOUS, DCL$Q_CREATED,
			DCL$Q_EXCLUDE, DCL$Q_EXPIRED,
			DCL$Q_EXTENSION, DCL$Q_FTP,
			DCL$Q_LOG, DCL$Q_MODIFIED,
			DCL$Q_OVERLAY, DCL$Q_PROTECTION,
			DCL$Q_RCP, DCL$Q_READ_CHECK,
			DCL$Q_REPLACE, DCL$Q_SINCE,
			DCL$Q_STYLE, DCL$Q_TRUNCATE,
			DCL$Q_VOLUME, DCL$Q_WRITE_CHECK,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"DIRECTORY", directory_function, DCL$VERB, DCL$NONE,
			DCL$Q_ACL, DCL$Q_BACKUP,
			DCL$Q_BEFORE, DCL$Q_BRIEF,
			DCL$Q_BY_OWNER, DCL$Q_COLUMNS,
			DCL$Q_CREATED, DCL$Q_DATE,
			DCL$Q_EXACT, DCL$Q_EXCLUDE,
			DCL$Q_EXPIRED, DCL$Q_FILE_ID,
			DCL$Q_FTP, DCL$Q_FULL,
			DCL$Q_GRAND_TOTAL, DCL$Q_HEADING,
			DCL$Q_HIGHLIGHT, DCL$Q_MODIFIED,
			DCL$Q_OUTPUT, DCL$Q_OWNER,
			DCL$Q_PAGE, DCL$Q_PRINTER,
			DCL$Q_PROTECTION, DCL$Q_SEARCH,
			DCL$Q_SECURITY, DCL$Q_SELECT,
			DCL$Q_SHELVED_STATE, DCL$Q_SINCE,
			DCL$Q_SIZE, DCL$Q_STYLE,
			DCL$Q_TIME, DCL$Q_TOTAL,
			DCL$Q_TRAILING, DCL$Q_VERSIONS,
			DCL$Q_WIDTH, DCL$Q_WRAP,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"HELP", help_function, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"LOGOUT", logout_function, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"RUN", run_function, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SET", set_function, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SEARCH", NULL, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"SHOW", NULL, DCL$VERB, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	/*
	 * KEYWORDS
	 */

	if ((*commands = command_add_on(*commands,
			"DEFAULT", NULL, DCL$KEYWORD, DCL$NONE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	/*
	 * QUALIFIERS
	 */

	if ((*commands = command_add_on(*commands,
			"/ACL", NULL, DCL$QUALIFIER, DCL$Q_ACL,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/ALLOCATION", NULL, DCL$QUALIFIER, DCL$Q_ALLOCATION,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BACKUP", NULL, DCL$QUALIFIER, DCL$Q_BACKUP,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BEFORE", NULL, DCL$QUALIFIER, DCL$Q_BEFORE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BRIEF", NULL, DCL$QUALIFIER, DCL$Q_BRIEF,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/BY_OWNER", NULL, DCL$QUALIFIER, DCL$Q_BY_OWNER,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/COLUMNS", NULL, DCL$QUALIFIER, DCL$Q_COLUMNS,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/CONCATENATE", NULL, DCL$QUALIFIER, DCL$Q_CONCATENATE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/CONFIRM", NULL, DCL$QUALIFIER, DCL$Q_CONFIRM,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/CONTIGUOUS", NULL, DCL$QUALIFIER, DCL$Q_CONTIGUOUS,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/CREATED", NULL, DCL$QUALIFIER, DCL$Q_CREATED,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/DATE", NULL, DCL$QUALIFIER, DCL$Q_DATE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXACT", NULL, DCL$QUALIFIER, DCL$Q_EXACT,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXCLUDE", NULL, DCL$QUALIFIER, DCL$Q_EXCLUDE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXPIRED", NULL, DCL$QUALIFIER, DCL$Q_EXPIRED,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/EXTENSION", NULL, DCL$QUALIFIER, DCL$Q_EXTENSION,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FILE_ID", NULL, DCL$QUALIFIER, DCL$Q_FILE_ID,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FTP", NULL, DCL$QUALIFIER, DCL$Q_FTP,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/FULL", NULL, DCL$QUALIFIER, DCL$Q_FULL,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/GRAND_TOTAL", NULL, DCL$QUALIFIER, DCL$Q_GRAND_TOTAL,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/HEADING", NULL, DCL$QUALIFIER, DCL$Q_HEADING,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/HIGHLIGHT", NULL, DCL$QUALIFIER, DCL$Q_HIGHLIGHT,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/LOG", NULL, DCL$QUALIFIER, DCL$Q_LOG,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/MODIFIED", NULL, DCL$QUALIFIER, DCL$Q_MODIFIED,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/OUTPUT", NULL, DCL$QUALIFIER, DCL$Q_OUTPUT,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/OVERLAY", NULL, DCL$QUALIFIER, DCL$Q_OVERLAY,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/OWNER", NULL, DCL$QUALIFIER, DCL$Q_OWNER,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PAGE", NULL, DCL$QUALIFIER, DCL$Q_PAGE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PRINTER", NULL, DCL$QUALIFIER, DCL$Q_PRINTER,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/PROTECTION", NULL, DCL$QUALIFIER, DCL$Q_PROTECTION,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/RCP", NULL, DCL$QUALIFIER, DCL$Q_RCP,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/READ_CHECK", NULL, DCL$QUALIFIER, DCL$Q_READ_CHECK,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/REPLACE", NULL, DCL$QUALIFIER, DCL$Q_REPLACE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SEARCH", NULL, DCL$QUALIFIER, DCL$Q_SEARCH,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SECURITY", NULL, DCL$QUALIFIER, DCL$Q_SECURITY,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SELECT", NULL, DCL$QUALIFIER, DCL$Q_SELECT,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SHELVED_STATE", NULL, DCL$QUALIFIER, DCL$Q_SHELVED_STATE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SINCE", NULL, DCL$QUALIFIER, DCL$Q_SINCE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/SIZE", NULL, DCL$QUALIFIER, DCL$Q_SIZE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/STYLE", NULL, DCL$QUALIFIER, DCL$Q_STYLE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TIME", NULL, DCL$QUALIFIER, DCL$Q_TIME,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TOTAL", NULL, DCL$QUALIFIER, DCL$Q_TOTAL,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TRAILING", NULL, DCL$QUALIFIER, DCL$Q_TRAILING,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/TRUNCATE", NULL, DCL$QUALIFIER, DCL$Q_TRUNCATE,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/VERSIONS", NULL, DCL$QUALIFIER, DCL$Q_VERSIONS,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/VOLUME", NULL, DCL$QUALIFIER, DCL$Q_VOLUME,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/WIDTH", NULL, DCL$QUALIFIER, DCL$Q_WIDTH,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/WRAP", NULL, DCL$QUALIFIER, DCL$Q_WRAP,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

	if ((*commands = command_add_on(*commands,
			"/WRITE_CHECK", NULL, DCL$QUALIFIER, DCL$Q_WRITE_CHECK,
			DCL$END_OF_LIST)) == NULL) return(DCL$FAILURE);

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
		int (*function)(), int type, int qualifier, ...)
{
	dcl$command			*new;

	int					compatibility;
	int					i;

	va_list				list;

	if ((new = malloc(sizeof(dcl$command))) != NULL)
	{
		(*new).next = commands;
		(*new).name = name;
		(*new).length = strlen(name);
		(*new).type = type;
		(*new).function = function;
		(*new).qualifier = qualifier;

		for(i = 0; i < DCL$NB_COMP; (*new).compat[i++] = 0);

		va_start(list, qualifier);

		do
		{
			if ((compatibility = va_arg(list, int)) != DCL$END_OF_LIST)
			{
				if ((compatibility > 0) && (compatibility <= DCL$NB_FLAGS))
				{
					set_compat_flag(new, compatibility);
				}
			}
		} while(compatibility != DCL$END_OF_LIST);

		va_end(list);
	}
	else
	{
		commands_freeing(commands);
	}

	return(new);
}


void
set_compat_flag(dcl$command *command, int flag)
{
	int				i;
	int				j;
	int				number_of_rotations;

	number_of_rotations = flag - 1;

	i = number_of_rotations / (8 * sizeof(unsigned char));
	j = number_of_rotations % (8 * sizeof(unsigned char));

	(*command).compat[i] |= 1 << j;

	return;
}


int
get_compat_flag(dcl$command *command, int flag)
{
	int				i;
	int				j;
	int				number_of_rotations;

	number_of_rotations = flag - 1;

	i = number_of_rotations / (8 * sizeof(unsigned char));
	j = number_of_rotations % (8 * sizeof(unsigned char));

	return((((*command).compat[i] & (1 << j)) != 0) ? -1 : 0);
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


unsigned char *
next_argument(unsigned char *ptr)
{
	while((*ptr) == ' ') ptr++;
	return(ptr);
}
