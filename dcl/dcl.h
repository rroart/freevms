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

#ifndef DCL$DCL
#	define DCL$DCL

#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>

#	include <readline/readline.h>
#	include <readline/history.h>

/*
--------------------------------------------------------------------------------
  DCL return values
--------------------------------------------------------------------------------
*/

#	define		DCL$FAILURE		-1
#	define		DCL$SUCCESS		0

/*
--------------------------------------------------------------------------------
  DCL errors
--------------------------------------------------------------------------------
*/

//	ambiguous command verb - supply more characters
#	define		DCL$WABVERB		1
//	unrecognized command verb - check validity and spelling
#	define		DCL$WIVVERB		2
//	unrecognized keyword - check validity and spelling
#	define		DCL$WIVKEYW		3
//	unrecognized qualifier - check validity, spelling, and placement
#	define		DCL$WIVQUAL		4

/*
--------------------------------------------------------------------------------
  DCL command structure
--------------------------------------------------------------------------------
*/

#	define		DCL$VERB		0x01
#	define		DCL$KEYWORD		0x02
#	define		DCL$QUALIFIER	0x04

	typedef struct dcl_env
	{
		int						end_flag;
		unsigned char			*last_error;
		unsigned char			*prompt;
	} dcl$env;

	typedef struct dcl_command
	{
		unsigned char			*name;		// static not allocated by malloc()
		unsigned char			*help;		// static not allocated by malloc()
		int						(*function)(unsigned char *argument,
										dcl$env *env);
		int						type;
		int						length;
		struct dcl_command		*next;
	} dcl$command;


/*
--------------------------------------------------------------------------------
  Protoypes
--------------------------------------------------------------------------------
*/

	dcl$command *command_add_on(dcl$command *commands,
			unsigned char *name, unsigned char *help,
			int (*function)(), int type);

	int commands_init(dcl$command **commands);
	int loop(dcl$command *commands, dcl$env *env);
	int parsing(unsigned char *line, dcl$command *commands, dcl$env *env,
			int required_type);

	unsigned char *read_command(dcl$env *env);

	void commands_freeing(dcl$command *commands);

/*
--------------------------------------------------------------------------------
  Builtins
--------------------------------------------------------------------------------
*/

	int directory_function(unsigned char *argument, dcl$env *env);

	int logout_function(unsigned char *argument, dcl$env *env);

	int set_function(unsigned char *argument, dcl$env *env);
#endif
