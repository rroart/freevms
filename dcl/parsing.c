/*
================================================================================
  DCL/2 version 1.00
  	Digital Command Language

    Date de cr�ation    : 28 novembre 2001

    Tous droits r�serv�s � l'auteur, Jo�l BERTRAND
	All rights reserved worldwide
================================================================================
*/


/*
================================================================================
  Copyright (C) 2001 BERTRAND Jo�l

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
parsing(unsigned char *line, dcl$command *commands, dcl$env *env,
		int required_type, dcl$command *parent)
{
	dcl$command		*current_command;
	dcl$command		*first_target;

	int				i;
	int				length;
	int				negative_qualifier;
	int				number_of_targets;
	int				status;
	int				string_flag;

	unsigned char	*ptr1;
	unsigned char	*ptr2;
	unsigned char	*ptr3;
	unsigned char	*string;

	ptr1 = next_argument(line);
	ptr2 = ptr1;

	string_flag = 0;
	while((((*ptr2) != ' ') && ((*ptr2) != 0)) || (string_flag != 0))
	{
		if ((*ptr2) == '"') string_flag = (string_flag == 0) ? -1 : 0;
		ptr2++;
	}

	if (string_flag != 0) return(DCL$FAILURE);

	/*
	 * "length" variable contains the length of the first element of the
	 * command line
	 */

	length = ptr2 - ptr1;

	if ((string = malloc((length + 1) * sizeof(unsigned char))) == NULL)
	{
		return(DCL$FAILURE);
	}

	for(ptr3 = string, ptr2 = ptr1, i = 0;
			(*ptr3) = (*ptr2), i < length;
			i++, ptr2++, ptr3++)
		if (((*ptr2) >= 'a') && ((*ptr2) <= 'z'))
			(*ptr3) -= 'a' - 'A';

	(*ptr3) = 0;

	/*
	 * Analysis
	 */

	current_command = commands;
	number_of_targets = 0;
	first_target = NULL;

	if (required_type == DCL$QUALIFIER)
	{
		if (strlen(string) > 3)
		{
			if (strncmp(string, "/NO", 3) == 0)
			{
				ptr2 = ((ptr1 = string + 1) + 2);
				while((*ptr2) != 0) *ptr1++ = *ptr2++;
				(*ptr1) = 0;

				negative_qualifier = 1;
				length -= 2;
			}
			else
			{
				negative_qualifier = 0;
			}
		}
		else
		{
			negative_qualifier = 0;
		}
	}
	else
	{
		negative_qualifier = 0;
	}

	while(current_command != NULL)
	{
		if ((*current_command).length >= length)
		{
			if (required_type == (*current_command).type)
			{
				if (strncmp((*current_command).name, string, length) == 0)
				{
					if (parent == NULL)
					{
						first_target = current_command;
						number_of_targets++;
					}
					else if (get_compat_flag(parent,
							(*current_command).qualifier) != 0)
					{
						printf("Found ! (%s)\n", (*current_command).name);
						first_target = current_command;
						number_of_targets++;
					}
				}
			}
		}

		current_command = (*current_command).next;
	}

	if (negative_qualifier != 0)
	{
		ptr2 = (ptr1 = string + length) - 2;
		i = length;
		while(--i) *ptr1-- = *ptr2--;
		(*(ptr1 = string + 1)) = 'N';
		(*(++ptr1)) = 'O';
		length += 2;
	}

	if (number_of_targets == 1)
	{
		if ((*first_target).function != NULL)
		{
			status = (*first_target).function(ptr2, first_target,
					commands, env);
		}
		else
		{
			if (fprintf(stderr, "Function not written yet !\n") < 0)
			{
				status = DCL$FAILURE;
			}
			else
			{
				status = DCL$SUCCESS;
			}
		}
	}
	else if (number_of_targets > 1)
	{
		(*env).last_error = string;
		/* V�rifier le message pour les keywords et qualifiers */
		return(DCL$WABVERB);
	}
	else
	{
		(*env).last_error = string;

		if (required_type == DCL$VERB)
		{
			return(DCL$WIVVERB);
		}
		else if (required_type == DCL$KEYWORD)
		{
			return(DCL$WIVKEYW);
		}
		else
		{
			return(DCL$WIVQUAL);
		}
	}

	free(string);

	return(status);
}
